#include <stdio.h>
#include <ctype.h>

typedef void* ((*func_ptr)());

static void *func_error()
{
    fprintf(stderr, "func_error in DynCall\n");
    return 0;
}

#ifdef sun

#if defined(sun4) || defined(sparc)
#       include <sun4/a.out.h>
#       undef relocation_info
#       define relocation_info reloc_info_sparc
#       define r_symbolnum r_index
#       define MASK(n) ((1<<(n))-1)
#       define M(a,d,n) *(long*)(a)= (*(long*)(a) & ~MASK(n)) | ((d) & MASK(n))
#else
#       include <a.out.h>
#       undef relocation_info
#       define relocation_info reloc_info_68k
#endif /* sun4 */

#include <sys/file.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>

#define bad_format 2
#define ok 0

/* The hash value is a function of the table size. */
#define hash(h,ss,m) { register char *_s=ss; for(h=0; *_s;) h+=((h<<1)+*_s++); h&=m; }

#define REHASH(hash, table) (((hash+1)*3) & (table)->mask)

#define INIT_TABLE_SIZE 8192    /* Must be a power of two */


typedef struct {
    struct nlist **array; 
    int size;           /* Number of slots in table.. power of 2 */
    int mask;           /* always = size-1, for calculating (num mod size)*/
    int entries;        /* number of entries in AssocTable */
} AssocTable;

struct Module {
    char *name, *name1;
    int fd;
    struct Module *next;
    struct nlist *symtab, *endp;
    long text;
    func_ptr dtor;
    int undefs;
    struct exec header;
};

extern char *malloc(), *calloc(), *strcpy(), *getenv();

static AssocTable *mainhashtable= 0;
static struct Module mainmodule;
static struct Module *firstModule= 0;
static long new_common= 0;
static char *etdir;
static char *STD, *STI;
static int STDlen, STIlen;
static int ccversion= 1;

static char *strsave(s)
char *s;
{
    return strcpy(malloc(strlen(s)+1), s);
}

static struct Module *FindModule(name)
char *name;
{
    register struct Module *mp;
    
    for (mp= firstModule; mp; mp= mp->next)
	if (strcmp(name, mp->name) == 0)
	    return mp;
    return 0;
}

static struct Module *AddModule(name, name1)
char *name, *name1;
{
    struct Module *mp;
    
    mp= (struct Module *) calloc(1, sizeof(struct Module));
    mp->next= firstModule;
    mp->undefs= 0;
    mp->fd= -1;
    mp->name= strsave(name);
    mp->name1= strsave(name1);
    mp->dtor= 0;
    
    return firstModule= mp;
}

static AssocTable *NewAssocTable()
{
    register AssocTable* table;

    table= (AssocTable*) calloc(1, sizeof(AssocTable));

    table->size= INIT_TABLE_SIZE;
    table->mask= table->size-1;
    table->array= (struct nlist**) calloc(table->size, sizeof(struct nlist*));

    return table;
}

static void ExpandTable(table)
register AssocTable* table;
{
    register struct nlist **old_table, **sym, **endp, *sp;
    register int rehash;
    int old_size;
 
    old_table= table->array;
    old_size= table->size;
    table->size*= 2;
    table->mask= table->size-1;
    table->array= (struct nlist**) calloc(table->size, sizeof(struct nlist*));

    /* Move the members from the old small table to the new big one. */
    sym= old_table;
    endp= sym + old_size;
    for (; sym < endp; sym++) {
	if (sp= *sym) {
	    hash(rehash, sp->n_un.n_name, table->mask);
	    while (table->array[rehash])
		rehash= REHASH(rehash, table);
	    table->array[rehash]= sp;
	}
    }
    free(old_table);
}

static void Assoc(table, sym)
register AssocTable* table;
struct nlist *sym;
{
    register int rehash;
    struct nlist *s;

    if (table->entries >= (table->size/2) - 1)
	ExpandTable(table);

