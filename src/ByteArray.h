#ifndef ByteArray_First
#ifdef __GNUG__
#pragma once
#endif
#define ByteArray_First

#include "Object.h"

//---- class ByteArray ---------------------------------------------------------
extern char *cAtPutName;
extern char *cOutOfBoundsError;

class ByteArray: public Object {
    byte *cont;
    int  cap;
public:
    MetaDef(ByteArray);

    //---- creation, destruction  
    ByteArray(int size);              
    ByteArray(byte *aStr, int l= -1);               
    ByteArray(char *aStr, int l= -1);               
    ~ByteArray();

    //---- accessing
    byte *Str ()
	{ return cont; }
    void SetString (byte *s); 
    void operator= (byte *s);
    char At(int i)
	{ return cont[i]; }
    void AtPut(int i, byte c)
	{ if ( i < 0 || i >= cap)
	    Error (cAtPutName, cOutOfBoundsError);
	  else
	    cont[i] = c; 
	}

    //---- standard overriden methods   
    unsigned long Hash ();
    bool IsEqual (ObjPtr);
    int  Compare (ObjPtr);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom (istream& s);
    void InspectorId(char *buf, int sz);
};

#endif ByteArray_First
