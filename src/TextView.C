//$TextView$
#include "TextView.h"

#include "TextCmd.h"
#include "CmdNo.h"
#include "String.h"
#include "View.h"
#include "BlankWin.h"
#include "Menu.h"
#include "Error.h"
#include "ChangeDialog.h"
#include "Document.h"
#include "RegularExp.h"
#include "ClipBoard.h"
#include "TextFormatter.h"
#include "ObjectTable.h"

static u_short CaretBits[]= {
#   include "images/Caret.image"
};  

const int cCaretHt = 4,
	  cCaretWd = 7;

static Bitmap *CaretImage;

void SwapSelPoints(SelPoint &p1, SelPoint &p2)
{
    SelPoint tmp;

    tmp= p1; p1= p2; p2= tmp;
}

//----- TextView Methods --------------------------------------------------

MetaImpl(TextView, (TB(updateSelection), TB(inTextSelector), 
    TP(stopChars), TP(Typeing), TP(findChange), 
    TP(scratchText), T(start.ch), T(start.line), T(start.viewp),
    T(end.ch), T(end.line), T(end.viewp), 0));

TextView::TextView(EvtHandler *eh, Rectangle r, Text *t, eTextJust m, eSpacing sp, 
		       bool doWrap, TextViewFlags fl, Point b, int id)
			    : StaticTextView(eh, r, t, m, sp, doWrap, fl, b, id) 
{
    Init();
}

void TextView::InitNew()
{
    View::InitNew();
    inTextSelector= FALSE;
}  

void TextView::Init()
{
    PrivSetSelection(0, 0, FALSE);
    Typeing= 0;
    stopChars= 0;
    updateSelection= TRUE;
    text->AddObserver(this);
    scratchText= text->GetScratchText(cMaxBatchedIns);
    findChange= 0;
}

TextView::~TextView()
{
    if (text)
	text->RemoveObserver(this);
    SafeDelete(stopChars);
    if (findChange)
	findChange->Close();
    SafeDelete(findChange);
    SafeDelete(scratchText);
}

void TextView::InvalidateSelection()
{
    if (AnySelection())
	InvalidateRange(start.line, start.viewp, end.line, end.viewp);
}

void TextView::Draw(Rectangle r)
{
    if (GrHasColor() && AnySelection())
	Invert(start.line, start.viewp, end.line, end.viewp);
    StaticTextView::Draw(r);
    DrawCaret(start.viewp, start.line, On);
    if (!GrHasColor() && AnySelection())
	Invert(start.line, start.viewp, end.line, end.viewp);
}

static Ink *caretColor;
    
void TextView::DrawCaret(Point p, int line, HighlightState)
{
    if (! inTextSelector && AnySelection() && Caret()) {
	int bh= BaseHeight(line);
	p+= GetInnerOrigin();

	if (caretColor == 0) {
	    if (GrHasColor())
		caretColor= new RGBColor(255, 0, 0);
	    else
		caretColor= ePatXor;
	}
    
	if (LineHeight(line) - bh >= cCaretHt) {
	    Rectangle r(p.x-cCaretHt, p.y+bh, cCaretWd, cCaretHt);
	    if (CaretImage == 0)
		ObjectTable::AddRoot(
		    CaretImage= new Bitmap(Point(cCaretWd, cCaretHt), CaretBits));
	    GrPaintBitMap(r, CaretImage, caretColor);
	}
	p.x--;
	GrPaintLine(caretColor, 1, eDefaultCap, Point(p.x, p.y), Point(p.x, p.y+bh));
    }
}

//---- highlight the characters in the given range -----------------------