    hash(rehash, sym->n_un.n_name, table->mask);
    while (s= table->array[rehash]) {
	if (strcmp(s->n_un.n_name, sym->n_un.n_name) == 0)
	    return;
	rehash= REHASH(rehash, table);
    }
    table->array[rehash]= sym;
    table->entries++;
}

static struct nlist *Lookup(table, name)
register AssocTable* table;
char *name;
{
    register struct nlist *syms;
    register int rehash;

    hash(rehash, name, table->mask);
    while (syms= table->array[rehash]) {
	if (strcmp(syms->n_un.n_name, name) == 0)
	    return syms;
	rehash= REHASH(rehash, table);
    }
    return 0;
}

static char *which(search, file)
char *search, *file;
{
    static char name[MAXPATHLEN];
    register char *ptr, *next;
    struct stat buffer;

    if (file[0] == '/') {
	if (stat(file, &buffer) != -1)
	    return file;
	return 0;
    }

    if (search == 0)
	search= ".";

    for (ptr= search; *ptr;) {
	for (next= name; *ptr && *ptr != ':'; )
	    *next++= *ptr++;
	*next= '\0';
	   
	strcat(name, "/");
	strcat(name, file);
	   
	if (stat(name, &buffer) != -1)
	    return name;
	if (*ptr)
	    ptr++;
    }
    return 0;
}

static void GetHeader(mp)
struct Module *mp;
{
    lseek(mp->fd, 0, L_SET);
    if (read(mp->fd, &mp->header, sizeof(struct exec)) != sizeof(struct exec)) {
	perror("GetHeader: read error");
	_exit(1);
    }
}

static void GetSymbols(mp)
struct Module *mp;
{
    register struct nlist *buffer, *sym;
    unsigned long size;
    long displ;

    lseek(mp->fd, N_SYMOFF(mp->header) + mp->header.a_syms, L_SET);
    if (read(mp->fd, &size, 4) != 4) {
	perror("GetSymbols: read error");
	_exit(1);
    }

    buffer= (struct nlist*) calloc(1, mp->header.a_syms + size);
  
    lseek(mp->fd, N_SYMOFF(mp->header), L_SET);
    if (read(mp->fd, buffer, mp->header.a_syms + size) != mp->header.a_syms + size) {
	free(buffer);
	perror("GetSymbols: read error");
	_exit(1);
    }
  
    mp->symtab= buffer;
    mp->endp= buffer + (mp->header.a_syms/sizeof(struct nlist));
    
    displ= (long)buffer + (long)(mp->header.a_syms);

    for (sym= buffer; sym < mp->endp; sym++)
	sym->n_un.n_name= (char*) sym->n_un.n_strx + displ;
}

static struct relocation_info *GetRelocInfo(mp)
struct Module *mp;
{
    struct relocation_info *buffer;
    int size= mp->header.a_trsize + mp->header.a_drsize;
  
    if (size == 0) {
	fprintf(stderr, "no relocation info\n");
	return 0;
    }
    
    buffer= (struct relocation_info*) calloc(1, size);

    lseek(mp->fd, N_TXTOFF(mp->header) + mp->header.a_text + mp->header.a_data, L_SET);
  
    if (read(mp->fd, buffer, size) != size) {
	perror("GetRelocInfo: read");
	free(buffer);
	return 0;
    }
  
    return buffer;
}

static void GetTextAndData(mp)
struct Module *mp;
{
    int size, rsize;
    
    lseek(mp->fd, N_TXTOFF(mp->header), L_SET);
  
    rsize= mp->header.a_text + mp->header.a_data;
    size= rsize + mp->header.a_bss;

    mp->text= (long) malloc(size);
  
    if (read(mp->fd, mp->text, rsize) !=  rsize) {
	perror("GetTextAndData: read error");
	free(mp->text);
	mp->text= 0;
	return;
    }
    
    bzero(mp->text + mp->header.a_text + mp->header.a_data, mp->header.a_bss);
}

