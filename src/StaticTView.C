//$StaticTextView,LineMark$
overload Swap;

#include "StaticTView.h"
#include "String.h"
#include "View.h"
#include "BlankWin.h"
#include "FixedStorage.h"
#include "TextFormatter.h"

Rectangle gFitRect(cFit, cFit);

//---- class LineMark ------------------------------------------------------

MetaImpl0(LineMark);

int LineMark_lineChanged;

LineMark::LineMark(LineDesc ldesc, int pos, int len, eMarkState s)
							    : Mark(pos, len, s)
{
    ld = ldesc; 
}

LineMark::~LineMark()
{
}

ostream& LineMark::DisplayOn (ostream&s)
{ 
    Mark::DisplayOn(s);
    return s << ld.lnAscent << "/" << ld.lnHeight; 
}

void *LineMark::operator new(size_t sz)
{
    return MemPools::Alloc(sz);
}

void LineMark::operator delete(void *vp)
{
    MemPools::Free(vp, sizeof(LineMark)); 
}

//---- global functions --------------------------------------------------

int TextViewLineHeight(FontPtr fd, eSpacing sp)
{ 
    return ((int) sp) * fd->Spacing() / 2;
}
 
//----- StaticTextView Methods --------------------------------------------------

MetaImpl(StaticTextView, (TP(text), TE(just), TE(spacing), TB(wrap),
    TB(drawViewBorder), TB(vertExtend), TB(horExtend), T(border),
    T(nLines), TP(lines), TP(marks), 0));

StaticTextView::StaticTextView(EvtHandler *eh, Rectangle r, Text *t, eTextJust m, 
	  eSpacing sp, bool doWrap, TextViewFlags fl, Point b, int id) 
								: View(eh,r,id) 
{
    Init(r, t, m, sp, doWrap, fl, b);
}   

void StaticTextView::Init(Rectangle r, Text* t, eTextJust ej, eSpacing sp, 
			bool doWrap, TextViewFlags flags, Point itsBorder)
{
    lines= new ObjArray(32);
    contentRect= r;
    text= t;
    nextc= text->MakeIterator();
    just= ej;
    spacing= sp;
    wrap= doWrap;
    drawViewBorder= FALSE;
    marks= new MarkList;
    LineDesc ld;
    lines->AtPut(0,new LineMark(ld)); //dummy
    marks->Add((*lines)[0]);    
    vertExtend= (contentRect.extent.y == cFit);
    horExtend= (contentRect.extent.x == cFit);
    border= itsBorder;
    if (horExtend)
	wrap= FALSE;
    if (!wrap)
	formatter= new SimpleFormatter(this);
    else
	formatter= new FoldingFormatter(this);
    nLines= 0;
    fixedSpacing.FromFont(text->GetFont());
    formatter->Preemptive(FALSE);
    ChangedAt(0);
    formatter->Preemptive(TRUE);

    if ((flags & eTextViewVisMode) == eTextViewVisMode)
	SetFlag(eTextViewVisMode);
    if ((flags & eTextViewReadOnly) == eTextViewReadOnly)
	SetFlag(eTextViewReadOnly);
    if ((flags & eTextFormPreempt) == eTextFormPreempt)
	SetFlag(eTextFormPreempt);
    if ((flags & eTextFixedOrigin) == eTextFixedOrigin)
	SetFlag(eTextFixedOrigin);
    if ((flags & eTextViewNoBatch) == eTextViewNoBatch)
	SetFlag(eTextViewNoBatch);
    if ((flags & eTextFixedSpacing) == eTextFixedSpacing)
	SetFlag(eTextFixedSpacing);
}

bool StaticTextView::IsJustified(int endOfLine)
{
    return (just == eJustified && endOfLine != text->Size() &&
	    (*text)[endOfLine-1] != '\n' && (*text)[endOfLine-1] != '\r');
}

StaticTextView::~StaticTextView()
{
    if (lines)
	lines->FreeAll();
    SafeDelete(formatter);
    SafeDelete(marks);
    SafeDelete(lines);
    SafeDelete(nextc);
}