void TextView::Invert(int from, Point fp, int to, Point tp)
{
    if (AnySelection() && !Caret()) {
	Rectangle r;
	
	if ((from == to && fp.x > tp.x) || from > to) { // normalize range
	    swap(from, to);
	    Swap(fp, tp);   
	}
    
	if (from == to) {
	    r= Rectangle(LineToPoint(from)+Point (fp.x,0),
			   Point(tp.x-fp.x, LineHeight(from)));
	    GrPaintRect(r+GetInnerOrigin(), gHighlightColor);
	    return;
	}
	r= Rectangle(LineToPoint(from)+ Point(fp.x,0),
		      Point(GetInnerExtent().x-fp.x, LineHeight(from)));
	GrPaintRect(r+GetInnerOrigin(), gHighlightColor);
	r= Rectangle(LineToPoint(from + 1), Point(GetInnerExtent().x,
		      LineToPoint(to).y - LineToPoint(from + 1).y));
	GrPaintRect(r+GetInnerOrigin(), gHighlightColor);
	r= Rectangle(LineToPoint(to), Point(tp.x, LineHeight(to)));
	GrPaintRect(r+GetInnerOrigin(), gHighlightColor);
    }
}

void TextView::Reformat()
{
    StaticTextView::Reformat();
    if (AnySelection())
	SetSelection(start.ch, end.ch, FALSE);
}

void TextView::SetNoSelection(bool redraw)
{
    if (AnySelection()) {
	InvalidateSelection();
	start.ch= end.ch= -1;
	if (redraw)
	    UpdateEvent();
    }
}

void TextView::PrivSetSelection(SelPoint s, SelPoint e, bool redraw)
{
    InvalidateSelection();          // invalidate old selection
    start= s;
    end= e;
    InvalidateSelection();          // invalidate new selection
    if (redraw)
	UpdateEvent();
}

void TextView::PrivSetSelection(int s, int e, bool redraw)
{
    SelPoint sp, ep;
    int size= text->Size();
    
    sp.ch= range(0, size, s);
    ep.ch= range(0, size, e);
    CharToPos(sp.ch, &sp.line, &sp.viewp);
    if (sp.ch != ep.ch)
	CharToPos(ep.ch, &ep.line, &ep.viewp);
    else
	ep= sp;
    PrivSetSelection(sp, ep, redraw);
}

void TextView::SetSelection(int s, int e, bool redraw)
{
    DoneTypeing();
    PrivSetSelection(s, e, redraw);
}

void TextView::SelectionAsString(byte *buf, int max)
{
    GetText()->CopyInStr(buf, max, start.ch, end.ch);
}

Text *TextView::SelectionAsText()
{
    return text->Save(start.ch, end.ch);
}

void TextView::RevealSelection()
{
    if (AnySelection()) {
	Rectangle r= SelectionRect().Expand(8);
	RevealRect(r, r.extent);
    }
}
     
Rectangle TextView::SelectionRect()
{
    if (AnySelection()) {
	Point p(end.viewp.x+1, end.viewp.y + LineHeight(end.line));
	Rectangle r= NormRect(start.viewp, p);
	r.origin+= GetInnerOrigin();
	return r;
    }
    return gRect0;
}

Command *TextView::DoLeftButtonDownCommand(Point lp, Token t, int clicks)
{
    SelPoint oldStart, oldEnd;
    TextRangeFP rf= CharacterRange;
	
    oldStart= start;
    oldEnd= end;
    formatter->ResumeFormat();
    if (! Enabled())
	return View::DoLeftButtonDownCommand(lp, t, clicks);
    
    Control(GetId(), cPartWantKbdFocus, this);
    
    if (clicks >= 3)
	rf= ParagraphRange;
    else if (clicks >= 2) 
	rf= WordRange;

    if (t.Flags == eFlgShiftKey) 
	return new ExtendRangeSelector(this, rf);  
    
    if (t.Flags == eFlgCntlKey && clicks == 1) 
	return new CopyDragPasteSelector(this, oldStart.ch, oldEnd.ch); 

    if (t.Flags == (eFlgShiftKey|eFlgCntlKey) && clicks == 1)
	return new QuickPasteSelector(this, oldStart.ch, oldEnd.ch);
    
    if (t.Flags == 0)
	return new RangeSelector(this, rf);
    return View::DoLeftButtonDownCommand(lp, t, clicks);
}

void TextView::SetStopChars(char *stops)
{
    strreplace(&stopChars, stops);
}

