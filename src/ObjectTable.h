#ifndef ObjectTable_First
#ifdef __GNUG__
#pragma once
#endif
#define ObjectTable_First

#include "Types.h"

extern class ObjectTable *gObjectTable; // only instance of object table 

//---- Object Table ----------------------------------------------------------
// this class registers all instances of Object or its subclasses
// in a hash table

class ObjectTable {
    Object **table;
    int size;           
    int tally;
    int cursor;              // for iterating
    class Class *fromClass;  // looking for instances of class in operator()()
    bool members;            // return only direct members of a clas in "   "
    bool inIterator;        

    //---- statistics
    int convoi;
    long cumconvoi;
    int seed;

    void Expand(int newsize);
    bool HighWaterMark ()
	{ return (bool) (tally >= (3 * size /4)); }
    int FindElement (Object*);
    void FixCollisions (int index);
public:
    ObjectTable();
    ~ObjectTable();
    void HashStatistics();
    void AddObject(Object*);
    void RemoveObject(Object*);
    bool PtrIsValidOfObject(Object *);      // check wether the given pointer is valid
    void Start(Class *cl= 0, bool members= TRUE);
    Object *operator()();
    void End();
    void Verify();
    //---- friends
    static void Add(Object *op);
    static void AddRoot(Object *op); // add a root object
    static void AddRoots(Object*, ...);
    static void Remove(Object *op);
    static void UpdateInstCount();
    static bool PtrIsValid(Object *);
    static void VisitObjects();  // visit all objects, reachable 
					    // from root objects
    static int Instances();      // return total number of instances 
    static Object *SomeInstance(Class *);
    static Object *SomeMember(Class *);
};

#endif ObjectTable

