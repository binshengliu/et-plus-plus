#ifndef ObjList_First
#ifdef __GNUG__
#pragma once
#endif
#define ObjList_First

#include "SeqColl.h"

typedef void (*ObjectFunction)(ObjPtr, ...);     

class ObjLink: public Object {
friend class ObjList;
friend class ObjListIter;
friend class RevObjListIter;
    ObjLink *next, *previous;
    ObjPtr op;
    
public:
    MetaDef(ObjLink);
    ObjLink(ObjPtr a, ObjLink* n, ObjLink* p);
    ObjLink(ObjPtr a, ObjLink*);
    ~ObjLink();
    ObjPtr Op()
	{ return op; }
    ObjLink *Next()
	{ return next; }
    void FreeAll();
    //---- memory allocation
    void *operator new (size_t);
    void operator delete(void *vp);
};

class ObjList: public SeqCollection {
friend class ObjListIter;
friend class RevObjListIter;

protected:
    ObjLink *first, *last;
    void RemoveDeleted();
    void RemoveLink(ObjLink *l);
    ObjLink *FindObjLink(ObjPtr op, bool byPtr);
    void PrivInsertBefore(ObjPtr before, ObjPtr op, bool byPtr, char *name);
    void PrivInsertAfter(ObjPtr after, ObjPtr op, bool byPtr, char *name);
    void InsertObjLink(ObjPtr op, ObjLink *l);
    
public:
    MetaDef(ObjList);
    
    ObjList();
    ~ObjList();
    void FreeAll();
    
    void Insert(ObjPtr a);
    void InsertBefore(ObjPtr before, ObjPtr a);
    void InsertAfter(ObjPtr after, ObjPtr a);
    void InsertBeforePtr(ObjPtr before, ObjPtr a);
    void InsertAfterPtr(ObjPtr after, ObjPtr a);
    
    virtual ObjPtr Replace(ObjPtr a, ObjPtr b);
    ObjPtr First();
    ObjPtr Last();
    ObjPtr GetAt(int);
    ObjPtr Remove(ObjPtr);
    ObjPtr RemovePtr(ObjPtr);
    
    //---- collection compatibility
    Iterator *MakeIterator();
    Iterator *MakeReversedIterator();
    ObjPtr Add(ObjPtr);
    void Empty(int);
    ObjPtr At (int i);
    ObjPtr Before(ObjPtr);
    ObjPtr After(ObjPtr);
};

//---- class ObjListIter -------------------------------------------------------

class ObjListIter: public Iterator {
public:
    ObjLink *ce;
    ObjList *cs;

    ObjListIter(Collection *s);
    ~ObjListIter();
    void Reset(Collection *s= 0);
    Collection *Coll();
    Object* operator()();

    //---- memory allocation
    void *operator new (size_t);
    void operator delete(void *vp);
};

//---- class RevObjListIter ----------------------------------------------------

class RevObjListIter: public ObjListIter {
public:
    RevObjListIter(ObjList *s);
    Object* operator()();
    void Reset(Collection *s= 0);
};

#endif ObjList