Command *TextView::DoKeyCommand(int ch, Point, Token token)
{
    int nDelete= 0;
    Token t;
    bool newCmd, delSelection;       // delete the selection only

    if (!Writeable())
	return gNoChanges;
    if (token.Code == 'd' + 128 && gDebug) {
	Dump();
	return gNoChanges;
    }

    GrSetCursor(eCrsNone);
    //---- map \r to to \n ???
    if (ch == '\r')
	ch= '\n';

    newCmd= (Typeing == 0 || Typeing->completed); // start new typing sequence?
 
    if (newCmd) {
	if (Typeing)
	    Typeing->SetFlag(eCmdDoDelete);
	Typeing= new TypeingCommand(this, cTYPEING, "typing");
    }

    delSelection= (newCmd && !Caret());
    if (ch != gBackspace || delSelection) {     
	for (;;) {
	    if (ch != gBackspace) {
		Typeing->AddChar();
		scratchText->Append(ch);
	    }
	    if (scratchText->Size() == cMaxBatchedIns)
		break;
	    t= gWindow->ReadEvent(0);
	    if (t.IsAscii() && (byte)t.Code != gBackspace 
		    // do not batch stopChars
		    && !(stopChars && index(stopChars, (byte)t.Code)) 
			&& ! TestFlag(eTextViewNoBatch)) {
		ch= (byte) t.Code;
	    } else {
		gWindow->PushBackEvent(t);
		break;
	    } 
	}
	Paste(scratchText);
	scratchText->Empty();
    } else {
	for (;;) {
	    Typeing->DelChar();
	    nDelete++;
	    t= gWindow->ReadEvent(0);
	    if ((byte) t.Code != gBackspace) {
		gWindow->PushBackEvent(t);
		break;
	    } 
	} 
	DelChar(nDelete);
    }
    RevealSelection();
    return Typeing;    
}

Command *TextView::DoCursorKeyCommand(EvtCursorDir cd, Point p, Token)
{
    int charNo= 0;

    if (!Enabled())
	return gNoChanges;
    switch (cd) {
    case eCrsLeft:
	charNo= start.ch-1;
	break;
    case eCrsRight:
	charNo= end.ch+1;
	break;
    case eCrsUp:
	formatter->ResumeFormat();
	charNo= CursorPos(start.ch, start.line, cd, p);
	break;
    case eCrsDown:
	formatter->ResumeFormat();
	charNo= CursorPos(end.ch, end.line, cd, p);
	break;
    }
    SetSelection(charNo, charNo);
    RevealSelection();
    return gNoChanges;
}

int TextView::CursorPos(int at, int line, EvtCursorDir d, Point p)
{
    int charNo;
    Point basePoint,screenPoint;

    CharToPos (at, &line, &screenPoint);
    if (d == eCrsDown)
	line= min(nLines-1, line+1);
    else
	line= max(0, line-1);        
    basePoint= LineToPoint(line, TRUE) + Point(screenPoint.x, 0);
    PointToPos(basePoint, &p, &line, &charNo);
    return charNo;
}

Command *TextView::DoOtherEventCommand(Point, Token t)
{
    if (t.Code == eEvtLocMove && TestFlag(eTextFormPreempt))
	formatter->ResumeFormat();
    return gNoChanges;
}

GrCursor TextView::GetCursor(Point)
{
    return eCrsIBeam;
}

Command *TextView::DoIdleCommand()
{
    if (TestFlag(eTextFormPreempt))
	formatter->ResumeFormat(TRUE);
    return View::DoIdleCommand();
}

void TextView::DoneTypeing()
{
    if (Typeing)
	Typeing->completed= TRUE;
    text->ResetCurrentStyle();
    formatter->ResumeFormat();
}

void TextView::TypeingDeleted()
{
    Typeing= 0;  
}

bool TextView::DeleteRequest(int,int)
{
    return TRUE;
}

