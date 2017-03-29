//$QuickPasteSelector,TextCommand,CutCopyCommand,ChangeStyleCommand,ExtendRangeSelector$
//$PasteCommand,TypeingCommand,CopyDragPasteSelector,RangeSelector$

#include "TextView.h" 
#include "TextCmd.h"
#include "CmdNo.h"

//---- range functions ---------------------------------------------------------

void CharacterRange(Text*, int at, int *start, int *end)
{
    *start= *end= at;    
}

void WordRange(Text *t, int at, int *start, int *end)
{
    t->GetWordBoundaries(at, start, end);
}

void ParagraphRange(Text *t, int at, int *start, int *end)
{
    t->GetParagraphBoundaries(at, start, end);
}

//---- RangeSelector -----------------------------------------------------------

RangeSelector::RangeSelector(class TextView *t, TextRangeFP f)
{
    tv= t;
    range= f;
}

Command *RangeSelector::TrackMouse(TrackPhase tp, Point, Point np, Point)
{
    SelPoint nextp;
    int start, end;
    
    np= np-tv->GetInnerOrigin();
    
    switch (tp) {
    case eTrackPress:
	tv->DoneTypeing();
	tv->inTextSelector= TRUE;
	tv->PointToPos(np, &nextp.viewp, &nextp.line, &nextp.ch);
	range(tv->GetText(), nextp.ch, &start, &end);
	DoPress(nextp, start, end);
	break;
	
    case eTrackMove:
	tv->PointToPos(np, &nextp.viewp, &nextp.line, &nextp.ch);
	range(tv->GetText(), nextp.ch, &start, &end);
	DoMove(nextp, start, end);
	break;
	
    case eTrackRelease:
	tv->inTextSelector= FALSE;
	if ((tv->start.line == tv->end.line) && tv->Caret())
	    tv->InvalidateSelection();
	tv->NormSelection();  
	return gNoChanges;
    }
    return this;
}

void RangeSelector::DoPress(SelPoint, int s, int e)
{
    tv->PrivSetSelection(s, e, FALSE);
    startp= tv->start;
    endp= tv->end;
}

void RangeSelector::DoMove(SelPoint nextp, int s, int e)
{
    if (nextp.ch <= startp.ch) {
	tv->CharToPos (nextp.ch= s, &nextp.line, &nextp.viewp);
	if (!nextp.IsEqual(tv->start)) {
	    tv->InvalidateRange(nextp.line, nextp.viewp, tv->start.line, tv->start.viewp);
	    tv->start= nextp;
	}
	else if (tv->end.ch > endp.ch && !endp.IsEqual(tv->end)) {
	    tv->InvalidateRange(endp.line, endp.viewp, tv->end.line, tv->end.viewp);
	    tv->end= endp;
	}            
    } else {
	tv->CharToPos(nextp.ch= e, &nextp.line, &nextp.viewp);
	if (!nextp.IsEqual(tv->end)) {
	    tv->InvalidateRange(tv->end.line, tv->end.viewp, nextp.line, nextp.viewp);
	    tv->end= nextp;
	} 
	else if (tv->start.ch < startp.ch && !startp.IsEqual(tv->start)) {
	    tv->InvalidateRange(tv->start.line, tv->start.viewp, startp.line, startp.viewp);
	    tv->start= startp;
	}
    }
}

//---- ExtendRangeSelector ----------------------------------------------------

ExtendRangeSelector::ExtendRangeSelector(TextView *tv, TextRangeFP rf) 
							: RangeSelector(tv, rf)
{
    swap= FALSE;
}
 
void ExtendRangeSelector::DoPress(SelPoint nextp, int s, int e)
{
    SelPoint tmp;
    
    startp.ch= s;
    endp.ch= e;
    if (nextp.ch <= tv->start.ch) {     // extend at start 
	tv->CharToPos(startp.ch, &startp.line, &startp.viewp);
	tv->InvalidateRange(startp.line, startp.viewp, tv->start.line, tv->start.viewp);
	tv->start= startp;
	swap= TRUE;
    } else {                            // extend at end
	tv->CharToPos(endp.ch, &endp.line, &endp.viewp);
	tv->InvalidateRange(tv->end.line, tv->end.viewp, endp.line, endp.viewp);
	tv->end= endp;
    }
    startp= nextp;
}

void ExtendRangeSelector::DoMove(SelPoint nextp, int s, int e)
{
    if (swap) {
	swap= FALSE;
	SwapSelPoints(tv->start, tv->end);
    } 
    if (nextp.ch < startp.ch)
	tv->CharToPos (nextp.ch= s, &nextp.line, &nextp.viewp);
    else
	tv->CharToPos(nextp.ch= e, &nextp.line, &nextp.viewp);
	
    if (!nextp.IsEqual(tv->end)) {
	tv->InvalidateRange(nextp.line, nextp.viewp, tv->end.line, tv->end.viewp);
	tv->end= nextp;
    } 
}