void StaticTextView::Draw(Rectangle r)
{
    register int i, lh, bh;
    int startAt= PointToLine(r.origin-GetInnerOrigin()); // r is in View coords
    Ink *oldtextink= 0; 

    if (startAt >= nLines)
	return;
       
    Point p= LineToPoint(startAt,TRUE) + GetInnerOrigin();
    Rectangle lineRect(GetInnerOrigin().x, p.y-BaseHeight(startAt),
				   GetInnerExtent().x, LineHeight(startAt));

    if (! Enabled()) {
	oldtextink= port->textink; // hack
	GrSetTextPattern(ePatGrey50);
    }

    bh= BaseHeight(startAt);
    for (i= startAt; ; i++) {
	if (! r.Intersects(lineRect))
	    break;
	DrawLine(p, i, lineRect, r);
	if (i >= nLines-1)
	    break;
	p.y+= (lh= LineHeight(i))-bh;
	p.y+= (bh= BaseHeight(i+1));
	lineRect.origin.y+= lh; 
    }        
    if (drawViewBorder) {
	GrSetPenPattern(ePatGrey50);
	GrSetPenSize(2);
	GrStrokeRect (Rectangle (contentRect.origin,contentRect.extent));
    }
    if (! Enabled())
	GrSetTextPattern(oldtextink);
}

void StaticTextView::DrawLine (Point p,int i,Rectangle, Rectangle rr)
{
    int start= StartLine(i), end= EndLine(i);
    bool justified= IsJustified(end);
    
    p.x+= FirstCharPos(start, end);
    if (Isspace((*text)[end-1]) && !TestFlag(eTextViewVisMode))     
	end= max(start, --end);
    GrTextMoveto(p);
    if (justified)
	text->DrawTextJust(start, end, GetInnerExtent().x, p, rr);
    else
	text->DrawText(start, end, rr);
}

void StaticTextView::SetFont(FontPtr fd)
{
    text->SetFont(fd);
    Reformat();
}

void StaticTextView::SetSpacing(eSpacing sp)
{
    spacing = sp;
    Reformat();
}

void StaticTextView::SetFormatter(TextFormatter *f)
{
    SafeDelete(formatter);
    formatter= f;
}

void StaticTextView::SetJust(eTextJust m)
{
    just = m;
    Reformat();
}

void StaticTextView::SetVisibleMode(bool m)
{
    SetFlag(eTextViewVisMode, m);
    Reformat();
}

bool StaticTextView::GetVisibleMode()
{ 
    return TestFlag(eTextViewVisMode); 
}

void StaticTextView::SetWordWrap(bool m)
{ 
    if (wrap == m)
	return;
    wrap= m;
    SafeDelete(formatter);
    if (wrap)
	formatter= new FoldingFormatter(this); 
    else
	formatter= new SimpleFormatter(this); 
    Reformat();
}

bool StaticTextView::GetWordWrap()
{ 
    return wrap; 
}

void StaticTextView::SetNoBatch(bool m)
{
    SetFlag(eTextViewNoBatch, m);
}

bool StaticTextView::GetNoBatch()
{ 
    return TestFlag(eTextViewNoBatch); 
}

Text *StaticTextView::SetText(Text *t)
{
    Text *old= text;
    text= t;   
    nextc= text->MakeIterator();
    Reformat();  
    Send(GetId(), cPartReplacedText, 0);
    return old;
}

void StaticTextView::SetString(byte *str, int len)
{
    text->ReplaceWithStr(str, len);
    Reformat();
    Send(GetId(), cPartReplacedText, 0);  
}

int StaticTextView::NumberOfLines()
{
    return nLines;
}

void StaticTextView::StartFormatting()
{
}

void StaticTextView::Reformat()
{
    nLines= 0;
    formatter->Preemptive(FALSE);
    ChangedAt(0);
    formatter->Preemptive(TRUE);
}

//---- mark a line and return whether line changed

bool StaticTextView::MarkLine (int line,int start,int end,LineDesc *ld)
{
    LineMark *m = 0;
    bool markChanged;

    if (TestFlag(eTextFixedSpacing)) {
	LineDesc tmp= fixedSpacing;
	fixedSpacing.Max(*ld);
	if (!tmp.IsEqual(fixedSpacing))
	    ForceRedraw();
	*ld= fixedSpacing;    
    }

    if (line < lines->Size())   
	m = MarkAtLine(line);

    if (m) {
	markChanged = m->HasChanged(start,end-start) || line >= nLines;
	m->ChangeMark(start,end-start,*ld);
	ld->Reset();
	return markChanged;
    }    
    m = new LineMark(*ld,start,end - start);
    ld->Reset();
    marks->Add(m);
    lines->AtPutAndExpand(line,m);
    return TRUE;
}

