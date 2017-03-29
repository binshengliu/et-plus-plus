//$ShellTextView,PttyInputHandler,ShellZombieHandler$

#include <stdio.h>

#include "Port.h"
#include "Iterator.h"
#include "ShellTView.h"
#include "StyledText.h"
#include "CheapText.h"
#include "System.h"
#include "String.h"
#include "Alert_e.h"
#include "Menu.h"
#include "CmdNo.h"
#include "BlankWin.h"
#include "WindowSystem.h"

char *gDefaultArgv[] = { "/bin/csh", 0 };

const int cPttyBufSize      = 8192,
	  cHighWaterMark    = 12000,
	  cLowWaterMark     = 8000,
	  cESC              = 27;

//---- output cursor (reversed caret) --------------------------------------

static u_short revCaretBits[]= {
#   include "images/revCaret.image"
};  
    
const int crevCaretHeight = 4,
	  crevCaretWidth  = 7;

static Bitmap *revCaretImage;

//---- class PttyInputHandler --------------------------------------------------

class PttyInputHandler : public SysEvtHandler {
    FILE *fp;
    ShellTextView *tv;
public:
    MetaDef(PttyInputHandler);
    PttyInputHandler(FILE *f, ShellTextView *t) : SysEvtHandler(fileno(f))
	{ fp= f; tv= t; }
    void Notify(SysEventCodes, int)
	{ tv->Received(); }
};

MetaImpl(PttyInputHandler, (TP(tv), 0));

//---- ShellZombieHandler ------------------------------------------------------

class ShellZombieHandler : public SysEvtHandler {
    ShellTextView *sh;
public:
    MetaDef(ShellZombieHandler);
    ShellZombieHandler(int pid, ShellTextView *t) : SysEvtHandler(pid)
	{ sh= t;}
    void Notify(SysEventCodes, int)
	{ sh->SlaveDied(); }
};

MetaImpl(ShellZombieHandler, (TP(sh), 0));

//---- class ShellTextView -----------------------------------------------------
    
Rectangle gShellTextViewRect(Point(1000,cFit));
 
MetaImpl(ShellTextView, (TP(outputCursor), TP(inputCursor), 
			   TP(zombieHandler), TP(pttyinp), TB(pendingCR),
			   TB(doReveal), TP(noEchoBuffer), T(lastSubmit),
			   T(state), TP(pttycstr), I_CSS(argv), 0));
			   
ShellTextView::ShellTextView(EvtHandler *eh, Rectangle r, Text *t, 
		char *name, char **args, bool cons, eTextJust m, eSpacing sp, 
					    TextViewFlags fl, Point b, int id)
		: FixedLineTextView(eh, r, t, m, sp, TRUE, fl, b, id) 
{
    Init(TRUE, name, args, cons);
}   

void ShellTextView::Init(bool reveal, char *name, char **args, bool cons)
{
    doReveal= reveal;
    outputCursor= new Mark(0, 0, eStateNone, (eMarkFlags)eMarkNone);
    inputCursor= new Mark(0, 0, eStateNone, (eMarkFlags)eMarkNone);
    MarksToEOT();

    state= 0;
    pendingCR= FALSE;
    noEchoBuffer= new CheapText(20);
    GetText()->AddMark(outputCursor);
    GetText()->AddMark(inputCursor);
    gSystem->Setenv("TERM", GetTerm());
    gSystem->Setenv("TERMCAP", GetTermCap());
    if (name == 0)
	name= gDefaultArgv[0];
    argv= args;
    if (args == 0)
	argv= gDefaultArgv;
    pttycon= gSystem->MakePttyConnection(name, argv);
    if (!pttycon)
	Error("Init", "cannot establish connection with tty");
    if (cons)
	BecomeConsole(); 
    if (pttycon->GetFile() == 0)
	Error("Init", "could not spawn slave pseudo tty");  
    pttycon->GetPttyChars(&pttyc);
    pttycstr= strprintf("\n\r%c%c%c%c%c", pttyc.rprnt, pttyc.susp, 
					       pttyc.intr, pttyc.quit,
					       pttyc.eof);
    SetStopChars(pttycstr);
    gSystem->AddFileInputHandler(
	       pttyinp= new PttyInputHandler(pttycon->GetFile(), this));
    gSystem->AddZombieHandler(
	       zombieHandler= new ShellZombieHandler(pttycon->GetPid(), this));       
}

ShellTextView::~ShellTextView()
{
    Term();
}

