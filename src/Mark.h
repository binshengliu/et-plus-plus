#ifndef Marks_First
#ifdef __GNUG__
#pragma once
#endif
#define Marks_First

#include "OrdColl.h"

//---- constants passed to DoObserve to the dependents of a mark

enum eMarkChanges  {
    eMarkNone = 0,
    eMarkDeleted,      // mark deleted
    eMarkLength,       // text in mark range changed
    eMarkPos           // position of the mark changed
};

//---- states of a mark

enum eMarkState { 
    eStateNone = 0, 
    eStateDeleted = 1,  
    eStateChanged = 2,     
};

enum eMarkFlags {
    eMarkInclStart  =   BIT(eObjLast+1),
    eMarkFixedSize  =   BIT(eObjLast+2),
    eMarkLocked     =   BIT(eObjLast+3),   // mark won't move     
    eMarkDefault    =   eMarkInclStart,
    eMarkLast       =   eObjLast + 3
};

//---- class Mark -------------------------------------------------------

class Mark: public Object {
public:
    int pos;
    int len;
    int state;

public:
    MetaDef(Mark);
    Mark(int p= 0, int l= 0, eMarkState s= eStateNone, eMarkFlags f= eMarkDefault);
    Mark(Mark *m);
    void ChangeMark (int p, int l, int s= 0)
	{ pos = p; len = l; state = s; }
    bool HasChanged (int start,int len);
    int Len()
	{ return len; }
    int Pos()
	{ return pos; }
    int End()
	{ return pos+len; }
    eMarkState State()
	{ return (eMarkState)state; }
    void Lock()
	{ SetFlag(eMarkLocked); }
    void Unlock()
	{ ResetFlag(eMarkLocked); }
    int Compare (Object* op);
    Object *DeepClone();
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &s);
    ostream& DisplayOn (ostream &s);
};

//---- class MarkList -------------------------------------------------------

class MarkList: public OrdCollection {
    bool doRemove;
public:
    MetaDef(MarkList);
    MarkList(bool doRemove = FALSE);    // remove the deleted marks
					// from the marklist
    void Paste(int at,int len);         // insert len characters
    void Cut(int at,int len);           // delete len characters
    void RangeChanged(int at, int len); // mark marks in the given range as changed
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &s);
};

#endif Marks_First     
