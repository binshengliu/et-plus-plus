//$ObjFloat$

#include "ObjFloat.h"
#include "String.h"

MetaImpl(ObjFloat, (T(val), 0));

//---- a float object ----------------------------------------------------------

// hashing idea stolen from OOPS

static union {
    unsigned long aslong[2];
    double asdouble;
} hashunion;


unsigned long ObjFloat::Hash()
{
    hashunion.asdouble= val;
    return hashunion.aslong[0] ^ hashunion.aslong[1];
}

bool ObjFloat::IsEqual(Object* op)
{
    return (bool) (op->IsKindOf(ObjFloat) && val == ((ObjFloat*)op)->val);
}

int ObjFloat::Compare(Object* op)
{
    double t = val - Guard(op, ObjFloat)->val;
    if (t < 0) 
	return -1;
    if (t > 0)
	return 1;
    return 0;
}

Object *ObjFloat::DeepClone()
{
    return new ObjFloat(val);
}

char* ObjFloat::AsString()
{
    return form("%g", val);
}

ostream& ObjFloat::PrintOn (ostream &s)
{
    Object::PrintOn(s);
    return s << val SP;
}

istream& ObjFloat::ReadFrom(istream &s)
{
    Object::ReadFrom(s);
    return s >> val;
}

void ObjFloat::InspectorId(char *buf, int)
{
    sprintf(buf,"%g", val);   
}