void ShellTextView::Term()
{
    Object *op;
    if (text) {
	op= text->RemoveMark(outputCursor);
	SafeDelete(op);
	op= text->RemoveMark(inputCursor);
	SafeDelete(op);
    }
    if (pttyinp) {
	pttyinp->Remove();
	pttyinp= 0;
    }
    if (zombieHandler) {
	zombieHandler->Remove();
	zombieHandler= 0;
    }
    SafeDelete(pttycon);
    SafeDelete(pttycstr);
    SafeDelete(noEchoBuffer);
}

Command *ShellTextView::DoKeyCommand(int ch, Point lp, Token token)
{
    CheapText buf(4096), *tosend;
    byte *cbuf;
    int n;
    
    if (pttycon == 0)
	return gNoChanges;
    if (ModifiesReadOnly())
	SetSelection(text->Size(), text->Size(), TRUE);
    if (pttycon->RawOrCBreak()) {
	char c= ch;
	pttycon->SubmitToSlave(&c, 1);
	lastSubmit= text->Size();
	if (!pttycon->Echo())
	    return gResetUndo;
	else
	    return FixedLineTextView::DoKeyCommand(ch, lp, token);
    }
    if (index(pttycstr,ch)) {
	if (ch == pttyc.intr || ch == pttyc.quit) {
	    SubmitInterrupt(ch);
	    return gResetUndo;
	}
	if (pttycon->Echo()) {
	    text->Copy(&buf,outputCursor->Pos(), text->Size());
	    tosend= &buf;
	}        
	else 
	    tosend= noEchoBuffer;
	tosend->Append(ch);
	n= tosend->Size();
	cbuf= tosend->GetTextAt(0, n);
	if (tosend == noEchoBuffer)
	    noEchoBuffer->Empty();
	Submit((char*) cbuf, n);
	return gNoChanges;
    }
    if (!pttycon->Echo()) {
	noEchoBuffer->Append(ch);
	return gNoChanges;
    }
    if (text->IsKindOf(StyledText) && ch != gBackspace) {
	int f, t;
	GetSelection(&f,&t);
	StyledText *st= (StyledText*) text;
	st->SetStyle(eStFace, f, t,
		    StyleSpec(eFontDefault, eFaceBold, 0, 0, FALSE));
    }
    if (Iscntrl(ch) && ch != gBackspace && ch != '\t')
	return CntrlChar((byte)ch);
    return FixedLineTextView::DoKeyCommand(ch, lp, token);
}

char *ShellTextView::GetTermCap()
{
    return "etterm|etterm dumb terminal emulator:bs";
}

char *ShellTextView::GetTerm()
{
    return "etterm";
}

void ShellTextView::Received()
{
    char buf[cPttyBufSize];
    int n= 0, f, t, upto;
    
    GetSelection(&f, &t);
    inputCursor->ChangeMark(f, t-f); 
    outputCursor->Unlock();
    if (pttycon)
	n= pttycon->Read(buf, sizeof(buf)-1);
	
    if (n <= 0)     // EOT from child or lost connection
	SlaveDied();
    else {  
	GapText gt(n);
	SetNoSelection(FALSE);
	SetNoBatch(pttycon->RawOrCBreak());
	ProcessOutput(&gt,buf, n);
	if (outputCursor->Pos() < lastSubmit)
	    upto= min(lastSubmit, text->Size());
	else 
	    upto= outputCursor->Pos();
	InsertReceivedText(&gt, outputCursor->Pos(), upto);
	if (outputCursor->Pos() > cHighWaterMark) 
	    Wrap();

	SetSelection(inputCursor->Pos(), inputCursor->End());
    }
    if (doReveal) {
	int l;
	Point p;
	CharToPos(outputCursor->Pos(), &l, &p);
	RevealRect(Rectangle(p, Point(20)), Point(20));
    }
    outputCursor->Lock();
    PerformCommand(gResetUndo);
}

void ShellTextView::InsertReceivedText(Text *t, int from, int to)
{
    if (!text->IsKindOf(StyledText))
	text->Paste(t,from,to);
    else {    
	StyledText *st= (StyledText*) text;
	int upto, ostart= outputCursor->Pos();
	st= (StyledText*) text;
	st->SetStyle(eStFace, from, to, StyleSpec(eFontDefault, eFacePlain, 0));
	text->Paste(t,from,to);
	if (ostart <= lastSubmit && !pttycon->RawOrCBreak()) {
	    upto= min(min(lastSubmit, outputCursor->Pos()), text->Size());
	    st->SetStyle(eStFace, ostart, upto, StyleSpec(eFontDefault, eFaceBold,0));
	    st->SetStyle(eStFace, upto, upto, StyleSpec(eFontDefault, eFacePlain,0));
	}
    }
}

