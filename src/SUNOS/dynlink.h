#ifndef dynlink_First
#define dynlink_First

#ifdef __GNUG__
#define __cfront_version__ 3
#else
#define __cfront_version__ 2
#endif

extern "C" {
    void dynlinkinit(char *name, int ccversion, char *etdir);
    int  DynLoad(char *name);
    void *DynLookup(char *name);
    void DynCleanup();                                            
    VoidPtrFunc DynCall(char*);
}

inline void DynLinkInit(char *name, char *etdir)
{
    dynlinkinit(name, __cfront_version__, etdir);
}

#endif dynlink_First


