//$TextFormatter, SimpleFormatter, FoldingFormatter$

#include "TextFormatter.h"
#include "StaticTView.h"
  
//---- TextFormatter ----------------------------------------------------------

AbstractMetaImpl(TextFormatter, (TP(tv), TB(isPreemptive), T(startedAt), 0));

TextFormatter::TextFormatter(StaticTextView *t)
{
    tv= t;
}

int TextFormatter::DoIt(int , int , int )
{
    AbstractMethod("DoIt");
    return 0;
}

void TextFormatter::ResumeFormat(bool)
{
    AbstractMethod("ResumeFormat");
}

//---- SimpleFormatter -----------------------------------------------------

MetaImpl0(SimpleFormatter);

SimpleFormatter::SimpleFormatter(StaticTextView *t) : TextFormatter(t)
{
}

int SimpleFormatter::DoIt(int fromLine, int upto, int minUpto)
{
    register int end, line, start= tv->StartLine(fromLine);
    LineDesc ld, maxld;
    Text *t= tv->GetText();
    AutoTextIter next(t, start, upto);
    line= startedAt= fromLine;
    
    tv->StartFormatting(); // ???
    while ((end= next.Line(&ld)) != cEOT) {
	maxld.Max(ld);
	if (!tv->MarkLine(line,start,end,&maxld) && line >= minUpto) 
	    return (line-1);
	line++;
	start= end;
    }
    return line-1;
}

void SimpleFormatter::ResumeFormat(bool preemptive)
{
}

//---- class FoldingFormatter -----------------------------------------------

const int cMaxFormat = 3;      // maximum number of consecutive lines formatted 
			       // without interruption (if preemptive
			       // formatting is enabled)

MetaImpl(FoldingFormatter, (T(width), TP(text), TB(wasPreempted), T(start),
			    T(end), T(nWords), T(startLine), T(line), T(lastpeek),
			    T(wx), T(cx), T(preemptedAt), 0));

FoldingFormatter::FoldingFormatter(StaticTextView *t) : TextFormatter(t)
{
    preemptedAt= -1;
}

int FoldingFormatter::DoIt(int fromLine, int upto, int minUpto)
{
    int to;
	
    startedAt= max(0, fromLine-1);
    tv->StartFormatting(); // ???
    Mark before(tv->MarkAtLine(startedAt));
    to= Format(startedAt, tv->StartLine(startedAt), upto, max(minUpto, fromLine));
    Mark after(tv->MarkAtLine(startedAt));
    
    if (startedAt == fromLine-1 && !after.HasChanged(before.Pos(), before.Len()))
	startedAt++; // no backward propagation
    return to;
}

int FoldingFormatter::Format(int fromLine, int fromCh, int upto, int minUpto)
{
    Text *t= tv->GetText();
    AutoTextIter next(t, fromCh, upto);
    text= t;
    int ch;
    width= tv->GetInnerExtent().x;
    start= fromCh; 
    nWords= wx= cx= 0;
    line= startLine= lastpeek= fromLine;
    LineDesc maxld, ld;
	
    while (TRUE) {            
	// check whether formatting process should be preempted
	if (isPreemptive  && tv->TestFlag(eTextFormPreempt) && Suspend(minUpto))
	    return line-1;
	ch= next.Token(&wx,&ld);
	// handle special case if one word has to be folded on several lines
	if (cx + wx > width && nWords == 0) {
	    end= next.GetPos();
	    BreakWord (&maxld);
	    wx= 0;
	}

	switch (ch) {
	case cEOT: 
	    maxld.Max(ld);
	    tv->MarkLine(line, start, next.GetPos(), &maxld);
	    return line;

	case '\t':
	    wx = text->Tabulate(cx);  // ---> no break
	case ' ': case '\n': case '\r':            
	    maxld.Max(ld);
	    if (ch == '\n' || ch == '\r' || (cx + wx > width)) {
		end= next.GetPos();
		if (!tv->MarkLine(line,start,end,&maxld) && line >= minUpto) 
		    return (line-1);
		line++;
		start= end;
		cx= 0;
		nWords= 0;
	    } else {
		cx+= wx;
		nWords++;
	    }
	    break;

	default:
	    if (cx + wx > width) {  // wordwrap
		end= next.GetLastPos();
		if (!tv->MarkLine(line,start,end,&maxld) && line >= minUpto)  // line did not change
		    return (line-1);
		maxld= ld;
		line++;
		start= end;
		nWords= 0;
		cx= wx;
	    } else {
		maxld.Max(ld);
		cx+= wx;
		nWords++;
	    }
	    break;
	}
    }
}

void FoldingFormatter::BreakWord(LineDesc *maxld)
{
    int l= 0, w, cw= 0; 
    AutoTextIter ti(text, start, end); // avoid nesting of nextc
    LineDesc ld;

    maxld->Reset();
    while (ti(&w,&ld) != cEOT) 
	if (cw + w > width) {
	    cw= w;
	    end= ti.GetPos() - 1;
	    tv->MarkLine(line,start,end, maxld); 
	    *maxld= ld;
	    line++;
	    start= end;
	} else {      
	    maxld->Max(ld);
	    cw+= w;
	}
    cx= cw;
    end= ti.GetPos();
} 
 
bool FoldingFormatter::Suspend(int minUpto)
{
    if (line > minUpto && line-lastpeek > cMaxFormat) {
	lastpeek= line;
	if (tv->SuspendFormatting()) {
	    if (preemptedAt != -1) 
		preemptedAt= min(line, preemptedAt);
	    else 
		preemptedAt= line;
	    wasPreempted= TRUE;
	    tv->MarkLineAsChanged(line);
	    return TRUE;
	}        
    }
    return FALSE;
}

void FoldingFormatter::ResumeFormat(bool preempt)
{
    register int i;

    if (preemptedAt == -1)
	return;
    isPreemptive= preempt;
    wasPreempted= FALSE;
    for (i= preemptedAt; i < tv->NumberOfLines(); i++)
	if (tv->MarkAtLine(i)->state != eStateNone) {
	    tv->ChangedAt(i, 0, FALSE, i);
	    if (wasPreempted)
		break;
	}
    isPreemptive= TRUE;
    preemptedAt= wasPreempted ? i : -1;
}

