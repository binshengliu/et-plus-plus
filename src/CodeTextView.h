#ifndef CodeTextView_First
#ifdef __GNUG__
#pragma once
#endif
#define CodeTextView_First
 
#include "FixLineTView.h"

//---- CodeTextView --------------------------------------------------------
// a TextView for editing programm text with a fixed line spacing.
// adds: auto indenting
//       highlighting matching brackets
//       (double clicking on a bracket selects the text enclosed by the brackets)

class CodeTextView: public FixedLineTextView {
protected:
    bool autoIndent;
    Point cursorPos;
    class Style *commentStyle;
    class Style *functionStyle;
    class Style *classDeclStyle;
    class Style *plainStyle;

    void MatchBracketForward(int from, int obracket, int cbracket);
    void MatchBracketBackward(int from, int obracket, int cbracket);
    void ExitCursorMode();
    int CursorPos(int ch, int line, EvtCursorDir d, Point);
    void SetupStyles(Font *fp);
    
public:
    MetaDef(CodeTextView);

    CodeTextView(EvtHandler *eh, Rectangle r, 
	     Text *t, eTextJust m= eLeft,
	     eSpacing= eOne, TextViewFlags= (TextViewFlags)eVObjDefault, 
	     Point border = gBorder, int id = -1);    
	    // for efficency reasons do not set contentRect.extent.x to cFit
	    
    //---- formatting of source code
    virtual class PrettyPrinter *MakePrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps);
    void FormatCode();
    void SetDefaultStyle();
    void SetFont(Font *fp);
    
    //---- controller methods 
    Command *DoLeftButtonDownCommand(Point, Token, int); // matching brackets
    Command *DoKeyCommand(int, Point, Token);      // auto indent
    Command *DoMenuCommand(int);
    Command *DoOtherEventCommand(Point p, Token t);
    Command *DoCursorKeyCommand(EvtCursorDir, Point, Token);    
    void SetAutoIndent(bool);
    bool GetAutoIndent();
};

//---- CodeAnalyzer --------------------------------------------------------

class CodeAnalyzer: public Root {
    bool inDefine;
    int lastComment, prevCh, c; 
    int braceStack, inString, escape, inClass, line;
    int canBeClas, canBeFunction;

    void FoundComment(AutoTextIter *next);
    void FoundEndOfLineComment(AutoTextIter *next);
    void FoundFunctionOrMethod(int at, int lastComment);
    void FoundClassDecl(int at);
    bool IsClassDecl(int at);
    
protected:
    Text *text;

    virtual void Start();
    virtual void Comment(int line, int start, int end);
    virtual void ClassDecl(int line, int start, int end, char *name);
    virtual void Function(int line, int start, int end, char *name, char *classname);
    virtual void End();

public:
    CodeAnalyzer(Text *t);
    void Doit();
};

//---- PrettyPrinter --------------------------------------------------------

class PrettyPrinter: public CodeAnalyzer {
    class StyledText *stext;
    class TextRunArray *st;
    class Style *commentStyle;
    class Style *functionStyle;
    class Style *classDeclStyle;
    class Style *plainStyle;
protected:
    void Start();
    void Comment(int line, int start, int end);
    void ClassDecl(int line, int start, int end, char *name);
    void Function(int line, int start, int end, char *name, char *classname);
    void End();
public:
    PrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps);
};

#endif CodeTextView_First  
