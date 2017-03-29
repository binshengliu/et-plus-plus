#ifndef ObjFloat_First
#ifdef __GNUG__
#pragma once
#endif
#define ObjFloat_First

#include "Object.h"

class ObjFloat: public Object {
    double val;
public:
    MetaDef(ObjFloat);

    ObjFloat(double v= 0.0)
	{ val= v; }

    double GetValue()                     
	{ return val; }
    double SetValue(double newval)           
	{ val= newval; Changed(); return val; }
    double operator= (double newval)
	{ return SetValue(newval); }
    operator double()
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
    void InspectorId(char *buf, int sz);
};

#endif ObjFloat_First