static void FindAll(mp, hashtable)
struct Module *mp;
AssocTable* hashtable;
{
    register struct nlist *symp= mp->symtab, *symbol;
    int value;

    mp->undefs= 0;
    for (; symp < mp->endp; symp++) {
	if (symp->n_type & N_EXT) {
	    symbol= Lookup(hashtable, symp->n_un.n_name);

	    if ((symp->n_type == N_EXT + N_UNDF) && (symbol == 0)) {
		value= symp->n_value;
		if (value > 0) {    /* is common */
		    int rnd= value >= sizeof(double) ? sizeof(double) - 1
			       : value >= sizeof(long) ? sizeof(long) - 1
			       : sizeof(short) - 1;

		    symp->n_type= N_COMM;
		    new_common+= rnd;
		    new_common&= ~(long)rnd;
		    symp->n_value= new_common;
		    new_common+= value;
		    Assoc(hashtable, symp);
		} else {
		    mp->undefs++;
		    try(symp->n_un.n_name);
		}
	    } else if (symbol == 0) /* is new */
		Assoc(hashtable, symp);
	}
    }
}

static int Testload(name)
char* name;
{
    char buf[100], buf2[100], *name2, *dynpath;
    struct Module *mp;
    
    sprintf(buf, "%s.o", name);
    
    dynpath= getenv("ET_DYN_PATH");
    if (dynpath == 0) {
#if defined(sun4) || defined(sparc)
	sprintf(buf2, ".:%s/dyn.sparc", etdir);
#else
	sprintf(buf2, ".:%s/dyn.68020", etdir);
#endif
	dynpath= buf2;
    }

    name2= which(dynpath, buf);
    if (name2 == 0)
	return 0;

    if (FindModule(name2))
	return 1;
	
    mp= AddModule(name2, name);
    
    if ((mp->fd= open(name2, O_RDONLY)) < 0)
	return 0;
	
    fprintf(stderr, "\t%s\n", name2);
    
    GetHeader(mp);
    GetSymbols(mp);

    close(mp->fd);
    return 1;
}

static int try(s)
char *s;
{
    char buf[200], name[100];
    int code= 0, l;
    
    switch (ccversion) {
    case 1:
	if (s[0] == '_' && s[1] == '_') {
	    char *st= &s[2];
	    s+= 2;
	    while (*s++ != '_')
		;
	    if (*s++ == '_') {
		if (s[0] == 'c' && s[1] == 't' && s[2] == 'o' && s[3] == 'r') {
		    l= s-2-st;
		    strncpy(buf, st, l);
		    buf[l]= '\0';
		    Testload(buf);
		    code= 1;
		}
	    }
	}
	break;
    case 2:
	if (strncmp(s, "___ct__", 7) == 0) {
	    if (sscanf(&s[7], "%d%s", &l, name) == 2) {
		strncpy(buf, name, l);
		buf[l]= '\0';
		Testload(buf);
		code= 1;
	    }
	}
	break;
    case 3:

#if 0 /* Naming changed for g++1.37, tom@izf.tno.nl */
/*
   The idea here is that we are given the name of a constructor of
   an object with the same name as the file it is in. For GNU g++
   1.35.1, this will look like "_bar_PSbar...", so we look in
   bar.o.
*/
    if (s[0] == '_')
      {  
	char *st1, *st2;
	int l;
 
	s += 1;
	st1 = s;                        /* Start of first part of symbol. */
	while (*s != '_' && *s != '\0')
	  s++;
	if (s[0] == '_' && s[1] == 'P' && s[2] == 'S')
	  {
	    s += 3;
	    st2 = s;                    /* Start of second part of symbol. */
	    while (*s != '_' && *s != '\0')
	      s++;
	    l = s-st2;
	    if (!strncmp(st1, st2, l))  /* Check they match */
	      {
		char buf[200];
		strncpy(buf, st2, l);
		buf[l] = '\0';
		code = Testload(buf);   /* Try to load the file. */
	      }
	  }
      }
#else
    /* Look for: ___[0-9]+name.*  tom@izf.tno.nl */

    if (s[0] == '_' && s[1] == '_' && s[2] == '_')
      {  
	char *st1, *st2;
	int l;
	char num[100], *nump= num;
 
	s += 3;
	st1 = s;                        /* Start of first part of symbol. */
	while (*s != '\0' && isdigit(*s))
	  *nump++= *s++;

	if (s != st1)
	  {
	    char buf[1000];
	    int len;

	    *nump= '\0';
	    len=atoi(num);

	    strncpy(buf, s, len);
	    buf[len]= '\0';

	    fprintf(stderr, "Trying: %s\n", buf);
	    code= Testload(buf);
	  }
      }
#endif
	break;
    }

    return code;
}

