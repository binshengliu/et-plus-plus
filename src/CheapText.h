#ifndef CheapText_First
#ifdef __GNUG__
#pragma once
#endif
#define CheapText_First

#include "Text.h"

//---- CheapText ------------------------------------------

class CheapText: public Text {                                                           
friend class CheapTextIter;
    int next;                               // next free slot
    int size;                               // size of allocated memory
    byte *cont;

    void Init(int, FontPtr);
    void Expand(int);    
    bool HighWaterMark(int n)
	{ return (bool) (cont == NULL || next+n >= size); }
    bool LowWaterMark()                    
	{ return (bool) (next < size/5); }  

protected:
    byte CharAt(int i)
	{ return cont[i]; }
    void SetFStringVL(char *fmt, va_list va);
public:
    MetaDef(CheapText);
    CheapText(int s = 16, FontPtr fd= gSysFont);
    CheapText(byte *buf, int len= -1, FontPtr fd= gSysFont);
    ~CheapText();

    //---- editing
    void Cut(int from,int to);
    void Paste(TextPtr t,int from,int to);
    void Insert(byte c, int from,int to);
    void Copy(Text* save,int from, int to);
    void CopyInStr(byte *str,int strSize,int from, int to);
    void ReplaceWithStr(byte *str,int len = -1); 
    byte *GetTextAt(int from, int to);
    Text *Save(int from, int to); 
	// allocate new text object and copy the given range

    //---- accessing
    void Empty(int initSize = 0);
    byte& operator[](int i);      
    int Size();
    bool IsEmpty();
    int TextWidth(int from, int to);
    void DrawText(int from, int to, Rectangle clip);
    int Search(class RegularExp *rex,int *nMatched, int start = 0, 
	     int range = cMaxInt, bool forward = TRUE);

    //---- iterators
    TextIter *MakeIterator(int from=0, int to = cMaxInt);

    //---- generic object methods
    unsigned long Hash ();
    bool IsEqual (ObjPtr t);

    bool IsEqualStr (byte *);

    //---- input/output
    ostream& PrintOn(ostream&s);
    istream& ReadFrom(istream& s);
    ostream& PrintOnAsPureText (ostream &s);
    istream& ReadFromAsPureText (istream& s, long sizeHint= -1);
};

//---- class CheapTextIter -------------------------------------------------

class CheapTextIter: public TextIter {
    FontPtr font;
public:
    CheapTextIter(Text *s,int from = 0,int to = cMaxInt);
    int operator()();      // returns cEOT at end
    int operator()(int *width,LineDesc* l = 0);// returns width of the character too
    int Line(LineDesc* l = 0);       // return position of next line end (\r,\n)
    int Token(int *width,LineDesc* l = 0); // returns next token and its size
};

#endif CheapText_First
