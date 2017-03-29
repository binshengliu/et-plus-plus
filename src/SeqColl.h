#ifndef SeqColl_First
#ifdef __GNUG__
#pragma once
#endif
#define SeqColl_First

#include "Collection.h"

//---- abstract class SeqCollection (sequencable collections) ---------------
//     sequenceable collections have an ordering relation, eg there is a first
//     and last element

typedef class SeqCollection *SeqCollPtr;

class SeqCollection: public Collection {
protected:
    SeqCollection();
    
public: 
    MetaDef(SeqCollection);
    virtual void AddFirst(ObjPtr);
    virtual void AddLast(ObjPtr);
    virtual ObjPtr RemoveFirst();
    virtual ObjPtr RemoveLast();
    virtual void InsertBefore(ObjPtr before, ObjPtr a);     // abstract
    virtual void InsertAfter(ObjPtr after, ObjPtr a);       // abstract
    virtual void InsertBeforePtr(ObjPtr before, ObjPtr a);  // abstract
    virtual void InsertAfterPtr(ObjPtr after, ObjPtr a);    // abstract

    virtual Iterator *MakeReversedIterator(); // abstract
    //---- accessing
    virtual ObjPtr Before(ObjPtr); // abstract
    virtual ObjPtr After(ObjPtr);  // abstract
    virtual ObjPtr First();   // abstract
    virtual ObjPtr Last();    // abstract
    virtual int IndexOf(ObjPtr); // returns -1 if not found, based on IsEqual
    virtual int IndexOfPtr(ObjPtr); // returns -1 if not found, based on identity 
				    // not equality
    int LastIndex()
	{ return Size()-1; }
};

//---- class RevIter ----------------------------------------------------------

class RevIter {
    Iterator *seq;
public:
    RevIter(SeqCollection *col)
	{ seq= col->MakeReversedIterator(); }
    ~RevIter()
	{ if (seq) delete seq; }
    class Object *operator()()
	{ return (*seq)(); }
    void Reset(SeqCollection *col)
	{ seq->Reset(col); }
};

#endif SeqColl_First