static void Resolve(mp, hashtable)
struct Module *mp;
AssocTable* hashtable;
{
    register struct nlist *symbol, *symp;
    int value;

    mp->undefs= 0;
    for (symp= mp->symtab; symp < mp->endp; symp++) {
	if (symp->n_type == N_EXT + N_UNDF) {   /* is not defined here yet. */
	    if (symbol= Lookup(hashtable, symp->n_un.n_name)) {
		if (symbol->n_type == N_COMM)
		    symp->n_type= N_COMM;
		else
		    symp->n_type= N_EXT + N_COMM;
		symp->n_value= symbol->n_value;
	    } else {
		if (value= symp->n_value) {    /* is common */
		    int rnd= value >= sizeof(double) ? sizeof(double) - 1
			       : value >= sizeof(long) ? sizeof(long) - 1
			       : sizeof(short) - 1;

		    symp->n_type= N_COMM;
		    new_common+= rnd;
		    new_common&= ~(long)rnd;
		    symp->n_value= new_common;
		    new_common+= value;
		} else {  /* is extern */
		    fprintf(stderr, "undef %s\n", symp->n_un.n_name);
		    mp->undefs++;
		}
	    }
	}
    }
}

static void UpdateSymtab(mp)
register struct Module *mp;
{
    register struct nlist *symp;

    for (symp= mp->symtab; symp < mp->endp; symp++)
	if (symp->n_type == N_TEXT+N_EXT || symp->n_type == N_DATA+N_EXT)
	    symp->n_value+= mp->text;
}

