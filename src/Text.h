#ifndef Text_First
#ifdef __GNUG__
#pragma once
#endif
#define Text_First

#include "Object.h"
#include "String.h"
#include "Font.h"
#include "Port.h"

//---- descriptor of a text line --------------------------------------------

struct LineDesc {
    int lnAscent;
    int lnHeight;
    LineDesc(int b = 0, int h = 0);
    void Reset();
    ostream& DisplayOn (ostream&s);

    bool IsEqual(LineDesc l)
	{ return (bool) (lnAscent == l.lnAscent && lnHeight == l.lnHeight); }

    void Max(FontPtr f);
    void Max(LineDesc ld);
    void Max(int ascent, int height);

    void FromFont(FontPtr f)
	{ lnAscent = f->Ascender(); lnHeight = f->Spacing(); }
};

//---- abstract class Text ----------------------------------------------

const int cEOT = -1,
	  cTabw = 40;

inline bool CheckRange (int max, int from, int to)
{
    return (to > max || from < 0 || from > to) ? FALSE: TRUE;
}

//---- change protocol -------------------------------------------------

// text is an abstract class defining methods to manage a buffer for holding 
// text. Together with the text there is a list of robust pointers (called marks)
// into the text. Dependents of the Text classes are notified by 
// the observer mechanism
// and receive a change record as described below as argument

enum eTextChanges { 
    eTextChangedRange, 
    eTextDeleted,
    eTextReplaced       // from,to=> replaced range, size=>size of inserted text
};

struct TextChanges {
    int from;
    int to;
    int size;
    TextChanges *operator()(int f, int t, int s= 0)
	{ from= f; to= t; size= s; return this; }
};

//---- Text -----------------------------------------------------------------

typedef class Text *TextPtr;

class Text: public Object { 
    class MarkList *marks;
    class Collection *observers;  
    int tabWidth; 
							       
protected:
    Font *font;                        
    Ink *ink;                        

    Text(); 
    virtual void CalcIws (int width, int from, int *to, int *addSpace, 
						int *longBlanks, int *numTabs);
	// calculate the interword spacing for a justified text line
	// 'to' is set to the position of the last non blank character
    virtual void SetFStringVL(char *fmt, va_list ap); 
    virtual int GrowBy(int desiredSize);
    
	// optimized observing 
    class Collection *MakeObserverColl();
    class Collection *GetObservers();
    void DestroyObserverColl();
    void SetObserverColl(Collection *);
public:    
    MetaDef(Text);
    void InitNew();

    ~Text();

    //---- editing
    virtual void Cut(int from,int to);   
    virtual void Paste(TextPtr t,int from,int to); 
    virtual void Insert(byte c, int from,int to);
    virtual void Append(byte c); 
    virtual void Copy(Text* save,int from, int to); // abstract

    //---- converting
    virtual void ReplaceWithStr(byte *str,int len = -1); 
    virtual void CopyInStr(byte *str,int strSize,int from, int to); 
    char *AsString();
    int  AsInt();
    float AsFloat(); 

    //---- accessing
    void SetFString(char *fmt, ...);
    virtual byte *GetTextAt(int from, int to);  // obsolete
    virtual Text *GetScratchText(int size); 
    virtual Text *Save(int from, int to); // abstract
	// allocate new text object and copy the given range
    virtual int SetTabWidth(int); // returns old tabWidth
    virtual int Tabulate(int cx); 
	// return the width of a tabulator based on cTabw
    virtual void Empty(int initSize = 0);  //abstract
    virtual byte& operator[](int i); //abstract      
    virtual int Size(); // abstract
    virtual bool IsEmpty();
    virtual int Search(class RegularExp *rex,int *nMatched, int start = 0, 
	     int range = cMaxInt, bool forward = TRUE);
	    // search rex and return position of match (-1 == no match)

    //---- fonts and metric
    virtual void SetFont(FontPtr fd); 
    virtual FontPtr GetFont(int at = 0);
    virtual void SetInk(Ink*);
    virtual Ink *GetInk(int at = 0); 
    virtual void ResetCurrentStyle();
    
    virtual int TextWidth(int from, int to); // abstract
    virtual void DrawText(int from, int to, Rectangle clip); 
    virtual void DrawTextJust (int from, int to, int w, Point start, Rectangle clip); 
	// draw the text in the given range at the current text position,abstract
    virtual void JustifiedMap(int from, int to, int w, int stopAt, int posX, 
							   int *charPos, int *x);
    virtual void Map(int from, int to, int stopAt, int posX, 
							   int *charPos, int *x);
    FontPtr SetDrawingState(int at);
    
    //---- iterator
    virtual class TextIter *MakeIterator(int from=0, int to= cMaxInt); //abstract

    //---- utilities
    virtual void GetWordBoundaries(int at,int *start,int *end);
    virtual void GetParagraphBoundaries(int at,int *start,int *end);

    //---- Marks
    virtual void AddMark(class Mark *);
    virtual Mark *RemoveMark(class Mark *);
    virtual class Iterator *GetMarkIter(); 
    virtual class MarkList *GetMarkList();    // ????   

    //---- input/output
    virtual ostream& PrintOnAsPureText(ostream &s);
    virtual istream& ReadFromAsPureText(istream &s, long sizeHint= -1);

    //---- inspecting 
    void InspectorId(char *b, int s);
};

//---- abstract class TextIter ----------------------------------------------

class TextIter : public Root {
protected:
    int ce;
    int upto;
    Text *ct;
    int unget;
public:
    TextIter(Text *s,int from = 0,int to = cMaxInt);        
    virtual int operator()();      // abstract
    virtual int operator()(int *width, LineDesc* l = 0); // abstract                                          
    virtual void Reset(Text *s,int from = 0, int to = cMaxInt);        
    virtual int Line(LineDesc* l = 0); // return end of next line, abstract
    virtual int Token(int *width, LineDesc* l = 0); // return next token and width, abstract
    virtual int GetPos();
    virtual Font *FontAt(int);
    virtual int GetLastPos();                 // get last position
    virtual void SetPos(int newPos);
    virtual int Unget();                     // unget last token
};

class AutoTextIter { 
    TextIter *ti;
public:
    AutoTextIter(Text *t, int from= 0, int to= cMaxInt);
    AutoTextIter(TextIter *tip)
	{ ti= tip; }
    ~AutoTextIter()
	{ if (ti) delete ti; }
    int operator()()
	{ return (*ti)(); }
    void Reset(Text *s,int from= 0, int to= cMaxInt)
	{ ti->Reset(s,from,to); }        
    int operator()(int *width, LineDesc* l= 0) 
	{ return (*ti)(width,l); }
    int Line(LineDesc* l= 0)
	{ return ti->Line(l); } 
    int Token(int *width, LineDesc* l= 0)
	{ return ti->Token(width,l); }
    int GetPos()
	{ return ti->GetPos(); }
    Font *FontAt(int i)
	{ return ti->FontAt(i); }
    int GetLastPos()  
	{ return ti->GetLastPos(); }               
    void SetPos(int newPos)
	{ ti->SetPos(newPos); }
    int Unget()
	{ return ti->Unget(); }                     
};


#endif Text_First