void TextView::Cut()
{
    if (!Writeable())
	return; 
    if (DeleteRequest(start.ch, end.ch) && AnySelection()) {
	updateSelection= FALSE;
	text->Cut(start.ch, end.ch);
	updateSelection= TRUE;
	PrivSetSelection(start.ch, start.ch, FALSE);
    }
}

void TextView::Copy(Text *t)
{
    text->Copy(t, start.ch, end.ch);
}

void TextView::Paste(Text *t)
{ 
    if (!Writeable())
	return;
    if ((!Caret() && !DeleteRequest(start.ch, end.ch)) || !AnySelection())
	return;
    // minor hack: if text is appended on the last line, 
    // fake last line to reach a mark 
    if (wrap && start.line == nLines-1) 
	MarkAtLine(start.line)->len= cMaxInt;
    updateSelection= FALSE;
    text->Paste(t, start.ch, end.ch);
    updateSelection= TRUE;
    PrivSetSelection(start.ch+t->Size(), start.ch+t->Size(), FALSE);       
}

Command *TextView::InsertText(Text *t)
{
    if (!Writeable())
	return gNoChanges;

    bool newCmd;
    newCmd= (Typeing == 0 || Typeing->completed); // start new typing sequence?

    if (newCmd) {
	if (Typeing)
	    Typeing->SetFlag(eCmdDoDelete);
	Typeing= new TypeingCommand(this, cTYPEING, "typing");
    }

    Typeing->AddChar(t->Size());
    Paste(t);
    return Typeing;
}

void TextView::DelChar (int n)
{
    int newStart= max(0, start.ch-n);
    int changedLine= CharToLine(newStart);

    if (newStart == start.ch)
	return;
    if (newStart == 0 && start.ch == 0)
	return;
    if (DeleteRequest(newStart, start.ch)) {
	updateSelection= FALSE;   
	text->Cut(newStart, start.ch);
	updateSelection= TRUE;
	PrivSetSelection(newStart, newStart, FALSE);       
    }
}

Text *TextView::SetText(Text *t)
{
    Text *old= text;
    old->RemoveObserver(this);
    bool anySel= AnySelection();
    PrivSetSelection(0, 0, FALSE); // reset old selection
    text= t;    // should remove possible reference from a command object ???
    SafeDelete(nextc);
    nextc= text->MakeIterator();
    SafeDelete(scratchText);
    scratchText= text->GetScratchText(cMaxBatchedIns);
    text->AddObserver(this);
    Reformat();  
    Send(GetId(), cPartReplacedText, 0);
    SetSelection(0, 0, FALSE); // set new selection
    if (!anySel)
	SetNoSelection(FALSE);  
    Typeing= 0;
    return old;
}       

void TextView::SetString(byte *str, int len)
{
    bool anySel= AnySelection();
    PrivSetSelection(0, 0, FALSE); 
    text->ReplaceWithStr(str, len);
    Reformat();
    Send(GetId(), cPartReplacedText, 0);
    DoneTypeing();
    PrivSetSelection(0, 0, FALSE); 
    if (!anySel)
	SetNoSelection(FALSE);  
    Typeing= 0;
}

void TextView::SetReadOnly(bool m)
{
    SetFlag(eTextViewReadOnly, m);
}

bool TextView::GetReadOnly()
{ 
    return TestFlag(eTextViewReadOnly); 
}

