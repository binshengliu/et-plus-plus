extern int gDebug;
extern char *malloc();

#define MEM_MAGIC       ((char)0xAB)

char storage_init()
{
}

int storage_size(ptr)
char *ptr;
{
    return ((int*)ptr)[-1];
}

char *storage_malloc(size)
unsigned size;
{
    register int *sp;

    if (gDebug)
	sp= (int*) malloc(size + sizeof(int) + sizeof(char));
    else
	sp= (int*) malloc(size + sizeof(int));

    if (sp == 0)
	Fatal("storage_malloc", "no more space");
    bzero(&sp[1], size);
    sp[0]= size;
    if (gDebug)
	*((char*)sp+size+sizeof(int))= MEM_MAGIC;
    return (char*) &sp[1];
}

char *storage_malloc_atomic(size)
unsigned size;
{
    register int *sp;

    if (gDebug)
	sp= (int*) malloc(size + sizeof(int) + sizeof(char));
    else
	sp= (int*) malloc(size + sizeof(int));
    if (sp == 0) {
	Fatal("storage_malloc_atomic", "no more space");
    }
    bzero(&sp[1], size);
    sp[0]= size;
    if (gDebug)
	*((char*)sp+size+sizeof(int))= MEM_MAGIC;
    return (char*) &sp[1];
}

char storage_free(ptr, size)
char *ptr;
int size;
{
    ptr-= sizeof(int);
    if (gDebug) {
	if (*(ptr+size+sizeof(int)) != MEM_MAGIC) {
	    Fatal("storage_free", "storage area overwritten");
	    abort();
	}
	bzero((char*) ptr, size + sizeof(int) + sizeof(char));
    } else
	bzero((char*) ptr, size + sizeof(int));
    free((char*) ptr);
}

char *storage_realloc(ptr, oldsize, size)
char *ptr;
unsigned oldsize, size;
{
    int *sp;
    char *cp;

    if (gDebug) {
	sp= (int*) realloc((char*) (ptr - sizeof(int)), size+sizeof(int)+sizeof(char));
	cp= (char*) &sp[1];
	if (cp[oldsize] != MEM_MAGIC) {
	    Fatal("storage_free", "storage area overwritten");
	    abort();
	}
	bzero(&cp[oldsize], size-oldsize);
	sp[0]= size; 
	*((char*)sp+size+sizeof(int))= MEM_MAGIC;
    } else {
	sp= (int*) realloc((char*) (ptr - sizeof(int)), size+sizeof(int));
	cp= (char*) &sp[1];
	bzero(&cp[oldsize], size-oldsize);
	sp[0]= size;
    }
    return cp;
}
