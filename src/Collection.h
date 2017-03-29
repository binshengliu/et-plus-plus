#ifndef Collection_First
#ifdef __GNUG__
#pragma once
#endif
#define Collection_First

#include "Object.h"
#include "Iterator.h"

extern class Collection *pCurrentCollection;
extern char *cNullPointerWarning;

//---- abstract class Collection ------------------------------------------

typedef class Collection *CollPtr;

const cCollectionInitCap = 16;

class Collection: public Object {
friend class CollFilterIter;
    int iterCount;          // number of active iterators
    int nDeleted;           // number of deleted Objects

protected:
    int size;
    
    Collection();
    virtual int GrowBy(int desiredSize);
    virtual void RemoveDeleted();
    void AnnounceRemove()
	{ nDeleted++; }
    bool AnyDeleted()
	{ return (bool) (nDeleted > 0); }

public: 
    MetaDef(Collection);
    ~Collection();
    bool assertclass(Class *);
    void InspectorId(char *, int);
	
    //---- manipulation
    virtual ObjPtr Add(ObjPtr);
    void AddVector(ObjPtr op1, ...);
    void AddVector(va_list ap); 
    virtual ObjPtr Remove(ObjPtr);
    virtual ObjPtr RemovePtr(ObjPtr);
    virtual void AddAll (CollPtr);
    virtual void RemoveAll (CollPtr);
    void FreeAll ();
    virtual void Empty (int);
    //      empty the contents of a collection and reinitialize it with a
    //      given capacity
    virtual ObjPtr Clone();  

    //---- accessing
    virtual Iterator *MakeIterator(); // return an iterator of a collection
    // if efficency is important the follwing functions should be overridden!!   
    virtual ObjPtr At (int);
    virtual bool Contains (ObjPtr);   // based on IsEqual
    virtual bool ContainsPtr (ObjPtr);// based on the identity of the objects
    virtual int OccurrencesOf (ObjPtr); // based on IsEqual
    virtual int OccurrencesOfPtr (ObjPtr); // based on the identity of the objects
    int Size()
	{ return size-nDeleted; }
    bool IsEmpty()
	{ return (bool) (Size() <= 0); }
    virtual ObjPtr Find(ObjPtr);        // return 0 when not found, based on IsEqual
    virtual ObjPtr FindPtr(ObjPtr);     // return 0 when not found, based on identity

    //---- comparing
    bool IsEqual(ObjPtr);
    unsigned long Hash();

    //---- enumerating
    //      the calling protocol for BoolFun and ObjFun is:
    //      Fun(this,CurrentOp,Arg)    
    virtual CollPtr Collect(ObjPtrFun,void * Arg = 0);
	// collect the elements returned by ObjPtrFun
    virtual CollPtr Select(BoolFun,void * Arg = 0);
	// select all elements for the new collection where BoolFun returns true
    virtual ObjPtr Detect(BoolFun,void * Arg = 0);
	// find first entry where BoolFun returns true

    //---- robust iterators
    void EnterIter();
    void ExitIter();
    bool InIterator()
	{ return (bool) (iterCount > 0); }
    void CheckActiveIter(char *where);

    //---- converting
    virtual class OrdCollection *AsOrderedCollection();
    virtual class ObjArray *AsObjArray();
    virtual class Bag *AsBag();
    virtual class Set *AsSet();
    virtual class ObjList *AsObjList();
    virtual class SortedObjList *AsSortedObjList(bool ascending = TRUE);

    //---- printing
    virtual ostream& DisplayOn(ostream &s);
    virtual ostream& PrintOn(ostream &s);
    virtual istream& ReadFrom(istream &s);

    void setCurrentCollection()
	{ pCurrentCollection= this; }
    bool NullPointerWarning(char *where);
    bool CheckNotNull(char *where, Object *op) 
	{ return (bool) (op == 0 ? NullPointerWarning(where) : FALSE); }
};

//---- class Iter ------------------------------------------------------------

class Iter {
    Iterator *seq;
public:
    Iter(Collection *col)
	{ seq= col->MakeIterator(); }
    Iter(Iterator *it)
	{ seq= it; }
    ~Iter()
	{ if (seq) delete seq; }
    class Object *operator()()
	{ return (*seq)(); }
    void Reset(Collection *col)
	{ seq->Reset(col); }
};

//---- class CollFilterIter --------------------------------------------------
//     iterate through a collection and apply a given filter function

class CollFilterIter : public Iterator {
    Iterator *next;
    ObjPtr op;
    ObjPtrFun filterFun;
    void *filterArg;
public:
    CollFilterIter(Collection *s,ObjPtrFun Filter,void *Arg = 0)
	{ next = s->MakeIterator(); filterFun = Filter; filterArg = Arg; }

    ~CollFilterIter();
    void Reset(Collection *s);
    ObjPtr operator()();
};

//---- class DeletedObject ---------------------------------------------------
//     place holder for deleted objects in a collection

class DeletedObject: public Object {
public:
    MetaDef(DeletedObject);
    DeletedObject();
};

#define AssertClass(name) assertclass(Meta(name))

#define ForEach(type,proc)                          \
setCurrentCollection();                             \
Iter _NAME3_(type,proc,_next)(pCurrentCollection);  \
register type *_NAME3_(type,proc,_op);              \
while (_NAME3_(type,proc,_op)= (type *) _NAME3_(type,proc,_next)())    \
    _NAME3_(type,proc,_op)->proc

#endif Collection_First
