#include "String.h"
#include "Error.h"

#define VPRINTF

static char *hexChars= "0123456789abcdef";
static int col;


#ifndef VPRINTF
#   ifdef DOPRNT

#       define vsprintf(s,f,p) _doprnt(f,p,s)
	extern "C" int _doprnt(const char*, va_list, char*);

#   else DOPRNT

	char *vsprintf(char *buf, const char *fmt, va_list ap)
	{
	    int *ip= (int*) ap;
	    return sprintf(buf, fmt, ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], 
						   ip[6], ip[7], ip[8], ip[9]);
	}

#   endif DOPRNT
#endif VPRINTF


int StuffChar(char *src, char *dst, int dstlen, char* specchars, char stuffchar)
{
    register char *p, *q, *end;

    end= dst+dstlen-1;

    for (p= src, q= dst; *p && q < end; ) {
	if (index(specchars, *p)) {
	    *q++= stuffchar;
	    if (q < end)
		*q++= *p++;
	} else 
	    *q++= *p++;
    }
    *q= '\0';

    if (*p != 0)
	return -1;
    return q-dst;
}

static int readchar(istream &i, byte end)
{
    byte c;
    int d, k, j;

    for (;;) {
	i.get(c);
	if (i.rdstate() != _good || c == end)
	    return -1;
	switch (c) {
	case '\\':
	    i.get(c);
	    switch (c) {
	    case 'n':
		return '\n';
	    case 'r':
		return '\r';
	    case 'b':
		return '\b';
	    case 'f':
		return '\f';
	    case 't':
		return '\t';
	    case 'v':
		return '\v';
	    case 'x':
		for (k= j= 0; j < 2 && k < 256; j++) {
		    i.get(c);
		    if (! Isxdigit(c)) {
			i.putback(c);
			break;
		    }
		    if (Isdigit(c))
			d= c - '0';
		    else if (Islower(c))
			d= c - 'a' + 10;
		    else
			d= c - 'A' + 10;
		    k= k*16 + d;
		}
		return k;

	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7':
		for (k= j= 0; j < 3 && k < 256; j++) {
		    k= k*8 + c - '0';
		    i.get(c);
		    if (c < '0' || c > '7')
			break;
		}
		i.putback(c);
		return k;

	    default:
		return c;
	    }
	    break;

	default:
	    if (Isprint(c))
		return c;
	    break;
	}
    }
}

istream &ReadString(istream &i, byte **s, int *lp)
{
    int l, c;    
    byte cc, *p, *pp;

    i >> l >> cc;
    pp= p= new byte[l];
    if (cc == '"')
	while ((c= readchar(i, '\"')) != -1)
	   *p++= (byte) c;
    if (s)
	*s= pp;
    if (lp)
	*lp= l;
    return i;
}

char ReadChar(istream &i)
{
    int c;    
    char cc;

    i >> cc;
    if (cc == '\'')
	while ((c= readchar(i, '\'')) != -1)
	   cc= (unsigned char) c;
    return cc;
}

void printchar(ostream &o, byte c)
{
    switch(c) {
    case '{':
    case '}':
	o.put('\\'); o.put('x'); PutHex(o, c); col+= 4;
	break;
    case '\n':
	o.put('\\'); o.put('n'); col+= 2;
	break;
    case '\r':
	o.put('\\'); o.put('r'); col+= 2;
	break;
    case '\t':
	o.put('\\'); o.put('t'); col+= 2;
	break;
    case '\v':
	o.put('\\'); o.put('v'); col+= 2;
	break;
    case '\f':
	o.put('\\'); o.put('f'); col+= 2;
	break;
    case '\b':
	o.put('\\'); o.put('b'); col+= 2;
	break;
    case '\"':
	o.put('\\'); o.put('\"'); col+= 2;
	break;
    case '\'':
	o.put('\\'); o.put('\''); col+= 2;
	break;
    case '\\':
	o.put('\\'); o.put('\\'); col+= 2;
	break;
    default:
	if (Isascii(c) && Isprint(c)) {
	    o.put(c); col++;
	} else {
	    o.put('\\'); o.put('x'); PutHex(o, c); col+= 4;
	}
    }
}

ostream &PrintString(ostream &o, byte *s, int l)
{
    if (l < 0)
	l= strlen((char*)s)+1;
    o << l SP;
    o.put('\"');
    for (col= 0; l > 0; l--) {
	printchar(o, *s++);
	if (col >= 72) {
	    o.put('\n');
	    col= 0;
	}
    }
    o.put('\"');
    return o;
}

ostream &PrintChar(ostream &o, char c)
{
    o.put('\"');
    printchar(o, c);
    o.put('\"');
    return o;
}

byte GetHex(istream &s)
{
    char c;
    unsigned short val= 0;

    for (int i= 0; i<2;) {
	s.get(c);
	if (Isxdigit(c)) {
	    if (Isdigit(c))
		val= val*16 + c-'0';
	    else if (Isupper(c))
		val= val*16 + c-'A'+10;
	    else 
		val= val*16 + c-'a'+10;
	    i++;
	}
    }
    return val;
}

