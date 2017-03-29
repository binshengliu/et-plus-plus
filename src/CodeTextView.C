//$CodeTextView, CodeAnalyzer, PrettyPrinter$
 
#include "Port.h"
#include "FixLineTView.h"
#include "CodeTextView.h"
#include "StyledText.h"

static char *match= "lass",
	    *obrackets = "({[\"",
	    *cbrackets = ")}]\"";
 
//---- CodeTextView ------------------------------------------------------------

MetaImpl(CodeTextView, (TB(autoIndent), T(cursorPos), 0));

CodeTextView::CodeTextView(EvtHandler *eh, Rectangle r, Text *t, eTextJust m, eSpacing sp, 
					TextViewFlags fl, Point b, int id)
			: FixedLineTextView(eh, r, t, m, sp, FALSE, fl, b, id) 
{
    SetStopChars("\n\r");
    autoIndent= TRUE;
    SetupStyles(t->GetFont());
    ExitCursorMode();
}   

Command *CodeTextView::DoKeyCommand(int ch, Point lp, Token token)
{
    Command *cmd;

    ExitCursorMode();
    if ((ch == '\r' || ch == '\n') && autoIndent) { // auto indent
	int from, to, i;
	Text *t= GetText();
	GetSelection(&from,&to);
	scratchText->Empty();
	scratchText->Append('\n'); 
	for (i= StartLine(CharToLine(from)); i < t->Size() ; i++) {
	    ch= (*t)[i];
	    if (ch == ' ' || ch == '\t')
		scratchText->Append(ch);
	    else
		break;
	}
	cmd= InsertText(scratchText);
	scratchText->Empty();
	RevealSelection();
	return cmd;
    }
    return FixedLineTextView::DoKeyCommand(ch, lp, token);
}

void CodeTextView::SetFont(Font *fp)
{
    SetupStyles(fp);
    FixedLineTextView::SetFont(fp);
}

void CodeTextView::SetupStyles(Font *fp)
{
    Font *fd1, *fd2, *fd3;
    GrFont fid= fp->Fid();
    int size= fp->Size();
    fd1= new_Font(fid, size, eFacePlain);
    fd2= new_Font(fid, size, eFaceItalic);
    fd3= new_Font(fid, size, eFaceBold);

    commentStyle= new_Style(fd2);
    functionStyle= new_Style(fd3);
    classDeclStyle= new_Style(fd3);
    plainStyle= new_Style(fd1);
}

void CodeTextView::SetAutoIndent(bool b)
{
    autoIndent= b;
}
    
bool CodeTextView::GetAutoIndent()
{
    return autoIndent;
}

void CodeTextView::ExitCursorMode()
{
    cursorPos= gPoint_1;
}

Command *CodeTextView::DoLeftButtonDownCommand(Point p, Token t, int cl)
{
    Point sp= p;
    ExitCursorMode();
    if (!Enabled())
	return gNoChanges;
    if (cl >= 2) {
	int line, cpos;
	char *br;
	Point pp;
	Text *text= GetText();

	p-= GetInnerOrigin();
	PointToPos(p, &pp, &line, &cpos);
	
	if (cpos > 0 && (br= index(obrackets,(*text)[cpos-1]))) {
	    MatchBracketForward(cpos, *br, cbrackets[br-obrackets]);
	    return gNoChanges;
	}
	if (cpos < text->Size() && (br= index(cbrackets,(*text)[cpos]))) {
	    MatchBracketBackward(cpos-1, obrackets[br-cbrackets], *br);
	    return gNoChanges;
	}
    }
    return FixedLineTextView::DoLeftButtonDownCommand(sp, t, cl);
}

Command *CodeTextView::DoMenuCommand(int m)
{
    ExitCursorMode();
    return TextView::DoMenuCommand(m);
}
    
Command *CodeTextView::DoOtherEventCommand(Point p, Token t)
{
    ExitCursorMode();
    return TextView::DoOtherEventCommand(p, t);
}

void CodeTextView::MatchBracketForward(int from, int obracket, int cbracket)
{
    Text *text= GetText();
    int ch, stack= 0;

    for (int i= from; i < text->Size(); i++) {
	ch= (*text)[i];
	if (ch == cbracket) {
	    if (stack-- == 0) 
		break;
	} else if (ch == obracket)
	    stack++;
    }    
    SetSelection(from, i, TRUE);
}