void TextView::DoObserve(int, int part, void *what, Object *op)
{
    int d= 0, d1= 0;
    
    if (op == text && part != cPartSenderDied) {
	TextChanges *tc= (TextChanges*)what;
	int fl= CharToLine(tc->from), tl; // lines corresponding from, to
	if (tc->from == tc->to)
	    tl= fl;
	else 
	    tl= CharToLine(tc->to);

	switch (part) {
	case eTextChangedRange:
	    ChangedAt(fl, tc->from, TRUE, tl);
	    if (updateSelection && AnySelection())
		PrivSetSelection(tc->from, tc->to, TRUE);
	    break;

	case eTextDeleted:
	    marks->Cut(tc->from, tc->to - tc->from);
	    d = DeleteLines (fl, tl);
	    ChangedAt(fl, tc->from, d > 0);
	    if (updateSelection && AnySelection())
		PrivSetSelection(tc->from, tc->from, TRUE);
	    break;

	case eTextReplaced:
	    if (tc->from != tc->to) {
		marks->Cut(tc->from, tc->to - tc->from);
		d1= DeleteLines(fl, tl);
	    }
	    marks->Paste(tc->from, tc->size); 
	    d= FormatInsertedText(fl, tc->from, tc->size);
	    ChangedAt(fl+d, tc->from, d > 0 || d1 > 0);
	    if (updateSelection && AnySelection()) {
		int p= tc->from+tc->size;
		PrivSetSelection(p, p, TRUE);   
	    }    
	    break;
	}
	if (TestFlag(eVObjLayoutCntl)) 
	    Control(GetId(), cPartChangedText, this);
	Send(GetId(), cPartChangedText, this);    // notify observers of the textview   
    }
}

bool TextView::PrintOnWhenObserved(Object *from)
{
    return GetText() != from;
}

istream& TextView::ReadFrom(istream &s)
{
    StaticTextView::ReadFrom(s);
    GetText()->AddObserver(this);
    scratchText= text->GetScratchText(cMaxBatchedIns);
    return s;
}

void TextView::NormSelection()
{
    if (start.ch > end.ch) 
	SwapSelPoints(start, end);
}

//---- erichsneusterhack

static ObjArray *saved;
MarkList *savedMarks;
static bool doSwap= FALSE;
static int swapAt;

void TextView::StartFormatting()
{
    if (doSwap) {
	// temporary replacement of the array with the line marks
	saved= lines;
	savedMarks= marks;
	marks= new MarkList;
	lines= new ObjArray(12, swapAt);
    }
}

int TextView::FormatInsertedText(int at, int startCh, int n)
{
    if (!formatter->IsKindOf(SimpleFormatter)) // ugly optimization for Simpleformatter
	return 0;

    int end, upto, d;
    LineMark *m;
	
    formatter->Preemptive(FALSE);
    doSwap= TRUE;
    swapAt= at;
    upto= startCh + n;
    end= formatter->DoIt(at, upto);
    d= max(0, end - at);
    swap((Object**)&saved, (Object**)&lines);
    swap((Object**)&savedMarks, (Object**)&marks);
    if (d > 0) {
	for (int i= nLines-1; i > at; i--) { // make room for new lines
	    m= MarkAtLine(i);
	    MarkLine(i+d, m->pos, m->pos+m->len, &m->ld);
	} 
	for (i= at; i <= at + d; i++) {  // insert the new formatted lines
	    m= (LineMark*)(*saved)[i];
	    MarkLine(i, m->pos, m->pos+m->len, &m->ld);
	}
	MarkLineAsChanged(i-1);           // mark the last line as changed
    }
    nLines+= d; 
    savedMarks->FreeAll();
    delete savedMarks;
    delete saved; 
    InvalidateRange(at, at+d);
    formatter->Preemptive(TRUE);
    doSwap= FALSE;
    return d;
}

int TextView::DeleteLines(int from, int to)
{
    int d= to - from;
    if (d > 0) {
	for (int i= from + 1; i < nLines-d; i++) {
	    LineMark *m= MarkAtLine(i+d);
	    MarkLine(i, m->pos, m->pos+m->len, &m->ld);
	} 
	nLines-= d;
	MarkLineAsChanged(from);
    }
    return d;
}
 
void TextView::Dump()
{
    StaticTextView::Dump();
    cerr << "Selection: start = " << start.ch << "," << start.line;
    cerr << " end = " << end.ch << "," << end.line NL;
    for (int i= start.ch; i < end.ch; i++)
	cerr.put((*text)[i]);
    cerr NL;
}

//---- menu related commands -----------------------------------------------