static int Relocate(mp, common)
register struct Module *mp;
long common;
{
    struct relocation_info *reloc;
    char *address;
    long datum;
    register struct relocation_info *rel, *first_data, *endp;
    struct nlist *symbols= mp->symtab;

    reloc= GetRelocInfo(mp);
    if (reloc == 0)
	return 0;
	
    /* text relocation */
	
    first_data= reloc + (mp->header.a_trsize  / sizeof(struct relocation_info));
    endp= reloc + (mp->header.a_trsize+mp->header.a_drsize )/ sizeof(struct relocation_info);

    for (rel= reloc; rel < endp; rel++) {
	address= (char*) (rel->r_address + mp->text);

	if (rel >= first_data)
	    address+= mp->header.a_text;
      
#if defined(sun4) || defined(sparc)
	datum= rel->r_addend;
#else
	switch (rel->r_length) {
	case 0:        /* byte */
	    datum= *address;
	    break;
	case 1:        /* word */
	    datum= *(short*) address;
	    break;
	case 2:        /* long */
#ifdef mc68000
	    datum= *((long*) address);
#else /* mc680xx */
	    *((short*)(&datum))= *((short*)address);
	    *((short*)(((char*)(&datum))+2))= *((short*)(address+2));
#endif /* mc68000 */
	    break;
	default:
	    return bad_format;
	}
#endif
	if (rel->r_extern) {
	    struct nlist *symbol= &symbols[rel->r_symbolnum];
	    
	    switch (symbol->n_type) {
	    case N_EXT + N_COMM:    /* old common or external */
		datum+= symbol->n_value;
		break;
	    case N_COMM:            /* new common */
		/* datum+= mp->text + mp->header.a_text + mp->header.a_data; */
		datum+= common;
		break;
	    default:
		return bad_format;
	    }
	} else {                /* is static */
	    switch (rel->r_symbolnum & N_TYPE) {
	    case N_TEXT:
	    case N_DATA:
		datum+= mp->text;
		break;
	    case N_BSS:
		datum+= mp->text /* + new_common */;
		break;
	    case N_ABS:
		break;
	    }
	}
#if defined(sun4) || defined(sparc)
	switch (rel->r_type) {
	case RELOC_DISP8:
	case RELOC_DISP16:
	case RELOC_DISP32:
	case RELOC_WDISP30:
	case RELOC_WDISP22:
		datum-= mp->text;
	}
	switch (rel->r_type) {
	case RELOC_8: case RELOC_DISP8:
		*address= datum;
		break;
	case RELOC_LO10: case RELOC_PC10: case RELOC_BASE10:
		M(address,datum,10);
		break;
	case RELOC_BASE13: case RELOC_13:
		M(address,datum,13);
		break;
	case RELOC_16: case RELOC_DISP16:
		*(short*)address= datum;
		break;
	case RELOC_22:
		M(address,datum,22);
		break;
	case RELOC_HI22: case RELOC_BASE22: case RELOC_PC22:
		M(address, datum >> 10, 22);
		break;
	case RELOC_WDISP22:
		M(address, datum >> 2, 22);
		break;
	case RELOC_JMP_TBL: case RELOC_WDISP30:
		M(address, datum >> 2, 30);
		break;
	case RELOC_32: case RELOC_DISP32:
		*(long*)address= datum;
		break;
	}
#else
	if (rel->r_pcrel)
	    datum-= mp->text;

	switch (rel->r_length) {
	case 0:      /* byte */
	    if (datum < -128 || datum > 127)
		return bad_format;
	    *address= datum;
	    break;
	case 1:      /* word */
	    if (datum < -32768 || datum > 32767) 
		return bad_format;
	    *(short*) address= datum;
	    break;
	case 2:      /* long */
#ifdef mc68000
	    *(long*) address= datum;
#else /* mc680xx */
	    *((short*) address)= *((short*) (&datum));
	    *((short*)(address+2)= *((short*)(((char*)(&datum))+2)));
#endif /* mc680xx */
	    break;
	}
#endif
    }
    
    free(reloc);

    return ok;
}

int DynLoad(name)
char *name;
{
    char buf[100];
    int allundefs, lastundefs, code= 0;
    struct Module *mp;
    long common= 0;
    struct nlist *symbol, *symp;
    func_ptr ctor;
    
    fprintf(stderr, "loading:\n");
    Testload(name);
    
    lastundefs= 0;
    for (;;) {
	allundefs= 0;
	for (mp= firstModule; mp; mp= mp->next) {
	    FindAll(mp, mainhashtable);
	    allundefs+= mp->undefs;
	}
	if (allundefs == lastundefs)
	    break;
	lastundefs= allundefs;
    }
    
    if (allundefs > 0) {
	int first= 1;
	for (mp= firstModule; mp; mp= mp->next) {
	    if (mp->undefs <= 0)
		continue;
	    
	    for (symp= mp->symtab; symp < mp->endp; symp++)
		if ((symp->n_type & N_EXT)
			    && Lookup(mainhashtable, symp->n_un.n_name) == 0) {
		    if (first) {
			fprintf(stderr, "undefined symbols:\n");
			first= 0;
		    }
		    fprintf(stderr, "\t%s\n", symp->n_un.n_name);
		}
	}
	_exit(1);
    }
    
    if (new_common > 0)
	common= (long) calloc(1, new_common);
    new_common= 0;
    
    for (mp= firstModule; mp; mp= mp->next) {
	if ((mp->fd= open(mp->name, O_RDONLY)) < 0)
	    perror("open");
	    
	Resolve(mp, mainhashtable);
	allundefs+= mp->undefs;
	lastundefs= allundefs;
	GetTextAndData(mp);
	Relocate(mp, common);
	UpdateSymtab(mp);
	close(mp->fd);
    }
    
    fprintf(stderr, "static ctors:\n");
    for (mp= firstModule; mp; mp= mp->next) {
	for (symp= mp->symtab; symp < mp->endp; symp++) {
	    if ((symp->n_type & N_EXT) && symp->n_un.n_name[1] == '_') {
		if (strncmp(symp->n_un.n_name, STI, STIlen) == 0) {
		    if (symbol= Lookup(mainhashtable, symp->n_un.n_name)) {
			ctor= (func_ptr) symbol->n_value;
			if (ctor) {
			    fprintf(stderr, "\t%s\n", symp->n_un.n_name);
			    (*ctor)();            /* call static constructors */
			}
		    }
		} else if (strncmp(symp->n_un.n_name, STD, STDlen) == 0) {
		    if (symbol= Lookup(mainhashtable, symp->n_un.n_name))
			mp->dtor= (func_ptr) symbol->n_value;
		}
	    }
	}
    }
    firstModule= 0;
    return code;  
}