void CodeTextView::MatchBracketBackward(int from, int obracket, int cbracket)
{
    Text *text= GetText();
    int ch, stack= 0;

    for (int i= from; i >= 0; i--) {
	ch= (*text)[i];
	if (ch == obracket) { 
	    if (stack-- == 0) 
		break;
	} else if (ch == cbracket)
	    stack++;
    }
    SetSelection(i+1, from+1, TRUE);
}

Command *CodeTextView::DoCursorKeyCommand(EvtCursorDir cd, Point p, Token t)
{
    switch (cd) {
    case eCrsLeft:
    case eCrsRight:
	ExitCursorMode();
	break;
    default:
	break;
    }
    return TextView::DoCursorKeyCommand(cd, p, t);
}

int CodeTextView::CursorPos(int at, int line, EvtCursorDir d, Point p)
{    
    int charNo;
    Point basePoint;
    
    if (cursorPos == gPoint_1) 
	CharToPos (at,&line,&cursorPos);

    if (d == eCrsDown)
	line= min(nLines-1, line+1);
    else
	line= max(0, line-1);        
    basePoint= LineToPoint(line, TRUE) + Point(cursorPos.x, 0);
    PointToPos(basePoint, &p, &line, &charNo);
    return charNo;
}

PrettyPrinter *CodeTextView::MakePrettyPrinter(Text *t, Style *cs, Style *fs,
							Style *cds, Style *ps)
{
    return new PrettyPrinter(t, cs, fs, cds, ps);
}

void CodeTextView::FormatCode()
{
    if (!text->IsKindOf(StyledText)) 
	return;
    PrettyPrinter *pp= MakePrettyPrinter(text, commentStyle, functionStyle, classDeclStyle, plainStyle);
    pp->Doit();
    SafeDelete(pp);
}

void CodeTextView::SetDefaultStyle()
{
    if (!text->IsKindOf(StyledText)) 
	return;
    StyledText *stext= (StyledText*)text;
    TextRunArray *st= new TextRunArray(stext);
    st->Insert(plainStyle, 0, 0, stext->Size());
    TextRunArray *tmpp= stext->SetStyles(st);
    delete tmpp;
}

//---- CodeAnalyzer ----------------------------------------------------------

CodeAnalyzer::CodeAnalyzer(Text *t)
{
    text= t;
}

void CodeAnalyzer::Doit()
{
    inDefine= escape= inClass= FALSE;
    lastComment= prevCh= braceStack= inString= line= 0;
    c= '\n'; 
    int canBeClass= canBeFunction= -1;
    AutoTextIter next(text, 0, text->Size());
    Start();
    
    while ((c= next()) != cEOT) {
	if (escape)
	    escape=FALSE;
	else
	    switch (c) {
	    case '#':
		if (prevCh == '\n')
		    inDefine= TRUE;
		break;
	    case '\\':
		escape= TRUE;
		break;
	    case '\n':
		inDefine= FALSE;
		line++;
		break;
	    case '*':
		if (prevCh == '/' && inString == 0) {
		    if (canBeFunction == -1 && canBeClass == -1)
			lastComment= next.GetPos(); 
		    FoundComment(&next);
		    prevCh= 0;
		    continue;
		}
		break;
	    case '/':
		if (prevCh == '/' && inString == 0) {
		    if (canBeFunction == -1 && canBeClass == -1)
			lastComment= next.GetPos(); 
		    FoundEndOfLineComment(&next);
		    prevCh= 0;
		    continue;
		}
		break;
	    case ';':
		canBeFunction= canBeClass= -1;
		break;
	    case '{':
		if (canBeFunction != -1) {
		    FoundFunctionOrMethod(canBeFunction, lastComment);
		    canBeFunction= -1;
		}
		if (canBeClass != -1) {
		    FoundClassDecl(canBeClass);
		    canBeClass= -1;
		}
		if (inString == 0)
		    braceStack++;
		break;
	    case '}':
		if (inString == 0)
		    braceStack--;
		break;
	    case '(':
		if (!inDefine && inString == 0) {
		    if (braceStack == 0 && canBeFunction == -1)
			canBeFunction= next.GetPos()-1;
		    braceStack++;
		}
		break;
	    case ')':
		if (!inDefine && inString == 0)
		    braceStack--;
		break;
	    case '\'':
	    case '\"':
		if (inString == 0) {
		    if (c == '\"')
			inString= '\"';
		    else
			inString= '\'';
		} else {
		    if ((inString == '\"' && c == '\"') || (inString == '\'' 
							 && c == '\''))
			inString= 0;
		}
		break;
	    default:
		if (c == 'c' && !inDefine && inString == 0 
		    && !inClass && canBeClass == -1) {
		    if (braceStack == 0)
			if (inClass= IsClassDecl(next.GetPos()))
			    next.SetPos(next.GetPos()+4);
		} else if (inClass && Isinword(c)) {
		    inClass= FALSE;
		    canBeClass= next.GetPos();
		}
		break;
	}
	prevCh= c;
    }
    
    End();
}

