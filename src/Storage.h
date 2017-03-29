#ifndef Storage_First
#ifdef __GNUG__
#pragma once
#endif
#define Storage_First

#include "Types.h"

#if defined(sun) || defined(sony)
#   define has_alloca
#endif

#if defined(sparc)
#   ifndef __GNUG__
#       define alloca(x) __builtin_alloca(x)
#   endif
#endif

#if defined(has_alloca)
#   ifndef __GNUG__
	extern "C" void *alloca(size_t);
#   endif

#   define Alloca(x) alloca(x)
#   define Freea(x) 
#else
#   define Alloca(x) ((void*) (new char[x]))
#   define Freea(x) delete (x)
#endif

extern bool gMemStatistics;

extern "C" void *Realloc(void*, size_t);

extern void PrintStorageStatistics();
typedef void (*FreeHookFun)(void *, void *addr, size_t);
    
void StorageSetFreeHook(FreeHookFun, void *data);
FreeHookFun StorageGetFreeHook(void **data);

#endif Storage_First
