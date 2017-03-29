#ifndef Dictionary_First
#ifdef __GNUG__
#pragma once
#endif
#define Dictionary_First

#include "Set.h"

//---- an association of the dictionary, a <key,value> tuple

class Assoc: public Object {
private:
    ObjPtr key;                 
    ObjPtr value;

public:
    MetaDef(Assoc);           

    Assoc (ObjPtr aKey, ObjPtr aValue)
	{ key= aKey; value= aValue; }
    void FreeAll();
    ObjPtr Key() 
	{ return key; }
    ObjPtr Value()
	{ return value; }
    ObjPtr SetValue(ObjPtr aValue) // returns old value
	{ ObjPtr t= value; value= aValue; return t; }
    void operator= (ObjPtr); // frees and deletes old value
	    
    //---- generic methods
    unsigned long Hash();
    bool IsEqual(ObjPtr);
    int Compare(ObjPtr);
    ostream& PrintOn (ostream&);
    istream& ReadFrom (istream&);
};

//---- Dictionary

typedef class Dictionary *DictionaryPtr;

class Dictionary: public Set {
    friend class DictIter;          // iterates over the associations
    friend class DictIterKeys;      // iterates over the keys of the dictionary
    friend class DictIterValues;    // iterates over the values of the dictionary
    bool AssertAssoc(char *where, Object *op);        
public:
    MetaDef(Dictionary);
    Dictionary(int s= cCollectionInitCap);
    ~Dictionary();
    void FreeAll();
    void FreeValues();
    void Empty(int initSize= 0);
    ObjPtr Add(ObjPtr);         // the given object has to be an instance
				// of an association
    ObjPtr Remove(ObjPtr);
    virtual ObjPtr AtKey(ObjPtr);               // returns the value associated with key
    virtual Assoc *AssociationAtKey(ObjPtr);    // returns the association of key
    virtual void AtKeyPut(ObjPtr,ObjPtr);       // associates a key with a value
						// the corresponding assoc is
						// put on the heap
    virtual ObjPtr RemoveKey(ObjPtr);           // remove association with the given key
    virtual bool ContainsKey (ObjPtr); 
    virtual bool ContainsAssoc (ObjPtr);        
    virtual bool Contains (ObjPtr);        
    virtual int OccurrencesOfKey (ObjPtr);
    
    //---- conversion
    virtual class Set* Keys();    // return all the keys of the dictionary as a set 
    virtual class Bag* Values();  // return all the values of the dictionary as a set
     
    //---- overloading
    Assoc& operator[](ObjPtr op) 
	{ return *AssociationAtKey(op); }
};

//---- Dictionary iterator -----------------------------------------------------

#ifndef __GNUG__
    typedef SetIter DictIter;
#else
#   define DictIter SetIter
#endif __GNUG__

//---- Dictionary iterator (keys) ----------------------------------------------

class DictIterKeys : public SetIter {
public:
    DictIterKeys(Collection *s);
    ObjPtr operator()();
};

//---- Dictionary iterator (values) --------------------------------------------

class DictIterValues : public SetIter {
public:
    DictIterValues(Collection *s);
    ObjPtr operator()();    
};

#endif Dictionary_First