static struct nlist *Copy(sym)
struct nlist *sym;
{
    struct nlist *tmp;
    
    tmp= (struct nlist*) calloc(1, sizeof(struct nlist));
    *tmp= *sym;
    tmp->n_un.n_name= strsave(sym->n_un.n_name);
    return tmp;
}

void *dynlinkinit(me, ccvers, etd)
char *me, *etd;
int ccvers;
{
    register struct nlist *symp;
    char *name;
    
    if (mainhashtable)
	return;
	
    ccversion= ccvers;
    switch (ccversion) {
    case 1:     /* cfront 1.2 */
	STI= "__STI";
	STD= "__STD";
	break;
    case 2:     /* cfront 2.0 */
	STI= "___sti";
	STD= "___std";
	break;
    case 3:     /* G++ */
	STI= "__GLOBAL_$I$";
	STD= "__GLOBAL_$D$";
	break;
    }
    STIlen= strlen(STI);
    STDlen= strlen(STD);
    etdir= etd;
    name= which(getenv("PATH"), me);
    mainmodule.fd= open(name, O_RDONLY);
    GetHeader(&mainmodule);
    GetSymbols(&mainmodule);
    close(mainmodule.fd);
    
    mainhashtable= NewAssocTable();
    
    for (symp= mainmodule.symtab; symp < mainmodule.endp; symp++)
	if ((symp->n_type & N_EXT) && strcmp(symp->n_un.n_name, "_main"))
	    Assoc(mainhashtable, Copy(symp));
    free(mainmodule.symtab);
}

char *DynLookup(name)
char *name;
{
    struct nlist *symbol;
    
    if (symbol= Lookup(mainhashtable, name))
	return (char*) symbol->n_value;
    return 0;
}

static void CallDtors(mp)
struct Module *mp;
{
    if (mp) {
	CallDtors(mp->next);
	if (mp->dtor) {
	    fprintf(stderr, "dtor of %s\n", mp->name1);
	    (*mp->dtor)();            /* call static destructors */
	}
    }
}

void DynCleanup()
{
    if (firstModule) {
	CallDtors(firstModule);
	firstModule= 0;
    }
}

func_ptr DynCall(name)
char *name;
{
    char buf[100];
    struct nlist *symbol;
    
    sprintf(buf, "_%s", name);
    if (symbol= Lookup(mainhashtable, buf))
	return (func_ptr) symbol->n_value;
    return func_error;
}

#else sun

void *dynlinkinit(me, ccversion, etdir)
char *me, *etdir;
int ccversion;
{
    return 0;
}

char *DynLookup(name)
char *name;
{
    return 0;
}

func_ptr DynCall(name)
char *name;
{
    return func_error;
}

int DynLoad(name)
char *name;
{
    return 0;
}

#endif /* sun */
