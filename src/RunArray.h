#ifndef RunArray_First
#ifdef __GNUG__
#pragma once
#endif
#define RunArray_First

#include "Collection.h"

//---- class RunArray --------------------------------------------------

typedef class RunArray *RunArrayPtr;

class RunArray: public Collection {
friend class RunArrayIter;
    Object **cont;          // array with objects of runs
    int *runlength;         // array with length of runs
    int size;               // size of allocated memory
    int count;              // number of runs
    int length;             // length of runarray
    //---- used for faster access:
    int current;            // index of current element
    int offset;             // start of current element
    //---- used to store a run with length 0
    Object  *nullrun;       // object of run with length 0
    int     nullpos;        // position of run with length 0

    void OutOfRangeError(char *where, int at);
protected:    
    void MoveTo(int );
    void Shift(int , int);
    void InsertRuns(int, int, ObjPtr*, int*, int, bool free = FALSE );
    void CopyRuns(ObjPtr*, int*, int, int, int );
    int EndOfRun()
	{ return (current < count ? offset + runlength[current]: length)  ; }
    bool IsInRun(int at)
	{ return (bool) (current < count ? at >= offset && at < EndOfRun(): at == length);}
    void NextRun()
	{ offset += runlength[current++]; }
    void PrevRun()
	{ offset -= runlength[--current]; }
public:
    MetaDef(RunArray);               
    //---- creation, destruction  
    RunArray(int elements = cCollectionInitCap);                
    ~RunArray();
    void FreeAll();
    ObjPtr Add(ObjPtr ob);
    ObjPtr Remove(ObjPtr);

    //---- accessing
    Iterator *MakeIterator();           // return a Iterator iterator of a collection
    int ContainsPtrAt (ObjPtr);     // returns -1 if not found     
    int ContainsAt (ObjPtr);        //        "           "

    void Insert(ObjPtr op, int from, int to, int len, bool free = FALSE);
    void ChangeRunSize(int i, int shift = 1, bool free = FALSE);
    void Cut(int from, int to, bool free = FALSE);
    void Paste(RunArray *paste, int from, int to, bool free = FALSE);
	// if free is TRUE, the replaced objects are freed with "delete"
    void Copy(RunArray *save, int from, int to);
    RunArrayPtr Save(int from, int to);

    ObjPtr& operator[](int i);      // shorthand notation
    ObjPtr RunAt(int i, int *start, int *end, int *size ,int *lenat);
    int LengthAt(int i);            // returns remaining length of run
    int RunSizeAt(int i);           // returns size of run
    int NumberOfRuns() 
	{ return count ; }         // number of runs
    int Size()
	{ return length ; }          // total length of runs

    //---- standard overriden methods 
    bool IsEqual(ObjPtr op);
    ostream& PrintOn(ostream& s);
    istream& ReadFrom(istream& s);  
    virtual Object *ReadItem(istream& s); 
    ostream& DisplayOn(ostream& s);
    void Dump();
    void InspectorId(char *buf, int sz);
};

class RunArrayIter: public Iterator {
    int ce;         // index of runelement
    int ci;         // index of runarray
    int cp;

    RunArray *cs;
public:
    RunArrayIter(Collection *s)
	{ cs= (RunArray*)s; ce = ci = cp = 0; }
    void Reset(Collection*);
    ObjPtr operator()();            
    ObjPtr Run(int *start, int *end, int *size);
	// iterate by runs
    ObjPtr *RunPtr(int *start, int *end, int *size);
	// iterate by runs and return address of run
    bool Filter(ObjPtr);
};

#endif RunArray_First

