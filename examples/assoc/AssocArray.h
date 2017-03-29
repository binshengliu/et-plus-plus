#ifndef AssocArray_First
#define AssocArray_First

#include "Dictionary.h"
#include "ByteArray.h"

typedef char *string;


class StringAssoc: public Assoc {
    class AssocArray *dict;
public:
    MetaDef(StringAssoc);
    
    StringAssoc(ObjPtr k, ObjPtr v);

    void FreeAll();
    void operator= (char *s)
	{ Assoc::operator= (new ByteArray((byte*)s)); }
    char *SValue();
    friend ostream &operator<< (ostream &s, StringAssoc &a)
	{ return s << a.SValue(); }
    operator string()
	{ return SValue(); }
    StringAssoc& operator[] (string s);
    ostream& PrintOn(ostream& s);
    istream& ReadFrom(istream& s);
};

class AssocArray: public Dictionary {
public:
    MetaDef(AssocArray);
    AssocArray(int s= 50) : (s)
	{ }
    
    StringAssoc& operator[] (string s);
};

#endif AssocArray_First

