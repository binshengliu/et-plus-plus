#ifndef ObjInt_First
#ifdef __GNUG__
#pragma once
#endif
#define ObjInt_First

#include "Object.h"

class ObjInt: public Object {
    int val;
public:
    MetaDef(ObjInt);

    ObjInt(int v= 0)
	{ val= v; }

    int GetValue()                     
	{ return val; }
    int SetValue(int newval)           
	{ val= newval; Changed(); return val; }
    int operator= (int newval)
	{ return SetValue(newval); }
    int operator++ ()
	{ return SetValue(GetValue()+1); }
    int operator-- ()
	{ return SetValue(GetValue()-1); }
    operator int()
	{ return val; }

    //---- comparing
    unsigned long Hash ();
    bool IsEqual (Object*);
    int Compare (Object*);

    //---- converting
    char* AsString();

    //---- activation passivation
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);

    Object *DeepClone();
    void InspectorId(char *, int);
};

#endif ObjInt_First

