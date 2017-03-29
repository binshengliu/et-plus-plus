//$BitSet,BitSetIter$
#include "BitSet.h"

MetaImpl(BitSet, (T(m), 0));

ObjPtr BitSet::DeepClone()
{
    return Clone();
}

int BitSet::Capacity()     
{ 
    return sizeof(int)*8; 
}

u_long BitSet::Hash() 
{   
    return m; 
}
	
bool BitSet::IsEmpty()  
{ 
    return m==0; 
}
	
bool BitSet::IsEqual(Object* ob)
{
    return ob->IsKindOf(BitSet) && m == ((BitSet*)ob)->m;
}

ostream& BitSet::PrintOn (ostream& s)
{
    Object::PrintOn(s);
    return s << m SP;  
}

istream& BitSet::ReadFrom (istream& s)
{
    long mm;
    Object::ReadFrom(s);
    s >> mm;
    m= (u_long) mm;
    return s;
}

int BitSet::Size()
{
    register u_long l, n;

    for (l= m, n= 0; l != 0; n++)
	l &= (l-1);     // removes rightmost 1 
    return n;
}

//---- class BitSetIter ----------------------------------------------------

void BitSetIter::Reset(BitSet *s)
{
    cb= s;
    pos= 0;
}
	
int BitSetIter::operator()()
{
    while (!cb->Includes(pos) && pos < cb->Capacity())
	pos++;
    if (pos == cb->Capacity())
	return 0;
    return pos++;
}


