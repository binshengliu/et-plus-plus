//$ObjInt$

#include "ObjInt.h"
#include "String.h"

MetaImpl(ObjInt, (T(val), 0));

//---- an integer object -------------------------------------------------------

unsigned long ObjInt::Hash()
{
    return (unsigned long) val;
}

bool ObjInt::IsEqual(Object* op)
{
    return op->IsKindOf(ObjInt) && val==((ObjInt*)op)->val;
}

int ObjInt::Compare(Object* op)
{
    return val - Guard(op, ObjInt)->val;
}

Object *ObjInt::DeepClone()
{
    return new ObjInt(val);
}

char* ObjInt::AsString()
{
    return form("%d", val);
}

ostream& ObjInt::PrintOn(ostream &s)
{
    Object::PrintOn(s);
    return s << val SP;
}

istream& ObjInt::ReadFrom(istream &s)
{
    Object::ReadFrom(s);
    return s >> val;
}

void ObjInt::InspectorId(char *buf, int)
{
    sprintf(buf, "%d", val);
}