void StaticTextView::MarkLineAsChanged (int line)
{
    LineMark *m = MarkAtLine(max(0,min(line,nLines-1)));
    m->state = eStateChanged;   
}

bool StaticTextView::SuspendFormatting()
{
    Token t= gWindow->ReadEvent(0);
    if (t.Code != eEvtNone) {
	gWindow->PushBackEvent(t);
	return TRUE;
    }
    return FALSE;  
}

void StaticTextView::SetExtent(Point p)
{
    if (p != contentRect.extent) {
	View::SetExtent(p);
	if (horExtend)
	    wrap= FALSE;
	Reformat();
    }
}

Metric StaticTextView::GetMinSize()
{
    Reformat();
    return Metric(GetExtent(), Base());
}

int StaticTextView::Base()
{
    return LineToPoint(0, TRUE).y+border.y;
}

void StaticTextView::Fit()
{
    Point newExtent(contentRect.extent);
    Point newOrigin(contentRect.origin);
    register int i, s, e;

    if (vertExtend) 
	newExtent.y = LineToPoint(nLines).y + 2*border.y;

    if (horExtend) {
	newExtent.x = 0;
	for (i= 0; i < nLines; i++) {
	    s= StartLine(i);
	    e= EndLine(i);
	    newExtent.x = max(newExtent.x, text->TextWidth(s,e));
	    if (!TestFlag(eTextFixedOrigin)) {
		// adjust origin
		switch (just) {
		case eCenter:
		    newOrigin.x = contentRect.origin.x + 
				  (contentRect.extent.x-newExtent.x)/2;
		    break;
		case eRight:
		    newOrigin.x = contentRect.origin.x + 
				  (contentRect.extent.x-newExtent.x);
		    break;
		}
	    }
	}
	newExtent.x += 2*border.x;
    }

    // the height of the view is at least the height of one line
    newExtent.y = max(newExtent.y,
		      TextViewLineHeight(text->GetFont(), spacing)+2*border.y);

    // invalidate difference Rects if necessary
    if (newExtent.x < contentRect.extent.x || newExtent.y < contentRect.extent.y) {
	Rectangle r[4], tmpRect;
	// expanded by 4,0 to invalidate the caret
	tmpRect= contentRect.Expand(Point(4,0)); // g++??
	int n= Difference(r, tmpRect, Rectangle(newOrigin,newExtent));
	for (int i= 0; i < n; i++) 
	    InvalidateRect(r[i]);
    }   
    if (newOrigin != contentRect.origin) {
	View::SetOrigin(newOrigin);
	if (TestFlag(eVObjLayoutCntl)) 
	    Control(GetId(), cPartOriginChanged, this);
	Send(GetId(), cPartOriginChanged, this); 
    }
    if (newExtent != contentRect.extent) {
	View::SetExtent(newExtent);
	if (TestFlag(eTextFixedOrigin)) 
	   InvalidateRect(contentRect);
	if (TestFlag(eVObjLayoutCntl)) 
	    Control(GetId(), cPartExtentChanged, this);
	Send(GetId(), cPartExtentChanged, this); 
    }    
}

Point StaticTextView::LineToPoint (int n, bool basePoint, bool relative)
{
    register int i, y= 0;

    n= range(0, nLines, n);
    for (i= 0; i < n; i++)
	y+= LineHeight(i);

    if (basePoint)
	y+= BaseHeight(n);
    if (relative)
	return Point(0, y);
    return GetInnerOrigin() + Point(0, y);
}

int StaticTextView::PointToLine(Point p) // p is in coordinates relative to contentRect
{
    register int line, y= 0, py= max(0, p.y), lh;
    
    for (line= 0; line < nLines; line++) {
	if (y + (lh= LineHeight(line)) > py)
	    break;
	y+= lh;
    }
    return (min(nLines, line));
}

//---- map a point in view coordinates to line and character number

