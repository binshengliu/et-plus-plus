#ifndef MemPools_First
#ifdef __GNUG__
#pragma once
#endif
#define MemPools_First

#include "Types.h"

const int cMemPoolSize=   200;
const int cObjMaxSize =   256;       

//---- Pools of fixed size storage allocators -----------------------

class MemPools {
public:
    static void *Alloc(size_t sz);
    static void Free(void *vp, size_t sz);
    static void PrintStatistics();
    static void FreePools();
};

//---- Pool for a specific size ---------------------------------------

class MemPool {
friend class MemPools;
    struct Chunk *chunks;
    struct FreeNode *freeList;
    int lastIndex;
    int chunkSize;
    size_t objSize;
    int allocated;
    int recycled;
    int freed;
    int nchunks;
public:
    MemPool(size_t objSize);
    ~MemPool();
    void *Alloc();
    void Free(void *);
};

#endif MemPools_First
