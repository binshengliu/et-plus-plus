#ifndef BitSet_First
#ifdef __GNUG__
#pragma once
#endif
#define BitSet_First

//------ class BitSet: a set of small integers (adapted from the oops library)

#include "Object.h"

class BitSet: public Object {
protected:
    u_long m;
    
    BitSet(unsigned long i, double) // second parameter double is a trick 
				    // to overload BitSet(unsigned long i1);        
	{ m= i; }
public:
    MetaDef(BitSet);

    BitSet()                        
	{ m= 0; }
    BitSet(int i1)                  
	{ m= BIT(i1); }
    BitSet(int i1, int i2)        
	{ m= BIT(i1)|BIT(i2); }
    BitSet(int i1, int i2, int i3)
	{ m= BIT(i1)|BIT(i2)|BIT(i3); }
    BitSet(int i1, int i2, int i3, int i4)
	{ m= BIT(i1)|BIT(i2)|BIT(i3)|BIT(i4); }
    BitSet(int i1, int i2, int i3, int i4, int i5)
	{  m= BIT(i1)|BIT(i2)|BIT(i3)|BIT(i4)|BIT(i5); }
    BitSet(int i1, int i2, int i3, int i4, int i5, int i6)
	{ m= BIT(i1)|BIT(i2)|BIT(i3)|BIT(i4)|BIT(i5)|BIT(i6); }
    BitSet(int i1, int i2, int i3, int i4, int i5, int i6, int i7)
	{ m= BIT(i1)|BIT(i2)|BIT(i3)|BIT(i4)|BIT(i5)|BIT(i6)|BIT(i7); }
    BitSet(BitSet *n)         
	{ m= n->m; }

    BitSet operator~()              
	{ return BitSet(~m, 0.0); }
    BitSet operator-(BitSet n)      
	{ return BitSet(m & ~n.m, 0.0); }
    bool operator>(BitSet n)        
	{ return m == (m|n.m) && m != n.m; }
    bool operator<(BitSet n)        
	{ return n.m == (m|n.m) && m != n.m; }
    bool operator>=(BitSet n)       
	{ return m == (m|n.m); }
    bool operator<=(BitSet n)       
	{ return n.m == (m|n.m); }
    bool operator==(BitSet n)       
	{ return m == n.m; }
    bool operator!=(BitSet n)       
	{ return m != n.m; }
    BitSet operator&(BitSet n)      
	{ return BitSet(m & n.m, 0.0); }
    BitSet operator^(BitSet n)      
	{ return BitSet(m ^ n.m, 0.0); }
    BitSet operator|(BitSet n)      
	{ return BitSet(m | n.m, 0.0); }
    // void operator=(BitSet n)        { m = n.m; } // necessary???
    void operator-=(BitSet n)       
	{ m &= ~n.m; }
    void operator&=(BitSet n)       
	{ m &= n.m; }
    void operator^=(BitSet n)       
	{ m ^= n.m; }
    void operator|=(BitSet n)       
	{ m |= n.m; }
    int AsMask()                    
	{ return m; }
    bool Includes(int i)            
	{ return (m & BIT(i)) != 0; }
    bool Contains(int i)            
	{ return Includes(i); }
    ObjPtr DeepClone();
    unsigned long Hash();
    bool IsEmpty();
    bool  IsEqual (Object*);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
    int Size();
    int Capacity();
};

//---- class BitSetIter ---------------------------------------------/

class BitSetIter {
    BitSet *cb;
    int pos;
public:
    BitSetIter(BitSet *s)
	{ cb=s; pos= 0; }
	
    void Reset(BitSet *);
    int operator()();
};

#endif BitSet_First
