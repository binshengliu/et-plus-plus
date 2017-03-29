#ifndef MethodAccessor_First
#define MethodAccessor_First

#include "Object.h"

//---- PeMethodAccessor -------------------------------------------------------

class PeMethodAccessor: public Object {
public:
    PeMethodAccessor();
    virtual class Collection *FindMethods(Class *cl, class PathLookup *universe);
};

//---- PeMapMethodAccessor ----------------------------------------------------

class PeMapMethodAccessor: public PeMethodAccessor {

    PathLookup *mapSrcPath;    
    char *GetMapFileName(Class *, PathLookup *);
    void CreateMapSearchPath(PathLookup *);
public:
    PeMapMethodAccessor();
    Collection *FindMethods(Class *cl, PathLookup *universe);
};

//---- PeScanMethodAccessor ----------------------------------------------------

class PeScanMethodAccessor: public PeMethodAccessor {

public:
    PeScanMethodAccessor();
    Collection *FindMethods(Class *cl, PathLookup *universe);
};


#endif MethodAccessor_First