void ShellTextView::ProcessOutput(Text *t, char *buf, int n)
{
    for (char *p= buf; p < buf+n; p++) {
	// during a more(1) spurious ESC '9's appear
	if (*p == cESC) {
	    state= TRUE;
	    continue;
	}
	if (state) {
	    state= FALSE;
	    continue;
	}  
	if (pendingCR && *p != '\n')
	    CarriageReturn(t);
	else
	    pendingCR= FALSE;
	    
	switch (*p) {
	case '\r':    
	    pendingCR= TRUE;                         
	    break;
	case '\b':
	    BackSpace(t);    
	    break;
	case '\007':
	    gWindow->Bell();
	    break;
	default:
	    t->Append(*p);
	}
    }
}

void ShellTextView::Submit(char *buf, int n)
{
    if (n && pttycon->SubmitToSlave(buf, n))
	ShowAlert(eAlertNote, "Pty command buffer overflow\nlast command ignored");
    lastSubmit= text->Size();
}

void ShellTextView::SubmitInterrupt(char ch)
{
    if (pttycon->SubmitToSlave(&ch, 1))
	pttycon->SubmitToSlave(&ch, 1); // try again after flush of buffers
}

void ShellTextView::SlaveDied()
{
    if (pttycon) 
	CleanupDeath();
}

void ShellTextView::CleanupDeath()
{
    if (pttyinp) {
	pttyinp->Remove();
	pttyinp= 0;
    }
    SafeDelete(pttycon);
    ForceRedraw();
}

void ShellTextView::DrawOutputCursor(Point p)
{
    Point hotspot(-crevCaretHeight, 0);
    Rectangle r(p+hotspot, Point(7,4));
    r.origin+= GetInnerOrigin();
    
    if (revCaretImage == 0)
	revCaretImage= new Bitmap(Point(crevCaretWidth,crevCaretHeight), revCaretBits);
    if (GrHasColor())
	GrPaintBitMap(r, revCaretImage, new RGBColor(0, 0, 255));
    else
	GrPaintBitMap(r, revCaretImage, ePatXor);
}

void ShellTextView::Draw(Rectangle r)
{
    Point p;
    int l;
    if (!pttycon)
	GrPaintRect(r, ePatGrey25);
    FixedLineTextView::Draw(r);
    if (pttycon && pttycon->RawOrCBreak())
	return;
    CharToPos(outputCursor->Pos(), &l, &p);
    DrawOutputCursor(p);
}

void ShellTextView::Paste(Text *insert)
{
    if (ModifiesReadOnly())
	return;
    if (pttycon && pttycon->RawOrCBreak()) {
	AutoTextIter next(insert);
	Token t;
	int ch;
	while ((ch= next()) != cEOT) {
	    t.Code= (short)ch;
	    DoKeyCommand((short)ch, gPoint0, t);
	}
    } else
	FixedLineTextView::Paste(insert);
}

bool ShellTextView::ModifiesReadOnly()
{
    int f, t;
    GetSelection(&f, &t);
    return f < outputCursor->Pos();
}
    
bool ShellTextView::DeleteRequest(int from, int)
{
    return from >= outputCursor->Pos();
}

void ShellTextView::BecomeConsole()
{
    if (pttycon)
	pttycon->BecomeConsole();
}

void ShellTextView::BackSpace(Text *t)
{
    int s= t->Size();
    if (s > 0)
	t->Cut(s-1,s);
    else {
	lastSubmit--;
	int p= outputCursor->Pos();
	outputCursor->ChangeMark(p-1, 0);
	text->Cut(p-1,p);
    }
}

void ShellTextView::CarriageReturn(Text *t)
{
    int s= t->Size(), ch;
    pendingCR= FALSE;
    while (s > 0) {
	int ch= (*t)[s];
	if (ch == '\n' || ch == '\r') {
	    t->Cut(s+1,t->Size());
	    return;
	}
	s--;
    }
    t->Empty();
    s= text->Size();
    int at= s-1;
    while (at >= 0) {
	ch= (*text)[at];
	if (ch == '\n' || ch == '\r') {
	    outputCursor->ChangeMark(at+1, 0);
	    text->Cut(at+1,s);
	    lastSubmit= at+1;
	    return;
	}
	at--;
    }
}

Command *ShellTextView::CntrlChar(byte b)
{
    GapText gt((byte*)"^");
    gt.Append(b+'@');
    return InsertText(&gt);
}

void ShellTextView::Wrap()
{
    int line, del, at= outputCursor->Pos();
    line= CharToLine(text->Size()-cLowWaterMark);
    del= StartLine(line);
    lastSubmit-= del;
    text->Cut(0, del);
}