void PutHex(ostream &ofp, byte b, int *col)
{    
    ofp.put(hexChars[b/16]);
    ofp.put(hexChars[b%16]);
    if (col) {
	(*col)++;
	if (*col >= 40) {
	    ofp.put('\n');
	    *col= 0;
	}
    }
}

char *strsave(char *s, int l)
{
    if (s == 0 || l == 0)
	return 0;
    if (l < 0)
	l= strlen(s)+1;
    char *str= new char[l];
    return strncpy(str, s, l);
}

char *strreplace(char **s, char *r, int l)
{
    if (s == 0)
	Fatal("strreplace", "s == 0, replace: %s l: %d", r, l);
    if (r == 0 || l == 0)
	return *s;
    if (l < 0)
	l= strlen(r)+1;
    *s= (char*) Realloc(*s, l);
    return strncpy(*s, r, l);
}

char *strfreplace(char **s, char *fmt, va_list ap)
{
    char buf[1000];

    if (s == 0)
	Fatal("strfreplace", "s == 0, fmt: %s", fmt);
    if (fmt == 0)
	return *s;

    vsprintf(buf, fmt, ap);

    *s= (char*) Realloc(*s, strlen(buf)+1);
    return strcpy(*s, buf);
}

char *strprintf(char *va_(fmt), ...)
{
    va_list ap;
    char buf[2000];

    va_start(ap,va_(fmt));
    vsprintf(buf, va_(fmt), ap);
    va_end(ap);
    return strsave(buf);
}

char* strvprintf(char* fmt, va_list ap)
{       
    char buf[2000];

    vsprintf(buf, fmt, ap);
    return strsave(buf);
}

bool strismember(const char *va_(cp), ...)
{
    char *s;   
    va_list ap;
    bool found= FALSE;
    va_start(ap,va_(cp));
    for (int i= 0; s= va_arg(ap, char*); i++)  
	if (strcmp(va_(cp), s) == 0) {
	    found= TRUE;
	    break;
	}
    va_end(ap);
    return found;
}

char* strn0cpy(char *dst, const char *src, int l)
{
    strncpy(dst, src, l-1);
    dst[l-1]= '\0';
    return dst;
}

char *strquotechar(byte ch, char *q)
{
    switch(ch) {
    case '\n':
	*q++='\\'; *q++='n'; 
	break;
    case '\r':
	*q++='\\'; *q++='r'; 
	break;
    case '\0':
	*q++='\\'; *q++='0'; 
	break;
    case '\t':
	*q++='\\'; *q++='t'; 
	break;
    case '\v':
	*q++='\\'; *q++='v'; 
	break;
    case '\f':
	*q++='\\'; *q++='f'; 
	break;
    case '\b':
	*q++='\\'; *q++='b'; 
	break;
    default:
	if (Isascii(ch) && Isprint(ch)) {
	    *q++= ch;
	} else {
	    *q++= '\\'; *q++= 'x'; *q++= hexChars[ch/16]; *q++= hexChars[ch%16]; 
	}
    }
    return q;
}

char *BaseName(char *name) 
{
    char *filename = rindex(name, '/');

    if (!filename)
	filename= name;
    else
	filename++;
    return filename;
}

int StrCmp(register byte *s1, register byte *s2, int n, register byte *map)
{
    if (map) {
	if (n < 0) {
	    while (map[(byte)*s1] == map[(byte)*s2++])
		if (map[(byte)*s1++] == '\0')
		    return 0;
	    return map[(byte)*s1] - map[(byte)*--s2];
	} else {
	    while (--n >= 0 && map[(byte)*s1] == map[(byte)*s2++])
		if (map[(byte)*s1++] == '\0')
		    return 0;
	    return (n < 0) ? 0 : map[(byte)*s1] - map[(byte)*--s2];
	}
    } else {
	if (n < 0) {
	    while (*s1 == *s2++)
		if (*s1++ == '\0')
		    return 0;
	    return (int)*s1 - (int)*--s2;
	} else {
	    while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
		    return 0;
	    return (n < 0) ? 0 : (int)*s1 - (int)*--s2;
	}
    }
}

const   cb_size = 1024;
const   fld_size = 512;

// a circular formating buffer
static char     formbuf[cb_size];       // some slob for form overflow
static char*    bfree= formbuf;
static char*    endbuf= &formbuf[cb_size-1];

char* form(const char* va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    const char *format= va_(fmt);
    register char* buf = bfree;

    if (endbuf < buf+fld_size)
	buf= formbuf;

    register int ll= (int) vsprintf(buf, format, ap);
    if (0 < ll && ll < cb_size)                 // length
	    ;
    else if (buf<(char*)ll && (char*)ll<buf+cb_size)// pointer to trailing 0
	    ll= (char*)ll - buf;
    else
	    ll= strlen(buf);
    if (fld_size < ll)  // oops
	Fatal("form", "buffer overflow");
    bfree = buf+ll+1;
    va_end(ap);
    return buf;
}

