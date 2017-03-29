#ifndef StaticTView_First
#ifdef __GNUG__
#pragma once
#endif
#define StaticTView_First

#include "Text.h"
#include "Mark.h"
#include "ObjArray.h"
#include "View.h"

enum eTextJust {                               // text justification 
    eLeft, 
    eRight, 
    eCenter, 
    eJustified  
};

enum eSpacing  {                                // line spacing
    eOne        = 2, 
    eOneHalf    = 3, 
    eTwo        = 4 
};

enum TextViewFlags {
    eTextViewNone       =   0,
    eTextViewReadOnly   =   BIT(eViewLast+1),   // disable text modifications
    eTextViewVisMode    =   BIT(eViewLast+2),   // show new line characters
    eTextFormPreempt    =   BIT(eViewLast+3),   // is formating process preemptive
    eTextFixedOrigin    =   BIT(eViewLast+4),   // if contentRect.width is set to
						// cFit and the justification is eRight or
						// eCenter, should the origin of the text 
						// remain fixed
    eTextViewNoBatch    =   BIT(eViewLast+5),   // do not batch characters in DoKeyCommand
    eTextFixedSpacing   =   BIT(eViewLast+6),   // use fixed line spaceing
    eTextNoFind         =   BIT(eViewLast+7),   // do not add "find" menu
    eTextViewLast       =   eViewLast+7,     
    eTextViewDefault    =   eVObjEnabled
};

extern Rectangle gFitRect;

const int cFit = -1;

//---- line mark ------------------------------------------------------------

extern int LineMark_lineChanged;    // height or baseheight of a line has changed 

struct LineMark: public Mark {
    LineDesc ld;

    MetaDef(LineMark);

    LineMark(LineDesc ldesc, int pos= 0, int len= 0, eMarkState s= eStateNone);
    ~LineMark();
    void ChangeMark (int pos, int len, LineDesc ldesc, eMarkState s = eStateNone)
	{ if (!LineMark_lineChanged) 
		LineMark_lineChanged= !ld.IsEqual(ldesc);
	  Mark::ChangeMark(pos,len,s); 
	  ld = ldesc;
	}
    ostream& DisplayOn (ostream&s);

    //---- memory allocation
    void *operator new (size_t);
    void operator delete(void *vp);
};
 
//---- global functions ----------------------------------------------------

int TextViewLineHeight(FontPtr fd = gSysFont, eSpacing sp = eOne); 
    // return the height of a textline

inline int TextViewlh(eSpacing sp, int h)
{
    return sp == eOne ? h: ((int)sp)*h/2;
}

//---- TextView -------------------------------------------------------------

class StaticTextView: public View {
protected:
    Text *text;                 // shown text
    class TextFormatter *formatter; // formatter
    eTextJust just;             // justification
    eSpacing spacing;           // spacing between two lines
    bool wrap;                  // break line at view boundary
    bool drawViewBorder;        
    bool vertExtend,horExtend;  // determine vertExtend, horExtend depending
				// on text
    Point border;               // border around the text

    int nLines;                 // number of lines
    ObjArray *lines;            // array of line marks, grows dynamically
    MarkList *marks;            // notice: this set of marks is managed by
				// members of the class StaticTextView not by
				// an instance of Text   
    class TextIter *nextc;      // iterator of currently loaded text
    LineDesc fixedSpacing;      // line spacing to use with fixed line spaceing

public:
    MetaDef(StaticTextView);
    int FirstCharPos (int start,int end); 
	// position of first char on a line
	
    //---- called from formatter
    bool MarkLine(int line, int start, int end, LineDesc *ld); 
	// add a line mark and return if the line changed
    void MarkLineAsChanged(int line);
    bool SuspendFormatting();
    virtual void StartFormatting();
    
    void Init(Rectangle, Text*, eTextJust, eSpacing, bool wrap, TextViewFlags,Point);
    bool IsJustified(int endOfLine);
    virtual int OutOfRangeLineMetric(bool lineHeight);   


    StaticTextView(EvtHandler *eh, Rectangle r, Text *t, eTextJust m= eLeft,
	     eSpacing= eOne, bool wrap= TRUE,
	     TextViewFlags= eTextViewDefault, 
	     Point border= gBorder, int id= cIdNone);    
	    // contentRect.width/height can be set to cFit, see above

    ~StaticTextView();

    void Draw(Rectangle);
    virtual void DrawLine (Point p,int line,Rectangle lineRect, Rectangle clip);
    virtual void Reformat();                  // reformat and redisplay whole text
    virtual void ChangedAt(int line, int ch = 0, bool redrawAll = FALSE, int minUpto = 0);
	// format text starting at 'line' at least up to line 'minupto'
    virtual void InvalidateRange(int from, int to);
    virtual void InvalidateRange(int from, Point fp, int to, Point tp);

    virtual Text *SetText(Text*);     // returns old text
    void SetFormatter(TextFormatter *f);       
    virtual void SetString(byte *str, int len= -1);
    Text *GetText()
	{ return text; }

    //---- acessing
    void SetSpacing (eSpacing);
    eSpacing GetSpacing()
	{ return spacing; }
    virtual void SetFont(FontPtr fd);
    FontPtr GetFont()
	{ return text->GetFont(); }
    eTextJust GetJust() 
	{ return just; }
    void SetJust(eTextJust);
    void SetVisibleMode(bool);
    bool GetVisibleMode();
    void SetNoBatch(bool);
    bool GetNoBatch();
    void SetWordWrap(bool m);
    bool GetWordWrap();
    bool Empty()
	{ return (bool) (text->Size() == 0); }
    int NumberOfLines();

    //---- mapping
    virtual void PointToPos(Point p,Point *viewPos,int *lineNo,int *CharNo); // viewPos.y ???
    virtual void CharToPos (int charNo,int *lineNo,Point *viewPos, bool relative = TRUE);
    virtual Point LineToPoint (int line, bool basePoint = FALSE, bool relative = TRUE);
    virtual int PointToLine (Point p);
    virtual int CharToLine(int ch);   // return line number of character

    //---- line structure access
    LineMark *MarkAtLine(int i)
	{ return (LineMark*)(*lines)[i]; }
    int StartLine(int l)
	{ return MarkAtLine(l)->pos; }
    int EndLine(int l)
	{ return MarkAtLine(l)->pos + MarkAtLine(l)->len; }
    int LineHeight(int l);
    int BaseHeight(int l);
    
    //---- size managment
    virtual void Fit();     
	// adapt view size to the extension of text if wrap is set to TRUE only 
	// the height is changed
    void SetExtent(Point p); // (reformats text)
    Metric GetMinSize();
    int Base();
    Point GetInnerOrigin()  // 
	{ return contentRect.origin+border; }
    Point GetInnerExtent()  
	{ return contentRect.extent-2*border; }
    void SetBorder(Point newBorder)
	{ border= newBorder; }
    Point GetBorder()
	{ return border; }

    //----- client marks    
    void AddMark(Mark *);
    Mark *RemoveMark(Mark *);
    Iterator *GetMarkIter(); 
    MarkList *GetMarkList();    

    //----- debugging
    virtual void Dump();
    void InspectorId(char *buf, int sz);
    void Parts(Collection*);

    //----- generic methods
    char *AsString();
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &s);
};

#endif StaticTView_First     