void StaticTextView::PointToPos(Point p, Point *viewPos, int *lineNo, int *charNo)
{
    int start, end, fx= 0, cx= 0, l= *lineNo; // ???

    l = PointToLine(p);
    if (l >= nLines) {
	l = nLines-1;
	p.x = contentRect.extent.x;    // set to end of line
    }
    if (l < 0) {
	l = 0;
	p.x = 0;            // set to start of line
    }
    start  = StartLine(l);
    end    = EndLine(l);

    fx = FirstCharPos (start,end);

    bool justified = IsJustified(end);

    // make new lines at end of line not selectable
    if (end - start) {
	if ((*text)[end-1] == '\n' || (*text)[end-1] == '\r')
	    end--;
	if (wrap && (*text)[end-1] == ' ')
	    end--;
    }
    if (justified) 
	text->JustifiedMap(start, end, GetInnerExtent().x, end, p.x-fx, charNo, &cx); 
    else
	text->Map(start, end, end, p.x-fx, charNo, &cx);
    *lineNo = l;
    *viewPos = Point(cx+fx,LineToPoint(l).y);
}

int StaticTextView::CharToLine(int ch)
{
    //register int line;
    //for (line= 0; line < nLines; line++)
    //    if (EndLine(line) > ch) 
    //        break;
    //return max(0, min(line, nLines-1));
    register int base, pos, last, s;
    
    base= pos= 0;
    last= nLines-1;
    
    while (last >= base) { // binary search
	pos= (base+last) / 2;
	if (EndLine(pos) > ch && (s= StartLine(pos)) <= ch)
	    break;
	if (StartLine(pos) > ch)
	    last= pos-1;
	else
	    base= pos+1;
    }
    return max(0, min(pos, nLines-1));
}

void StaticTextView::CharToPos (int charNo, int *lineNo, Point *viewPos, bool relative)
{
    int line, ch, start, end, x;
    Point p;

    ch= min(max(0,charNo),text->Size()); // establish 0 <= ch <= size
    line= CharToLine(ch);
    p.y= LineToPoint(line).y;
    if (line >= nLines && line > 0) { // beyound end of text
	p.y-= LineHeight(line);
	line= max(0,nLines-1);
    }
    start= StartLine(line);
    end= EndLine(line);

    //-- find character position    
    p.x= FirstCharPos (start,end);

    bool justified= IsJustified(end);
    if (justified) 
	text->JustifiedMap(start, end, GetInnerExtent().x, charNo, cMaxInt, 0, &x);
    else
	text->Map(start, end, charNo, cMaxInt, 0, &x);
    *viewPos= p;
    viewPos->x+= x;
    if (!relative)
	*viewPos += GetInnerOrigin();
    *lineNo= line;
}

#include "System.h"

static int hcacheline= -1 , hcachelh= 0;

int StaticTextView::LineHeight(int l)
{
    if (hcacheline != l) {
	// hcacheline= l;
    
	hcachelh= TestFlag(eTextFixedSpacing) ? 
	    TextViewlh(spacing, fixedSpacing.lnHeight) :
	    l >= nLines ? 
		OutOfRangeLineMetric(TRUE) :
		TextViewlh(spacing, MarkAtLine(l)->ld.lnHeight);
    }
    return hcachelh;
}
    
static int bcacheline= -1 , bcachelb= 0;

int StaticTextView::BaseHeight(int l)
{
    if (bcacheline != l) {
	// bcacheline= l;
    
	bcachelb= TestFlag(eTextFixedSpacing) ? 
	    fixedSpacing.lnAscent :
	    l >=nLines ? 
		OutOfRangeLineMetric(FALSE) : 
		MarkAtLine(l)->ld.lnAscent;
    }
    return bcachelb;
}

int StaticTextView::FirstCharPos (int from,int to)
{
    //---- strip trailing whitespace, blank, tab or new line
    if (Isspace((*text)[to-1]))
	to= max(0, --to);

    switch (just) {
    case eCenter:
	return (GetInnerExtent().x - text->TextWidth(from,to))/2;
    case eRight:
	return GetInnerExtent().x - text->TextWidth(from,to);
    }
    return 0;
}