void CodeAnalyzer::FoundComment(AutoTextIter *next)
{
    int start= next->GetPos()-2;
    int end, c, prevCh= 0, l= line;
   
    while ((c= (*next)()) != cEOT) {
	if (c == '\n')
	    line++;
	if (c == '/' && prevCh == '*') {
	    end= next->GetPos();
	    Comment(l, start, end);
	    break;
	}
	prevCh= c;
    }
}

void CodeAnalyzer::FoundEndOfLineComment(AutoTextIter *next)
{
    int start= next->GetPos()-2;
    int end, c;

    while ((c= (*next)()) != cEOT) 
	if (c == '\n') {
	    end= next->GetPos()-1;
	    Comment(line, start, end);
	    line++;
	    break;
	}
}

void CodeAnalyzer::FoundFunctionOrMethod(int at, int lastComment)
{
    byte c;
    int pos= at, len= 0;
    while (--pos >= lastComment) {
	c= (*text)[pos];
	if (!Isspace(c) && !index("[]*+-=%&><|:^%()~/",c)) // overloaded operators
	    break;
    }
    while (pos >= lastComment) {
	c= (*text)[pos];
	if (!(Isinword(c) || c == ':' || c == '~' ))
	    break;
	if (pos-1 < 0)
	    break;
	pos--;
	len++;
    }
    
    if (len) {
	char buf[500];
	pos++;
	text->CopyInStr((byte*)buf, sizeof buf, pos, pos+len);
	char *p= index(buf, ':');
	if (!p)
	    Function(line, pos, pos+len, buf, 0);
	else {
	    *p= '\0';
	    char *pp= p+2;
	    Function(line, pos, pos+len, pp, buf);
	}
    }
}

bool CodeAnalyzer::IsClassDecl(int at)
{
    char *p= match;
    int c;
    AutoTextIter next(text, at, text->Size());
    while ((c= next()) != cEOT && *p) {
	if (c != *p++) 
	    return FALSE;
    }
    return TRUE;
}

void CodeAnalyzer::FoundClassDecl(int at)
{
    int start= at-1;
    AutoTextIter next(text, start, text->Size());
    int end, c;

    while ((c= next()) != cEOT) 
	if (!Isinword(c)) 
	    break;
    end= next.GetPos()-1;
    if (start != end) {
	static byte name[1000];
	text->CopyInStr(name, sizeof name, start, end);
	ClassDecl(line, start, end, (char*)name);
    }
}

void CodeAnalyzer::Start()
{
}

void CodeAnalyzer::End()
{
}

void CodeAnalyzer::Comment(int, int, int)
{
}

void CodeAnalyzer::ClassDecl(int, int, int, char *)
{
}
    
void CodeAnalyzer::Function(int, int, int, char *, char *)
{
}

//---- PrettyPrinter ---------------------------------------------------------

PrettyPrinter::PrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds,
					Style *ps) : CodeAnalyzer(t)
{
    stext= Guard(t, StyledText);
    plainStyle= ps;
    commentStyle= cs;
    classDeclStyle= cds;
    functionStyle= fs;
}

void PrettyPrinter::Start()
{
    st= new TextRunArray(stext);
    st->Insert(plainStyle, 0, 0, stext->Size());
}

void PrettyPrinter::End()
{
    TextRunArray *tmpp= stext->SetStyles(st);
    delete tmpp;
}

void PrettyPrinter::Comment(int, int start, int end)
{
    st->Insert(commentStyle, start, end, end-start);
}
 
void PrettyPrinter::ClassDecl(int, int start, int end, char *)
{
    st->Insert(classDeclStyle, start, end, end-start);
}
    
void PrettyPrinter::Function(int, int start, int end, char *, char *)
{
    st->Insert(functionStyle, start, end, end-start);
}

