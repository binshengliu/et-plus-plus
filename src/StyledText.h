#ifndef StyledText_First
#ifdef __GNUG__
#pragma once
#endif
#define StyledText_First

#include "GapText.h"
#include "RunArray.h"

overload new_Style;

//------ Style ----------------------------------------------------------------

class Style: public Object {
friend class StyleTable;
    FontPtr fp;
    GrPattern ink;
    
    Style(int, FontPtr f, GrPattern ink= ePatBlack);

public:

    MetaDef(Style);

    friend Style *new_Style(int, FontPtr f, GrPattern= ePatBlack);
    friend Style *new_Style(FontPtr f= gSysFont, GrPattern= ePatBlack);
    friend Style *new_Style(GrFont ft, GrFace fc, int Size, GrPattern= ePatBlack);
    
    ~Style();
    bool IsEqual (Object*);
    void GetStyle (GrFont *fid, GrFace *face, int *size, GrPattern *ink);
    FontPtr GetStyle()
	{ return fp; } 
    GrPattern GetInk()
	{ return ink; }
    ostream &PrintOn(ostream &s); 
    ostream &DisplayOn(ostream &s); 
    istream &ReadFrom(istream &s); 
    ObjPtr DeepClone();
};

//------ StyledText -----------------------------------------------------------

enum StChangeStyle {    // modes for SetStyle
    eStFont     = BIT(0),       
    eStFace     = BIT(1),       
    eStSize     = BIT(2), 
    eStAddSize  = BIT(3),    
    eStInk      = BIT(4),    
    eStAll      = eStFont + eStFace + eStSize + eStInk // change all
};

struct StyleSpec {
    GrFont font;
    GrFace face;
    short size;
    GrPattern ink;
    bool xor; // apply face change in an XOR or OR fashion

    StyleSpec();
    StyleSpec(Font *fp= gSysFont, GrPattern c= ePatBlack, bool mode= TRUE);
    StyleSpec(GrFont ft, GrFace fc, int sz, GrPattern c= ePatBlack, bool mode= TRUE);
};

//---- class TextRunArray --------------------------------------------------

class TextRunArray: public RunArray {
    class StyledText *st;
public:
    MetaDef(TextRunArray);

    TextRunArray(StyledText *sp = 0, int elements = cCollectionInitCap);

    ObjPtr RunAt(int i, int *start, int *end, int *size ,int *lenat);
    ostream &PrintOn(ostream &s); 
    istream &ReadFrom(istream &s); 
    Object *ReadItem(istream &s); 
};

//---- class StyledText -------------------------------------------------

class StyledText: public GapText {                                                           
friend class StyledTextIter;
    class TextRunArray *styles;
    Style *sp;      // current style and position of next font change

    Style *styleHere; // style used during next text insertion
    bool applyStyleHere; // will we use styleHere ?

    byte escape; // escape character           
    int nextFontChange, start, end, size, lenat;

    byte CharAt(int i)
	{ return GapText::CharAt(i); }

    bool NextFontChange (int at)
	{ if (at == nextFontChange) {
	    sp= (Style*)styles->RunAt(at,&start,&end,&size,&lenat);
	    nextFontChange= at + lenat;
	    return TRUE;
	  }
	  else 
	    return FALSE;
	}
    void Init(FontPtr fd, bool styleHere);
    Style *ApplySpec(Style *ostyle, StyleSpec st, StChangeStyle mode);
    void BuildStylesFromString(Style *initiaStyle, char *buf);
protected:
    void SetFStringVL(char *fmt, va_list va);
public:
    MetaDef(StyledText);

    StyledText();
    StyledText(int size, FontPtr fd= gSysFont);
    StyledText(byte *buf, int len = -1, bool ic = FALSE, FontPtr fd= gSysFont);
    StyledText(class TextRunArray *st, byte *buf, int len = -1, bool ic = FALSE);
    StyledText(FontPtr fd, char* format, ...);
	// to create a text in different faces, specified in a string
	// 'format' can contain a format specifications as strprintf.
	// Extensions are: @B, @I, @O, @S, @P, @U with the meaning to change the 
	//                 face of the text to
	//                 Bold,Italic,Outline,Shadow,Plain,Underline
	// example:
	// char * filename;
	// StyledText ex("File @B%s@B does not exist", filename);
    ~StyledText();
    void InitNew();

    TextRunArray *GetStyles();
    TextRunArray *SetStyles(TextRunArray *st);
    
    //---- editing
    void ReplaceWithStr(byte *str,int len = -1); 
    void Cut(int from,int to);
    void Paste(TextPtr t,int from,int to);
    void Insert(byte c, int from,int to);
    Text *GetScratchText(int size); 
    void Copy(Text* save,int from, int to);
    TextPtr Save(int from, int to);
    
    //---- rendering/mapping
    int TextWidth(int from, int to);
    void DrawText(int from, int to, Rectangle clip);
    void DrawTextJust(int from, int to, int w, Point start, Rectangle clip);
    int PointToPos(int from, int to, int w, Point start, Point pos);
    int CharToPos(int from, int to, int at, int w);
    void JustifiedMap(int from, int to, int w, int stopAt, int posX, int *charPos, int *x);
    
    //---- style manipulation
    void SetStyle(StChangeStyle mode, int from, int to, StyleSpec newStyle);
    Style *GetStyle(int at);
    FontPtr GetFont(int at = 0);
    void SetFont(FontPtr);
	// apply the font to entire text
    void ReplaceStyles(RunArray *styles, int from, int to);
    void CopyStyles(RunArray *styles,int from, int to);
    void ResetCurrentStyle();
    const Style *GetCurrentStyle();
	// 0 == no current style

    //---- escape character handling
    void SetEscapeChar (byte);
    byte GetEscapeChar ();
    virtual int CalcEscape (int at,LineDesc* = 0);
    virtual void DrawEscape (int at,Rectangle clip);
    virtual bool IsEscape (int at);
    bool DoEscape(int at, int ch)
	{ return (bool) (escape && ch == escape && IsEscape(at)); }

    TextIter *MakeIterator(int from=0, int to = cMaxInt);

    //---- activation/passivation
    ostream &PrintOn(ostream &s); 
    istream &ReadFrom(istream &s); 
    istream &ReadFromAsPureText(istream &s, long sizeHint= -1);
};

//---- class StyledTextIter -------------------------------------------------

class StyledTextIter: public GapTextIter {
    int nextFontChange, start, end, size, lenat;
    Style *sp;
public:
    StyledTextIter(Text *s,int from = 0,int to = cMaxInt);
    void Reset(Text *s,int from = 0, int to = cMaxInt);        
    int operator()();
    int operator()(int *width, LineDesc*);// returns width of the character too
    int Token(int *width, LineDesc*); // returns next token with its extent
    int Line(LineDesc* l = 0);        // return end of next line
    byte CharAt(int i)
	{ return ((StyledText*)ct)->CharAt(i); }
    bool NextFontChange ();
    bool DoEscape(int at, int ch)
	{ return ((StyledText*)ct)->DoEscape(at,ch); }
};

#endif StyledText_First
