#ifndef ObjArray_First
#ifdef __GNUG__
#pragma once
#endif
#define ObjArray_First

#include "Collection.h"

extern char *cOutOfBoundsError;
extern char *cMethodName;

//---- class ObjArray --------------------------------------------------

class ObjArray: public Collection {
friend class ObjArrayIter;
friend class OrdCollection;
    ObjPtr *cont;
    int  lb; // lower bound of the array
    bool BoundsOk(char *where, int at)
	{ if ( at < lb || at-lb >= Size()) {
	    Error(where, cOutOfBoundsError, at, Size(), (int)this);
	    return FALSE;
	  }
	  else
	    return TRUE;
	}        
public:  
    MetaDef(ObjArray);

    //---- creation, destruction  
    ObjArray(int s= cCollectionInitCap, int lowerBound= 0); 
    ~ObjArray();
    void InitNew();              
    void Expand (int);              // expand or shrink an array
    ObjPtr Add(ObjPtr);
    void FreeAll();
    
    //---- accessing
    Iterator *MakeIterator();           // return a Iterator iterator of a collection
    int IndexOfPtr (ObjPtr);     // returns -1 if not found     
    int IndexOf (ObjPtr);        //        "           "
    ObjPtr At(int i);
    ObjPtr AtPut(int i, ObjPtr op);
    void AtPutAndExpand(int i, ObjPtr op);// expands the array if necessary
    ObjPtr& operator[](int i)       // shorthand notation
	{ 
	    if ( i < lb || i-lb >= size)
		Error(cMethodName, cOutOfBoundsError, i, size, this);
	    return cont[i-lb]; 
	}
    ObjPtr UncheckedAt(int i)
	{ return cont[i]; }
    ObjPtr RemoveAt(int i);   
    ObjPtr Remove(ObjPtr a);
    ObjPtr RemovePtr(ObjPtr a);
    int LowerBound()
	{ return lb; }
    void Sort(int upto = cMaxInt);
    int BinarySearch(ObjPtr, int upto = cMaxInt);
	// the ObjArray has to be sorted, -1 == not found !!

    //---- comparing   
    unsigned long Hash ();
    bool IsEqual (ObjPtr);
    int  Compare (ObjPtr);

    //---- activation/passivation
    ostream& PrintOn(ostream&s);
    istream& ReadFrom(istream&s);
};

class ObjArrayIter: public Iterator {
    int ce;
    ObjArray *cs;
public:
    ObjArrayIter(Collection *s);
    ~ObjArrayIter();
    
    void Reset(Collection *c= 0);
    ObjPtr operator()();
    virtual bool Filter(ObjPtr);

    //---- memory allocation
    void *operator new (size_t);
    void operator delete(void *vp);
};

#endif ObjArray_First
