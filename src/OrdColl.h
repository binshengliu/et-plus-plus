#ifndef OrdColl_First
#ifdef __GNUG__
#pragma once
#endif
#define OrdColl_First

#include "SeqColl.h"
#include "ObjArray.h"

//---- ordered collection ------------------------------------------

class OrdCollection: public SeqCollection {
friend class OrdCollectionIter;
friend class RevOrdCollectionIter;
    ObjArray *cont;    
    void RemoveDeleted();
    bool LowWaterMark()
	{ return  (bool) (Size() < (cont->Size() / 4) && size > cCollectionInitCap); }
    ObjPtr DoRemoveAt(int index);    
    void InsertAtPos(ObjPtr at, ObjPtr op, bool byPtr, bool after, char *name); 

public:
    MetaDef(OrdCollection);
    OrdCollection(int s= 1);
    ~OrdCollection();

    void FreeAll();
    void Empty(int initSize = cCollectionInitCap);
    void InitNew();
    
    ObjPtr Add(ObjPtr);
    void AddAt(int,ObjPtr); // add an element at the position i 

    void InsertAfter(ObjPtr after, ObjPtr op);
    void InsertBefore(ObjPtr before, ObjPtr op);
    void InsertAfterPtr(ObjPtr after, ObjPtr op);
    void InsertBeforePtr(ObjPtr before, ObjPtr op);

    ObjPtr Remove(ObjPtr);
    ObjPtr RemovePtr(ObjPtr);
    ObjPtr RemoveAt(int); // remove and element at the given position
    
    ObjPtr At(int i);
    ObjPtr After(ObjPtr op); // returns 0 if op is the last item
    ObjPtr Before(ObjPtr op); // return 0 if op is the first item
    
    ObjPtr First();
    ObjPtr Last();
    void Sort(); 
	// based on compare
    int BinarySearch(ObjPtr); 
	// the collection has to be sorted !! -1 == not found
    int Capacity();

    //---- enumerating
    Iterator *MakeIterator(); 
    Iterator *MakeReversedIterator();
};

class OrdCollectionIter : public Iterator {
protected:
    int ce;
    OrdCollection *cs;
public:
    OrdCollectionIter(Collection *s);
    ~OrdCollectionIter();
    ObjPtr operator()();
    Collection *Coll();
    void Reset(Collection *s= 0);

    //---- memory allocation
    void *operator new (size_t);
    void operator delete(void *vp);
};

class RevOrdCollectionIter : public OrdCollectionIter {
public:
    RevOrdCollectionIter(Collection *s);
    ObjPtr operator()();    
    void Reset(Collection *s= 0);
};

#endif OrdColl_First
