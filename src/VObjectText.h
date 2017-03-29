#ifndef VObjectText_First
#ifdef __GNUG__
#pragma once
#endif
#define VObjectText_First

#include "StyledText.h"
#include "Mark.h"

const cVObjectChar  = '#'; // chararcter to mark a VObject

enum VobMarkFlags {
    eVobInvalid     = eMarkLast + 1,
    eVobMarkLast    = eMarkLast + 1
};

//------ VObjectMark -----------------------------------------------------------

class VObjectMark: public Mark {
    class VObject *gop;
public:
    MetaDef(VObjectMark);
    
    VObjectMark(int p= 0, int l= 0, VObject *go= 0, eMarkState s= eStateNone);
    ~VObjectMark();

    VObject *GetVObject()
	{ return gop; }
    void SetVObject(VObject *go)
	{ gop = go; }
    void Invalidate()
	{ SetFlag(eVobInvalid); }
    bool IsInvalidated()
	{ return TestFlag(eVobInvalid); }
    void Validate()
	{ ResetFlag(eVobInvalid); }
    ObjPtr DeepClone();
    void FreeAll();
    ostream &PrintOn(ostream &s); 
    istream &ReadFrom(istream &s); 
    void Parts(Collection *col);
};

//------ VObjectText -----------------------------------------------------------

class VObjectText: public StyledText {                                                           
    class MarkList *marks;
    class TextView *tv;
    void Init();
public:
    MetaDef(VObjectText);
    
    VObjectText();
    VObjectText(int size, FontPtr fd= gSysFont);
    VObjectText(byte *buf, int len = -1, bool ic = FALSE, FontPtr fd= gSysFont);
    VObjectText(class TextRunArray *st, byte *buf, int len = -1, bool ic = FALSE);
    ~VObjectText();
    void InitNew();

    TextView *GetTextView();
    void ReplaceWithStr(byte *str,int len = -1); 
    void Cut(int from,int to);
    void Paste(TextPtr t,int from,int to);
    void Insert(byte c, int from,int to);
    void Copy(Text* save,int from, int to);
    TextPtr Save(int from, int to);

    class Command* InsertVObject(VObject *gop);
    VObject *ContainsPoint(Point p);
    VObject *MarkAt(int charNo);
    VObjectMark *GetMarkAt(int charNo);
    int FindPositionOfVObject(VObject *);   // -1 VObject not found
    void SetView(class TextView*);
    bool IsVObjectChar(int charNo)
	{ return (bool) ((*this)[charNo] == cVObjectChar); }
    bool IsVObject(int charNo)
	{ return (bool) (IsVObjectChar(charNo) && MarkAt(charNo)); }
    bool IsEscape (int at);
    int CalcEscape (int at,LineDesc* = 0);
    void DrawEscape (int at,Rectangle clip);
    void VObjectChangedSize(VObject *);
    Iterator *VObjectIterator();
    int VObjectCount();
    
    //---- activation/passivation
    ostream &PrintOn(ostream &s); 
    istream &ReadFrom(istream &s); 
    void Parts(Collection *c);
};

#endif VObjectText_First
