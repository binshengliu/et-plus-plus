#ifndef GapText_First
#ifdef __GNUG__
#pragma once
#endif
#define GapText_First

#include "Text.h"

//---- class GapText ------------------------------------------------------

typedef class GapText *GapTextPtr;

extern char *cMemOverflow;
extern char *cGapTextName;

class GapText: public Text  {  
    friend class GapTextIter;

    int size;                               // size of allocated memory
    int length;                             // length of text
    int part1len;                           // length of text before gap
    int part2len;                           // redundant
    int gaplen;                             // length of gap
    byte *body;                             // access to text befor gap
    byte *part2body;                        // access to text behind gap
    byte *body2;                            // redundant

private:
    void Update(int l)
	{
	    part1len= l;
	    gaplen= size - length;
	    part2body= body + gaplen;
	    part2len= length - part1len;
	    body2= part2body + part1len;
	}
    void Init(int l, FontPtr fd);
    void Expand(int to = 0, int moveto = cMaxInt);
    void Shrink(int to = 0);
    void MoveGap(int to);
    void CopyTo(byte *dst, int from, int to);
    bool HighWaterMark(int n)
	{ return (bool) (body == 0 || gaplen <= n ); }
    bool LowWaterMark()
	{ return (bool) (size / 5 > length) ;}
    void CheckPtr(byte *ptr)
	{ if (ptr == NULL) Error(cGapTextName, cMemOverflow); }

protected:
    byte CharAt(int i)
	{ return i < part1len ? body[i] : part2body[i]; } 
    void SetFStringVL(char *fmt, va_list va);

public:
    MetaDef(GapText);

    GapText(int s = 16, FontPtr fd = gSysFont);
    GapText(byte *buf, int len = -1, bool ic = FALSE, FontPtr fd = gSysFont);
    ~GapText();

    //---- editing
    void Cut(int from,int to);
    void Paste(TextPtr paste,int from,int to);
    void Insert(byte c, int from, int to);
    void Copy(Text *save,int from, int to);
    void CopyInStr(byte *str,int strsize,int from, int to);
    void ReplaceWithStr(byte *str, int len = -1);
    byte *GetTextAt(int from, int to);
    TextPtr Save(int from, int to);
	// allocate new text object and copy the given range

    //---- accessing
    void Empty (int initSize = 0);
    byte& operator[](int i);
    int Size();
    bool IsEmpty();
    int TextWidth(int from, int to);
    void DrawText(int from, int to, Rectangle clip);
    int Search(class RegularExp *rex,int *nMatched, int start = 0, 
	       int range = cMaxInt, bool forward = TRUE);

    //---- iterators
    TextIter *MakeIterator(int from = 0, int to = cMaxInt);

    //---- generic object methods
    unsigned long Hash();
    bool IsEqual(ObjPtr t);
    bool IsEqualStr (byte *ch);

    //---- input/output
    ostream& PrintOn (ostream& s);
    istream& ReadFrom (istream& s);
    ostream& PrintOnAsPureText (ostream &s);
    istream& ReadFromAsPureText (istream& s, long sizeHint= -1);
    void Dump(); // print info for testing
};

//---- class GapTextIter --------------------------------------------------

class GapTextIter: public TextIter {
    FontPtr font;
public:
    GapTextIter(Text *s,int from = 0 , int to = cMaxInt);

    int operator() ();   // returns cEOT at End
    int operator () (int *width,LineDesc*);  // returns width of chars too
    int Line(LineDesc*);            // return position of next line end (\r,\n)
    int Token(int *width,LineDesc*);// returns next token and its size
};


#endif GapText_First    
