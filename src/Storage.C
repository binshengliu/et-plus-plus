#include "Storage.h"
#include "System.h"
#include "Error.h"
#include "FixedStorage.h"

#include "MALLOC/storage.h"

const int cMaxSize= 300;
static bool init= FALSE;
static void *smallestAddress= 0;
static int maxA= 0;
static int ASizes[cMaxSize];
static int FSizes[cMaxSize];
static int totalA, totalF;

FreeHookFun storageFreeHook= 0;
void *storageFreeHookData;
     
void StorageSetFreeHook(FreeHookFun fh, void *data)
{
    storageFreeHook= fh;
    storageFreeHookData= data;
}

FreeHookFun StorageGetFreeHook(void **data)
{
    *data= storageFreeHookData;
    return storageFreeHook;
}
	    
static void StorageInit()
{
    init= TRUE;
    storage_init();
    // smallestAddress= Malloc(1);
}

inline void InitStorage()
{
    if (!init)
	StorageInit();
}

static void EnterStat(int size)
{
    if (size >= cMaxSize)
	ASizes[cMaxSize-1]++;
    else
	ASizes[size]++;
    totalA+= size;
}

static void RemoveStat(int size)
{
    if (size >= cMaxSize)
	FSizes[cMaxSize-1]++;
    else
	FSizes[size]++;
    totalF+= size;
}

void* operator new(size_t size)
{
    InitStorage();
    if (size < 0)
	Fatal("operator new", "size < 0");
    maxA= max(maxA, size);
    EnterStat(size);
    return storage_malloc(size);
}

void operator delete(void* ptr)
{
    if (ptr) {
	if (ptr < smallestAddress)
	    Fatal("Free", "unreasonable address (0x%x)", ptr);
	int size= storage_size(ptr);
	if (size < 0 || size > maxA)
	    Fatal("Free", "unreasonable size (%d)", size);
	RemoveStat(size);
	if (storageFreeHook) 
	    storageFreeHook(storageFreeHookData, ptr, size);
	storage_free(ptr, size);
    }
}

void *Realloc(void *ptr, size_t size)
{
    InitStorage();
    maxA= max(maxA, size);
    if (ptr == 0) {
	EnterStat(size);
	ptr= storage_malloc(size);
    } else if (ptr < smallestAddress) {
	Fatal("Realloc", "unreasonable address (0x%x)", ptr);
    } else {
	int oldsize= storage_size(ptr);
	if (size > oldsize) {
	    RemoveStat(oldsize);
	    EnterStat(size);
	    ptr= storage_realloc(ptr, oldsize, size);
	}
    }
    return ptr;
}

void PrintStorageStatistics()
{
    cerr NL;
    cerr << "  Size Alloc  Free  Diff\n";
    cerr << "------------------------\n";
    for (int i= 0; i < cMaxSize; i++)
	if (ASizes[i] != FSizes[i])
	    cerr.form("%6d%6d%6d%6d\n", i, ASizes[i], FSizes[i],
							ASizes[i]-FSizes[i]);
    cerr.form("\n\ntotalA: %d  totalF: %d totalD: %d\n", totalA, totalF,
							totalA-totalF);
}

