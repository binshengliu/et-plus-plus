#ifndef TextView_First
#ifdef __GNUG__
#pragma once
#endif
#define TextView_First

#include "StaticTView.h"

const int cMaxBatchedIns = 80; // maximum number of batched inserts

struct SelPoint {
    int ch;         // char number of a selection point
    int line;       // line number of a selection
    Point viewp;    // position in view coordinates
    bool IsEqual(const SelPoint &s2)
	    { return (bool) (ch == s2.ch); }
    
};

void SwapSelPoints(SelPoint &p1, SelPoint &p2);

//---- TextView -------------------------------------------------------------

class TextView: public StaticTextView {
friend class RangeSelector;
friend class ExtendRangeSelector;

    //---- selection    
    SelPoint start,end;            // selection range
    bool inTextSelector;
    bool updateSelection;
    char *stopChars; // these characters wont be batched during DoKeyCommand              // if DoKeyCommand is overriden
    class TypeingCommand *Typeing; // current typeing command object
    class FindDialog *findChange;

protected:
    void NormSelection();   
	// normalize selection, establisch start < end 
    int FormatInsertedText(int startLine, int startCh, int n);
	// returns the number of inserted lines
    class Text *scratchText;
    int DeleteLines(int from, int to);
    void PrivSetSelection(int s, int e, bool redraw= TRUE);
    void PrivSetSelection(SelPoint s, SelPoint e, bool redraw= TRUE);
    void Init();
    bool Writeable()
	{ return (bool) (!TestFlag(eTextViewReadOnly) && Enabled()); }
    virtual int CursorPos(int ch, int line, EvtCursorDir d, Point);

public:
    MetaDef(TextView);

    TextView(EvtHandler *eh, Rectangle extent, Text *t, eTextJust m= eLeft,
	     eSpacing= eOne, bool wrap= TRUE,
	     TextViewFlags= eTextViewDefault, 
	     Point border= gBorder, int id= cIdNone);    
	    // extent.width/height can be set to cFit

    ~TextView();
    void InitNew();

    void Draw(Rectangle);
    void Invert(int from, Point fp, int to, Point tp);
    void HighlightSelection(HighlightState hs);
    virtual void Reformat();

    //---- controller methods 
    Command *DoLeftButtonDownCommand(Point, Token, int);
    Command *DoKeyCommand(int, Point, Token);
    void SetStopChars(char *stopChars);
	// define in a string a list of characters that should not be
	// batched in DoKeyCommand. Has to be used when DoKeyCommand
	// is overridden to intercept carriage returns for instance
    const char *GetStopChars()
	{ return stopChars; }
    Command *InsertText(Text *insert); 
	// has the same effect as a sequence of DoKeyCommands -> undoable
    Command *DoCursorKeyCommand(EvtCursorDir, Point, Token);
    Command *DoOtherEventCommand(Point p, Token t);
    Command *DoIdleCommand();
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);
    void SendDown(int, int, void*);
    void Enable(bool b= TRUE, bool redraw= TRUE);
    GrCursor GetCursor(Point);

    //---- selection ----
    void InvalidateSelection();
    void SetSelection(int s= 0, int e= cMaxInt, bool redraw = TRUE);
    void SetNoSelection(bool redraw = TRUE);
    void GetSelection(int *s, int *e)
	{ *s = start.ch; *e = end.ch; } // if no selection s and e are set to -1
    void SelectionAsString(byte *, int max);
    Text *SelectionAsText();
    Rectangle SelectionRect();
    void RevealSelection();
    bool InTextSelector()
	{ return inTextSelector; }
    virtual void Home();
    virtual void Bottom();
    bool Caret()
	{ return (bool) (start.ch == end.ch); }
    bool AnySelection()
	{ return (bool) (start.ch != -1); }
    virtual void SelectAll();
    virtual void DrawCaret(Point p, int line, HighlightState);
    
    //---- clipboard --------------------
    bool HasSelection();
    void SelectionToClipboard(char*, ostream &os);
    Command *PasteData(char*, istream &s);
    bool CanPaste(char *type);

    //---- editing and screen update 
    virtual void Cut();            
    virtual void DelChar(int n=1);
    virtual bool DeleteRequest(int from, int to);
	// called before deleting the text in the range from, to
	// a return value of FALSE indicates a veto, and the text will not be
	// deleted 
    virtual void Copy(Text *save);
    virtual void Paste(Text *insert);
    Text *SetText(Text *);     // returns old text       
    void SetString(byte *str, int len);    
    void StartFormatting();

    void SetReadOnly(bool);
    bool GetReadOnly();
    
    //---- searching 
    virtual FindDialog *MakeFindDialog();
    virtual bool SelectRegExpr(class RegularExp *rex, bool forward= TRUE);
    
    //----- Command Notification
    void DoneTypeing();  // notify typeing command    
    void TypeingDeleted();
    
    //----- change notification from the text object
    void DoObserve(int, int part, void*, Object *op);
    bool PrintOnWhenObserved(Object *from);

    //---- activation passivation ----------------------------------------------
    istream& ReadFrom(istream &);

    //----- debugging
    void Dump();
};

#endif TextView_First     