void StaticTextView::ChangedAt(int line, int, bool redrawAll, int minUpto)
{ 
    int start, to;

    LineMark_lineChanged= FALSE;
    
    to= formatter->DoIt(line, cMaxInt, minUpto);
    start= formatter->StartedAt();
    nLines= max(nLines, to+1);
    
    if (vertExtend || horExtend)
	Fit(); 
    if (redrawAll || LineMark_lineChanged)
	to= nLines-1;
    InvalidateRange(start, max(to, start));
}

void StaticTextView::InvalidateRange(int from, int to)
{
    if (from > to)  // normalize range
	swap(from, to);
    Point p= LineToPoint(from),
	  t= Point(contentRect.extent.x, LineToPoint(to).y+LineHeight(to));
    Rectangle r= NormRect(p, t);

    r += GetInnerOrigin();
    if (from == 0)  { // consider border
	r.origin.y -= border.y;
	r.extent.y += border.y;
    }
    if (to == nLines-1 || to == 0) 
	r.extent.y += border.y;
    InvalidateRect(r.Expand(Point(max(4,border.x),0)));
}

void StaticTextView::InvalidateRange(int from, Point fp, int to, Point tp)
{
    Rectangle r, r1;

    from= range(0, nLines, from);
    to=   range(0, nLines, to);

    if ((from == to && fp.x > tp.x) || from > to) { // normalize range
	swap(from, to);
	Swap(fp, tp);   
    }

    if (from == to) { // optimize invalidate on one line
	r= Rectangle(LineToPoint(from)+Point (fp.x,0),
					Point(tp.x-fp.x, LineHeight(from)));
	if (from == 0)  { // consider border
	    r.origin.y -= border.y;
	    r.extent.y += border.y;
	}
	if (to == nLines-1 || to == 0) 
	    r.extent.y += border.y;
	r.origin+= GetInnerOrigin();
	InvalidateRect(r.Expand(Point(max(4,border.x),0)));
	return;
    }
    InvalidateRange(from, to);
}

void StaticTextView::Dump()
{
    drawViewBorder = TRUE;    
    cerr << "Lines: " << nLines NL;
    cerr << "Contents:\n"<< *text NL;
    for (int i = 0; i < nLines; i++)
	cerr << i << "." << (*lines)[i]->ClassName(), (*lines)[i]->DisplayOn(cerr), cerr NL;;
}

char *StaticTextView::AsString()
{
    return text->AsString();
}

ostream& StaticTextView::PrintOn (ostream&s)
{
    View::PrintOn(s);
    return s << text SP << wrap SP << spacing SP << just SP << 
		vertExtend SP << horExtend SP << border SP << contentRect.extent;
}

istream& StaticTextView::ReadFrom(istream &s)
{
    Text *txt;
    bool wrap, vExtend, hExtend;
    Point extent, itsBorder;
    eSpacing sp;
    eTextJust just;
    Rectangle r;

    SafeDelete(text);

    View::ReadFrom(s);
    s >> txt >> Bool(wrap) >> Enum(sp) >> Enum(just) >> Bool(vExtend)
				    >> Bool(hExtend) >> itsBorder >> extent;
    r= Rectangle(extent);
    if (vExtend)
	r.extent.y = cFit;
    if (hExtend)
	r.extent.x = cFit;

    Init(r, txt, just, sp, wrap, eTextViewNone, itsBorder);
    return s;
}

void StaticTextView::AddMark(Mark *m)
{
    text->AddMark(m);
}

Mark *StaticTextView::RemoveMark(Mark *m)
{
    return text->RemoveMark(m);
}

Iterator *StaticTextView::GetMarkIter()
{
    return text->GetMarkIter();
}

MarkList *StaticTextView::GetMarkList()
{
    return text->GetMarkList();
}

int StaticTextView::OutOfRangeLineMetric(bool lineHeight)
{
    if (lineHeight)
	return TextViewLineHeight(text->GetFont(), spacing);  
    return text->GetFont()->Ascender();      
}

void StaticTextView::InspectorId(char *buf, int sz)
{
    if (text)
	text->InspectorId(buf, sz);
    else
	View::InspectorId(buf, sz);
}

void StaticTextView::Parts(Collection* col)
{
    View::Parts(col);
    col->Add(text);
}        