void TextView::DoCreateMenu(Menu *mp)
{
    View::DoCreateMenu(mp);
    if (!TestFlag(eTextNoFind)) {
	char *fs= GetReadOnly() ? "find..." : "find/change...";
	mp->InsertItemsAfter(cLASTEDIT, "select all",   cSELECTALL,
					fs,             cFIND,
					0);
    }
}

bool TextView::HasSelection()
{
    return AnySelection() && !Caret();
}

void TextView::SelectionToClipboard(char *type, ostream &os)
{
    Text *t= SelectionAsText();
    if (t) {
	if (strcmp(type, cDocTypeET) == 0)
	    os << t SP;
	else if (strcmp(type, cDocTypeAscii) == 0)
	    t->PrintOnAsPureText(os);
	delete t;
    }
}

Command *TextView::PasteData(char *type, istream &s)
{
    if (strcmp(type, cDocTypeET) == 0) {
	Object *op= 0;
	s >> op;
	if (op && op->IsKindOf(Text))
	    return new PasteCommand(this, (Text*)op);
    } else if (strcmp(type, cDocTypeAscii) == 0)
	return new PasteCommand(this, new GapText((byte*)s.bufptr()));
    return gNoChanges;
}

bool TextView::CanPaste(char *type)
{
    return strismember(type, cDocTypeET, cDocTypeAscii, 0);
}

void TextView::DoSetupMenu(Menu *mp)
{
    View::DoSetupMenu(mp);
    if (!GetReadOnly()) {
	if (!Caret())
	    mp->EnableItem(cCUT);
	if (AnySelection())
	    mp->EnableItem(cPASTE);
    }
    if (AnySelection())
	mp->EnableItems(cSELECTALL, cFIND, 0);
}

Command *TextView::DoMenuCommand(int cmd)
{
    switch (cmd) {
    case cCUT:
	gClipBoard->SetType((char*) cDocTypeET);
	View::DoMenuCommand(cmd);
	return new CutCopyCommand(this, cCUT);
    case cCOPY:
	gClipBoard->SetType((char*) cDocTypeET);
	View::DoMenuCommand(cmd);
	return new CutCopyCommand(this, cCOPY);
    case cSELECTALL:
	SelectAll();
	break;
    case cFIND:
	if (findChange == 0) {
	    findChange= MakeFindDialog();
	    Document *dp= GetDocument();
	    if (dp)
		GetDocument()->AddWindow(findChange->GetWindow());
	}
	findChange->ShowOnWindow(GetWindow());
	return gNoChanges;
    default:
	return View::DoMenuCommand(cmd);
    }
    return gNoChanges;
}

void TextView::SendDown(int id, int part, void *val)
{
    switch (part) {
    case cPartFocusChanged:
	bool dontRedraw= ((bool) val);
	if (id == cIdStartKbdFocus)   
	    SetSelection(0, cMaxInt, !dontRedraw);
	else 
	    SetNoSelection(!dontRedraw);
	break;
    case cPartHasSelection:  
	*(bool*)val= HasSelection();
	break;
    default:
	StaticTextView::SendDown(id, part, val);
    }
}

FindDialog *TextView::MakeFindDialog()
{
    if (GetReadOnly())
	return new FindDialog("Find", this);
    return new ChangeDialog("Find/Change", this);
}

bool TextView::SelectRegExpr(RegularExp *rex, bool forward)
{
    int selStart, selEnd, pos, matched;

    GetSelection(&selStart, &selEnd);
    if (forward)
	pos= GetText()->Search(rex, &matched, selEnd);
    else
	pos= GetText()->Search(rex, &matched, max(0,selStart-1), cMaxInt, FALSE);

    if (pos != -1) {
	SetSelection(pos, pos+matched);
	return TRUE;
    }
    return FALSE;
}

void TextView::Home()
{
    SetSelection(0, 0);
}

void TextView::Bottom()
{
    SetSelection(cMaxInt, cMaxInt);
}

void TextView::SelectAll()
{
    SetSelection(0, text->Size());
}

void TextView::Enable(bool b, bool redraw)
{
    StaticTextView::Enable(b, redraw);
    if (! Enabled())
	SetNoSelection(TRUE);
} 
