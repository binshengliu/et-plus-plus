#ifndef PathLookup_First
#ifdef __GNUG__
#pragma once
#endif
#define PathLookup_First

#include "Object.h"

//---- class PathLookup -----------------------------------------------------

class PathLookup: public Object {
friend class PathIter;
protected:
    char *path;
    class OrdCollection *paths;
    virtual void Scan();
public:
    MetaDef(PathLookup);
    PathLookup(char *path);                   // eg. "/bin:/tmp:/usr/bin"
    ~PathLookup();
    bool Lookup(const char *file, char *buf); // returns in buf the path to file
    void Add(char *);
};

class PathIter: public Root {
    class Iterator *ip;
public:
    PathIter(PathLookup *p);
    ~PathIter();
    char *operator()();
};
    
#endif PathLookup_First

