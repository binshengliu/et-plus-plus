//$ByteArray$
#include "ByteArray.h"
#include "String.h"

char *cAtPutName= "AtPut";

MetaImpl(ByteArray, (TV(cont,cap), T(cap), 0));

ByteArray::ByteArray(byte *aStr, int l)                
{
    if (aStr && (l != 0)) {
	if (l < 0)
	    l= strlen((char*)aStr)+1;
	cont= new byte[cap= l];
	BCOPY(aStr, cont, l);
    } else
	cont= new byte[cap= 10];
}

ByteArray::ByteArray(char *aStr, int l)                
{
    if (aStr && (l != 0)) {
	if (l < 0)
	    l= strlen(aStr)+1;
	cont= new byte[cap= l];
	BCOPY((byte*)aStr, cont, l);
    } else
	cont= new byte[cap= 10];
}

ByteArray::ByteArray(int size)                
{
    if (size <= 0)
	size= 10;
    cont= new byte[cap= size];
}

ByteArray::~ByteArray()               
{
    SafeDelete(cont);
}

void ByteArray::operator= (byte *s)
{
    strreplace((char**)&cont, (char*)s);
    cap= strlen((char*)s);
}

unsigned long ByteArray::Hash()
{
    register unsigned long hash;
    register byte *p;

    for (hash = 0, p = cont; *p; p++)
	hash = (hash << 1) ^ *p;
    return hash;
}

bool ByteArray::IsEqual(ObjPtr b)
{
    return b->IsKindOf(ByteArray)
	&& strcmp((char*)cont, (char*)Guard(b, ByteArray)->cont) == 0;
}

int ByteArray::Compare(ObjPtr b)
{
    return strcmp((char*)cont, (char*)Guard(b, ByteArray)->cont);
}

void ByteArray::SetString(byte *s)
{
   SafeDelete(cont);
   cap= strlen((char*)s)+1;
   cont= new byte[cap];
   BCOPY(s, cont, cap);
}

ostream& ByteArray::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    return PrintString(s, cont, cap);
}

istream& ByteArray::ReadFrom(istream& s)
{
    Object::ReadFrom(s);
    SafeDelete(cont);
    return ReadString(s, &cont, &cap);
}

void ByteArray::InspectorId(char *buf, int sz)
{
    strn0cpy(buf, (char*)Str(), sz);
}

