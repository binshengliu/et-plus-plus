#ifndef Bag_First
#ifdef __GNUG__
#pragma once
#endif
#define Bag_First

#include "Set.h"

//---- Bag ---------------------------------------------------------------------

typedef class Bag *BagPtr;

class Bag: public Collection {
friend class BagIter;
friend class BagOccIter;
protected:
    class Set *cont;
public:
    MetaDef(Bag);  
    Bag(int s= cCollectionInitCap);
    ~Bag();
    
    void Empty(int initSize = 0); 
    void FreeAll();
    ObjPtr Add(ObjPtr);
    void AddWithOccurrences(ObjPtr,int);
    ObjPtr Remove(ObjPtr);
    ObjPtr At(int);
    ObjPtr Find(ObjPtr);    // returns NULL if not found
    bool Contains (ObjPtr); 
    int OccurrencesOf (ObjPtr);  

    //---- enumerating
    Iterator *MakeIterator();
};

class BagIter : public Iterator {
    int occurrences;
    SetIter next;
    ObjPtr op;
    Bag *cs;
public:
    BagIter(Collection *s) :next(((Bag *)s)->cont)
	{ occurrences = 0; cs = (Bag *)s; }
	
    void Reset(Collection *s);
    ObjPtr operator()();
};

class BagOccIter : public Iterator { // return each object in the bag together
				     // with a count of the number of occurrences
    SetIter next;
    Bag *cs;
public:
    BagOccIter(Collection *s) :next(((Bag *)s)->cont)
	{ cs = (Bag *)s; }
	
    void Reset(Collection *s);
    ObjPtr Occurrence(int *occurences);
};

class BagItem: public Object {
    friend class Bag;
    friend class BagIter;
    friend class BagOccIter;

    ObjPtr op;                  // the object itself
    int occurrences;            // the number of occurrences

public:
    MetaDef(BagItem);

    BagItem(ObjPtr o, int aValue= 0)
	{ op= o; occurrences= aValue; }
    void FreeAll();

    ObjPtr Op() 
	{ return op; }
    int Occurrences()
	{ return occurrences; }
    void Inc()
	{ occurrences++; }
    void Dec()
	{ occurrences--; }
	    
    //---- generic methods
    unsigned long Hash();
    bool IsEqual(ObjPtr anOp);
    int Compare(ObjPtr anOp);
    ostream& PrintOn (ostream&);
    istream& ReadFrom (istream&);
};

#endif Bag_First
