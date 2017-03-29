#ifndef TextCmd_First
#ifdef __GNUG__
#pragma once
#endif
#define TextCmd_First

#include "StyledText.h"
#include "TextView.h"

class Text;

typedef void (*TextRangeFP)(Text *t, int at, int *s, int *e);

//---- RangeSelector ------------------------------------------------------------

extern void CharacterRange(Text *t, int at, int *start, int *end);
extern void WordRange(Text *t, int at, int *start, int *end);
extern void ParagraphRange(Text *t, int at, int *start, int *end);

class RangeSelector: public Command {
protected:
    TextView *tv;
    SelPoint startp, endp;
    TextRangeFP range;
public:    
    RangeSelector(TextView*, TextRangeFP rf= CharacterRange);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    virtual void DoPress(SelPoint nextp, int start, int end);
    virtual void DoMove(SelPoint nextp, int start, int end);
};

class ExtendRangeSelector: public RangeSelector {
    bool swap;
    bool first;
public:    
    ExtendRangeSelector(TextView*, TextRangeFP rf= CharacterRange); 
    void DoPress(SelPoint nextp, int start, int end);
    void DoMove(SelPoint nextp, int start, int end);
}; 

//---- QuickPasteSelector -----------------------------------------------

class QuickPasteSelector: public RangeSelector {
    int from, to;
public:    
    QuickPasteSelector(TextView*, int, int);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

//---- CopyDragPasteSelector -------------------------------------------

class CopyDragPasteSelector: public Command {
    int from, to;
    TextView *tv;
public:    
    CopyDragPasteSelector(TextView*, int, int);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool);
};

//---- TextCommands ----------------------------------------------------------

class TextCommand: public Command {
protected:
    TextView *tv;
    Text *oldText;
    int oldStart, oldEnd;
public:  
    MetaDef(TextCommand);      
    TextCommand(TextView*, int cmdNo, char *cmdName, bool saveOldtext= TRUE);
    ~TextCommand();
    void RestoreSelection();    
    void RedoIt();
};

class CutCopyCommand: public TextCommand {
public:    
    MetaDef(CutCopyCommand);      
    CutCopyCommand(TextView*, int cmdNo, char *cmdName= 0);
    void DoIt();
    void UndoIt();
};

class PasteCommand: public TextCommand {
protected:
    Text *pastetext;
    int newStart,newEnd;   
public: 
    MetaDef(PasteCommand);      
    PasteCommand(TextView*, Text*, int cmdNo= cPASTE, char *cmdName= 0);
    ~PasteCommand();
    void DoIt();
    void UndoIt();
};

class TypeingCommand: public TextCommand {
protected:
    Text *backspaceBuf;    // to store text backspaced over 
    Text *newText;
    int newStart;
public:
    bool completed;
    
    MetaDef(TypeingCommand);
    TypeingCommand(TextView *t, int cmdNo, char *cmdName);
    ~TypeingCommand();
    void UndoIt();
    void RedoIt();
    void Done(Command *newCmd);
    void AddChar(int n=1);
    void DelChar();
};

//---- class ChangeStyle -------------------------------

class ChangeStyleCommand: public TextCommand {
    RunArray *newStyles, *oldStyles;
    TextView *tvp;
    StChangeStyle mode;
    StyleSpec style;    
public:    
    MetaDef(ChangeStyleCommand);
    ChangeStyleCommand(TextView *t,int cmdNo,char *cmdName, StChangeStyle mode, 
		       StyleSpec newStyle);
    void DoIt();
    void UndoIt();
    void RedoIt();
};

#endif TextCmd_First     
