#ifndef Set_First
#ifdef __GNUG__
#pragma once
#endif
#define Set_First

#include "ObjArray.h"

//---- set ------------------------------------------

typedef class Set *SetPtr;

class Set: public Collection {
friend class SetIter;
    int initSize;                           // initial size of the set

protected:
    ObjArray *cont;

    bool HighWaterMark ()
	{ return (bool) (cont == NULL || size >= ((3 * cont->Size() /4))); }
    bool LowWaterMark ()
	{ return (bool) (cont && size < (1 * cont->Size() /4) && size > initSize); }
    void FixCollisions(int at);
    void RemoveDeleted();

    virtual int FindElement(ObjPtr); 
    virtual void Expand(int); 
    virtual u_long HashObject(Object *);
    virtual bool   ObjectsEqual(Object *op1, Object *op2);
public:
    MetaDef(Set);   
    Set(int s= cCollectionInitCap);
    ~Set();
    void Init(int s);
    void InitNew();
    void Empty(int initSize = 0);
    ObjPtr Add(ObjPtr);
    void Filter(ObjPtr);  // if an equal object is already in the set
			  // it will be deleted with a call to delete ObjPtr
    ObjPtr Remove(ObjPtr);
    ObjPtr RemovePtr(ObjPtr);
    ObjPtr Find(ObjPtr);
    ObjPtr FindPtr(ObjPtr);    
    bool Contains (ObjPtr); 
    bool ContainsPtr (ObjPtr); 

    //---- enumerating
    virtual Iterator *MakeIterator(); 

    //---- set operations
    SetPtr Union (SetPtr);
    SetPtr Intersection (SetPtr);
    SetPtr Difference (SetPtr);
    SetPtr operator&(SetPtr s)
	{ return Intersection(s); }      // intersection 
    SetPtr operator|(SetPtr s) 
	{ return Union(s); }             // union 
    SetPtr operator-(SetPtr s)
	{ return Difference(s); }        // difference 
};

class SetIter : public Iterator {
    int ce;
    Set *cs;
public:
    SetIter(Collection *s);
    ~SetIter();
    ObjPtr operator()();
    Collection *Coll();
    void Reset(Collection *s= 0);
};

int SetNextPrime(int); // find next prime number

#endif Set_First
