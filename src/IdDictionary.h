#ifndef IdDictionary_First
#ifdef __GNUG__
#pragma once
#endif
#define IdDictionary_First

#include "Dictionary.h"

//----- class IdDictionary (IdentityDictionary) tests the identity of 
//      the keys and not equality (IsEqual)
//      

class IdDictionary: public Dictionary {
protected:
    u_long HashObject(Object *);
    bool   ObjectsEqual(Object *op1, Object *op2);
public:
    MetaDef(IdDictionary);
    IdDictionary(int s= 0);
    Set* Keys();     // return all the keys of the dictionary as a set (IdSet)
};

//---- class IdSet (IdentitySet) ----------------------------------------

class IdSet: public Set {
protected:
    u_long HashObject(Object *);
    bool   ObjectsEqual(Object *op1, Object *op2);
public:
    MetaDef(IdSet);
    IdSet(int s= 0);
};

#endif IdDictionary_First