Text *ShellTextView::SetText(Text *t)
{
    Text *ot= FixedLineTextView::SetText(t);
    if (ot) {
	ot->RemoveMark(outputCursor);
	ot->RemoveMark(inputCursor);
    }
    t->AddMark(outputCursor);
    t->AddMark(inputCursor);
    MarksToEOT();
    return ot;
}

void ShellTextView::MarksToEOT()
{
    Text *t= GetText();
    outputCursor->ChangeMark (t->Size(), 0);
    outputCursor->Lock();
    inputCursor->ChangeMark (t->Size(), 0);
    lastSubmit= t->Size();
}

void ShellTextView::Clear()
{
    text->Cut(0, text->Size());
    MarksToEOT();
    SetSelection(0,0,FALSE);
    RevealSelection();
}

void ShellTextView::DoReveal(bool b)
{
    if (!doReveal && b)
	RevealSelection();
    doReveal= b;
}

void ShellTextView::Reconnect() 
{
    int s= text->Size();
    outputCursor->ChangeMark (s, 0);
    outputCursor->Lock();
    inputCursor->ChangeMark (s, 0);
    lastSubmit= s;
    state= 0;
    pttycon= gSystem->MakePttyConnection(argv[0],argv); 
    if (pttycon->GetFile() == 0)
	Error("Reconnect", "could not spawn slave pseudo tty");  
    gSystem->AddFileInputHandler(
	       pttyinp= new PttyInputHandler(pttycon->GetFile(), this));
    gSystem->AddZombieHandler(
	       zombieHandler= new ShellZombieHandler(pttycon->GetPid(), this));       
    ForceRedraw();
}

void ShellTextView::DoSetupMenu(Menu *m)
{
    char *current;

    FixedLineTextView::DoSetupMenu(m);
    if (ModifiesReadOnly()) {
	m->DisableItem(cCUT);
	m->DisableItem(cPASTE);
    }
    if (doReveal)
	current= "auto reveal off";
    else
	current= "auto reveal on";

    m->ReplaceItem(cAUTOREVEAL, current);
    if (!pttycon)
	m->EnableItem(cRECONNECT);
    else 
	m->EnableItem(cBECOMECONSOLE);
    m->EnableItems(cCLEAR, cAUTOREVEAL, 0);
    if (!Caret())
	m->EnableItem(cDOIT);
}
    
Command *ShellTextView::DoMenuCommand(int cmd)
{
    switch (cmd) {

    case cAUTOREVEAL:
	DoReveal(!doReveal);
	break;
    
    case cCLEAR:
	Clear();
	return gResetUndo;
	
    case cRECONNECT:
	Reconnect();
	break;
	
    case cBECOMECONSOLE:
	pttycon->BecomeConsole();
	break;

    case cDOIT:
	Doit();
	return gResetUndo;
    }
    return FixedLineTextView::DoMenuCommand(cmd);
}

void ShellTextView::SelectAll()
{
    SetSelection(outputCursor->Pos(), text->Size());
    RevealSelection();
}

void ShellTextView::Doit()
{
    int f, t;
    GapText buf(256);
    Token token;
    
    GetSelection(&f, &t);
    text->Copy(&buf,f, t);
    SetSelection(text->Size(), text->Size());
    Paste(&buf);
    DoKeyCommand('\n', gPoint0, token);
}

void ShellTextView::SetTtySize(Rectangle r, Font *fd)
{
    int cols= r.extent.x / fd->Width(' ');
    int rows= r.extent.y / TextViewLineHeight(fd, GetSpacing()); 
    SetTtySize(rows, cols);
}

void ShellTextView::SetTtySize(int rows, int cols)
{
    if (pttycon)
	pttycon->SetSize(rows, cols);
}

Command *ShellTextView::TtyInput(char *buf, int len)
{
    Token t;
    Point p;
    Command *cmd= gNoChanges;
    if (len == -1)
	len= strlen(buf);
    for (char *cp= buf; cp < buf+len; cp++) {
	t.Code= (short)*cp;
	cmd= DoKeyCommand((int)*cp, gPoint0, t);
    }
    return cmd;
}

ostream& ShellTextView::PrintOn(ostream &s)
{
    FixedLineTextView::PrintOn(s);
    return s << doReveal SP;
}

istream& ShellTextView::ReadFrom(istream &s)
{
    FixedLineTextView::ReadFrom(s);
    Term();
    s >> Bool(doReveal);
    Init(doReveal, gDefaultArgv[0], gDefaultArgv, FALSE);
    MarksToEOT();
    return s;
}
