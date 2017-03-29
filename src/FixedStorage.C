#include "FixedStorage.h"
#include "Storage.h"
#include "String.h"
#include "Error.h"

extern FreeHookFun storageFreeHook;
extern void *storageFreeHookData;

struct FreeNode {
    class FreeNode *next;
};

struct Chunk {
    struct Chunk *next;
    char *storage;    
};

static MemPool *pools[cObjMaxSize];

void *MemPools::Alloc(size_t sz)
{
    if (sz >= cObjMaxSize) {
	Warning("MemPools::Alloc", "supports only objects with a size upto %d\n", cObjMaxSize);
#       ifdef __GNUG__
	    return (void*) new char[sz];
#       else
	return ::operator new(sz);
#           endif __GNUG__
    }
    MemPool *mp= pools[sz];
    if (mp == 0) 
	mp= pools[sz]= new MemPool(sz);
    return mp->Alloc();
}

void MemPools::Free(void *vp, size_t sz)
{
    MemPool *mp;
    if (sz >= cObjMaxSize) {
#       ifdef __GNUG__
	    delete vp;
#       else
	    ::operator delete(vp);
#       endif __GNUG__
	return;
    }
    else
	mp= pools[sz];
	
    if (mp)
	mp->Free(vp);
    else
	Fatal("MemPools::Dealloc", "No pool of size %d\n", sz);
}
    
void MemPools::FreePools()
{
    for (int i= 0; i < cObjMaxSize; i++) {
	MemPool **fs= &pools[i];
	SafeDelete(fs);
    }
}
void MemPools::PrintStatistics()
{
    cerr << "Pools statistics\n----------------\n" <<
	    form("%-10s%8s%8s%8s%8s\n", "size", "alloc", "recycl", "freed", "chunks");

    MemPool *fsp;
    for (int i= 0; i < cObjMaxSize; i++) 
	if (fsp= pools[i]) 
	    cerr << form("%10d%8d%8d%8d%8d\n", 
			    fsp->objSize, fsp->allocated, 
			    fsp->recycled, fsp->freed, fsp->nchunks);
}

//---- class MemPool ---------------------------------------------------------

MemPool::MemPool(size_t osize)
{
    objSize= osize;
    chunkSize= cMemPoolSize * objSize;
    chunks= 0;
    lastIndex= cMemPoolSize; // force allocation of a new chunk
    freeList= 0;
    nchunks= allocated= recycled= freed= 0;
}

MemPool::~MemPool()
{
    Chunk *p, *lp;

    for (p= chunks; p; p= lp) {
	delete p->storage;
	lp= p->next;
	delete p;
    }
}

void *MemPool::Alloc()
{
    allocated++;
    if (freeList) {
	recycled++;
	FreeNode *tmp= freeList;
	freeList= freeList->next;
	return (void*)tmp;
    }
    if (lastIndex >= cMemPoolSize) { // need new chunk
	nchunks++;
	Chunk *cp;
	cp= new Chunk;
	cp->next= chunks;
	chunks= cp;
	chunks->storage= new char[chunkSize];
	lastIndex= 0;
    }
    return (void*)&chunks->storage[lastIndex++*objSize];
}

void MemPool::Free(void *op)
{
    if (storageFreeHook)
	storageFreeHook(storageFreeHookData, op, objSize);

    for (Chunk *p= chunks; p; p= p->next) {
	if (op >= (void*)p->storage && op <  (void*)(p->storage + chunkSize)) {
	    bzero((char*) op, objSize);
	    FreeNode *fn= (FreeNode*)op;
	    fn->next= freeList;
	    freeList= fn;
	    freed++;
	    return;
	}
    }
    Fatal("MemPool::Free", "object to delete not found in pool %d", objSize);
}