//---- QuickPasteSelector ------------------------------------------------------

QuickPasteSelector::QuickPasteSelector(class TextView *tv, int f, int t)
							     : RangeSelector(tv)
{ 
    from= f;
    to= t;
}

Command *QuickPasteSelector::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    Command *cmd= RangeSelector::TrackMouse(tp, ap, pp, np);
    if (tp == eTrackRelease) {
	if (tv->Caret()) {
	    tv->SetSelection(from, to);
	    return gNoChanges;
	}
	Text *t= tv->SelectionAsText();
	tv->SetSelection(from, to);
	return new PasteCommand(tv, t); 
    }
    return cmd;
}

//---- CopyDragPasteSelector ---------------------------------------------------

CopyDragPasteSelector::CopyDragPasteSelector(class TextView *tvp, int f, int t)
{ 
    from= f;
    to= t;
    tv= tvp;
}

Command *CopyDragPasteSelector::TrackMouse(TrackPhase tp, Point, Point, Point)
{
    if (tp == eTrackRelease) {
	int cfrom, cto;
	tv->GetSelection(&cfrom, &cto);
	tv->SetSelection(from, to, TRUE);
	if (tv->Caret()) {
	    tv->SetSelection(cfrom, cto, TRUE);
	    return gNoChanges;
	}
	Text *t= tv->SelectionAsText();
	tv->SetSelection(cfrom, cto, TRUE);
	return new PasteCommand(tv, t); 
    }
    return this;
}

void CopyDragPasteSelector::TrackFeedback(Point, Point np, bool on)
{
    if (on) {
	Point p;
	int line, ch;
	tv->PointToPos(np, &p, &line, &ch);  
	tv->SetSelection(ch, ch, TRUE);
    }
}

//----- TextCommand Methods ----------------------------------------------------

AbstractMetaImpl(TextCommand, (TP(tv), TP(oldText), T(oldStart), T(oldEnd), 0));

TextCommand::TextCommand (TextView *t, int cmdNo, char *cmdName, bool save)
							: Command(cmdNo, cmdName)
{
    tv= t;
    tv->GetSelection(&oldStart, &oldEnd);
    oldText= (Text*)tv->GetText()->New();
    if (save && oldStart != oldEnd) // save old text
	tv->Copy(oldText);
}

TextCommand::~TextCommand ()
{
    SafeDelete(oldText);
} 

void TextCommand::RestoreSelection ()
{
    tv->SetSelection(oldStart, oldEnd, FALSE);
}

void TextCommand::RedoIt()
{
    RestoreSelection();
    DoIt();
    tv->RevealSelection();
}

//----- CutCopyCommand Methods -------------------------------------------------

MetaImpl0(CutCopyCommand);
 
CutCopyCommand::CutCopyCommand(TextView *t, int cmdNo, char *cmdName) 
						    : TextCommand(t, cmdNo, 0)
{ 
    if (cmdName == 0) {
	if (cmdNo == cCUT)
	    cmdName= "cut text";
	else if (cmdNo == cCOPY)
	    cmdName= "copy text";
    }
    SetName(cmdName);
    if (cmdNo == cCOPY)
	ResetFlag(eCmdCausesChange);
}

void CutCopyCommand::DoIt()
{
    if (GetId() == cCOPY)
	return;
    tv->Cut();
    tv->RevealSelection();
}  

void CutCopyCommand::UndoIt()
{
    Text *t= tv->GetText();    

    if (GetId() == cCUT) {
	tv->SetSelection(oldStart, oldStart, FALSE);
	if (oldText->Size()) 
	    tv->Paste(oldText);
	RestoreSelection();
	tv->RevealSelection();
    }
}

//----- Paste Command Methods --------------------------------------------------

MetaImpl(PasteCommand, (TP(pastetext), T(newStart), T(newEnd), 0));

PasteCommand::PasteCommand(TextView *t, Text *pt, int cmdNo, char *cmdName) 
			: TextCommand(t, cmdNo, cmdName ? cmdName : "paste text")
{
    pastetext= pt;
}

PasteCommand::~PasteCommand()
{
    SafeDelete(pastetext);
}

void PasteCommand::DoIt()
{
    tv->Paste(pastetext);
    tv->GetSelection(&newStart, &newEnd);
    tv->RevealSelection();
}  

void PasteCommand::UndoIt()
{
    tv->SetSelection(oldStart, newEnd, FALSE);
    tv->Cut();
    if (oldText->Size())
	tv->Paste(oldText);
    RestoreSelection();
    tv->RevealSelection();
}

