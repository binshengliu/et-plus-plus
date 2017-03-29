//$StringAssoc,AssocArray$
#include "AssocArray.h"

//---- StringAssoc -------------------------------------------------------------

MetaImpl(StringAssoc, (TP(dict), 0));

StringAssoc::StringAssoc(ObjPtr k, ObjPtr v) : (k, v)
{
    dict= 0;
}

void StringAssoc::FreeAll()
{
    Assoc::FreeAll();
    if (dict) {
	dict->FreeAll();
	SafeDelete(dict);
    }
}

StringAssoc& StringAssoc::operator[] (string s)
{
    if (dict == 0)
	dict= new AssocArray;
    return (*dict)[s];
}

char *StringAssoc::SValue()
{
    ByteArray *b= (ByteArray*) Assoc::Value();
    if (b == 0)
	return "no value";
    if (b->Str())
	return (char*) b->Str();
    return "not defined";
}

ostream& StringAssoc::PrintOn(ostream& s)
{
    Assoc::PrintOn(s);
    return s << dict SP;
}

istream& StringAssoc::ReadFrom(istream& s)
{
    Assoc::ReadFrom(s);
    return s >> Ptr(dict);
}

//---- AssocArray --------------------------------------------------------------

MetaImpl0(AssocArray);

StringAssoc &AssocArray::operator[] (string s)
{
    ByteArray *b= new ByteArray((byte*)s);
    StringAssoc *a= (StringAssoc*) AssociationAtKey(b);
    ObjPtr old;

    if (a == 0) {
	a= new StringAssoc(b, 0);
	old= Add(a);
	if (old) {
	    old->FreeAll();
	    delete old;
	}
    } else {
	delete b;
	if (! a->IsKindOf(StringAssoc))
	    Error("operator[]", "no StringAssoc");
    }
    return *a;
}