//----- TypeInCommand Command Methods ------------------------------------------

MetaImpl(TypeingCommand, (TP(backspaceBuf), TP(newText), T(newStart), 0));

TypeingCommand::TypeingCommand(TextView *t, int cmdNo, char *cmdName) 
					       : TextCommand(t, cmdNo, cmdName) 
{
    Text *txt= tv->GetText();
    backspaceBuf= (Text*) txt->New();
    newText= (Text*) txt->New();
    newStart= oldStart;
    completed= FALSE;
    ResetFlag(eCmdDoDelete);
}

TypeingCommand::~TypeingCommand()
{
    SafeDelete(backspaceBuf);
    SafeDelete(newText);
}

void TypeingCommand::Done(Command *newCmd)
{
    if (newCmd != this) {
	if (newCmd == 0 || newCmd->GetId() != cTYPEING) {
	    SetFlag(eCmdDoDelete);
	    tv->TypeingDeleted();
	} else
	    completed= TRUE;
    }
}

void TypeingCommand::UndoIt()
{
    Text *t= tv->GetText();
    int firstNewChar;

    firstNewChar= min(oldStart - backspaceBuf->Size(),oldStart);
    if (firstNewChar != newStart) {                     // save new text
	t->Copy(newText, firstNewChar, newStart);         
	tv->SetSelection(firstNewChar, newStart, TRUE);
	tv->Cut();                                      // remove new text
    } else
	tv->SetSelection(firstNewChar, newStart, TRUE);
    if (backspaceBuf->Size())                            
	tv->Paste(backspaceBuf);                        // insert saved text
    if (oldText->Size())
	tv->Paste(oldText);
    RestoreSelection();
    tv->RevealSelection();
}

void TypeingCommand::RedoIt()
{
    RestoreSelection();
    tv->Cut();
    if (backspaceBuf->Size()) {
	tv->SetSelection(oldStart - backspaceBuf->Size(), oldStart, FALSE);
	tv->Cut();        
    }
    if (newText->Size())
	tv->Paste(newText);
    tv->RevealSelection();
}

void TypeingCommand::AddChar(int n)
{ 
    newStart+= n; 
}

void TypeingCommand::DelChar()
{
    Text *t= tv->GetText();

    if (newStart > 0)
	newStart--;  
    if (newStart < oldStart) 
	if (oldStart - newStart > backspaceBuf->Size()) { // char already saved??
	    Text *tmp= t->Save(newStart, newStart+1);
	    backspaceBuf->Paste(tmp, 0, 0);
	    SafeDelete(tmp);
	}
}

//---- class ChangeStyle -------------------------------------------------------

MetaImpl(ChangeStyleCommand, (TP(newStyles), TP(oldStyles), TP(tvp), TE(mode), 0));

ChangeStyleCommand::ChangeStyleCommand(TextView *t, int cmdNo, char *cmdName, 
	   StChangeStyle m, StyleSpec ns) : TextCommand(t, cmdNo, cmdName, FALSE)
{
    if (!t->GetText()->IsKindOf(StyledText))
	Error("ChangeStyleCommand::ChangeStyleCommand",
			    "Apply ChangeStyleCommands only to StyledText");
    newStyles= oldStyles= 0;
    tvp= t; 
    style= ns;
    mode= m;
}

void ChangeStyleCommand::DoIt()
{
    int from, to;
    StyledText *sp= (StyledText*) tvp->GetText();

    tvp->GetSelection(&from, &to);
    if (tvp->Caret())
	sp->SetStyle (mode, from, to, style);
    else {    
	if (oldStyles == 0) 
	    oldStyles= new RunArray;
	sp->CopyStyles (oldStyles, from, to);
	sp->SetStyle (mode, from, to, style);
    }
    tvp->RevealSelection();
}  

void ChangeStyleCommand::UndoIt()
{
    StyledText *sp= (StyledText*) tvp->GetText();

    if (oldStart == oldEnd)
	sp->ResetCurrentStyle();
    else {
	if (newStyles == 0) {
	    newStyles= new RunArray;
	    sp->CopyStyles (newStyles, oldStart, oldEnd);
	}
	sp->ReplaceStyles (oldStyles, oldStart, oldEnd); 
    }
    RestoreSelection();
    tvp->RevealSelection();
}

void ChangeStyleCommand::RedoIt()
{
    StyledText *sp= (StyledText*) tvp->GetText();
    if (oldStart == oldEnd)
	sp->SetStyle(mode, oldStart, oldEnd, style);        
    else
	sp->ReplaceStyles(newStyles, oldStart, oldEnd); 
    RestoreSelection();
    tvp->RevealSelection();
}
