//$Text,TextIter$
#include "Text.h"
#include "Error.h"
#include "RegularExp.h"
#include "Mark.h"
#include "Port.h"
#include "OrdColl.h"

//---- LineDesc --------------------------------------------------------

LineDesc::LineDesc(int b, int h)
{
    lnAscent = b;
    lnHeight = h;
} 

void LineDesc::Reset()
{
    lnAscent = 0;
    lnHeight = 0;
}

void LineDesc::Max(FontPtr f)
{ 
    lnHeight =  max(lnHeight-lnAscent,f->Spacing()-f->Ascender()) +
		max(f->Ascender(),lnAscent);
    lnAscent =  max(f->Ascender(),lnAscent);
}

void LineDesc::Max(LineDesc ld)
{ 
    lnHeight =  max(lnHeight-lnAscent,ld.lnHeight - ld.lnAscent) +
		max(ld.lnAscent,lnAscent);
    lnAscent =  max(ld.lnAscent,lnAscent);
}

void LineDesc::Max(int ascent, int height)
{
    lnHeight =  max(lnHeight-lnAscent,height-ascent) +
		max(ascent,lnAscent);
    lnAscent =  max(ascent,lnAscent);     
}    

ostream& LineDesc::DisplayOn (ostream&s)
{
    return s << "Ascent=" << lnAscent << "/Height=" << lnHeight;
}

//---- Text ------------------------------------------------------------

AbstractMetaImpl(Text, (TP(marks), TP(observers), 0));

Text::Text()
{ 
    tabWidth = cTabw;
    marks = 0;
    observers= 0;
}

Text::~Text()
{   
    if (marks) {
	marks->FreeAll();
	delete marks;
    }   
    if (observers) 
	CleanupObservers(observers);
}

void Text::InitNew()
{
    marks = 0;
    tabWidth = cTabw;
}

void Text::CalcIws (int width, int from, int *to, int *addSpace, 
						int *longBlanks, int *ntab)
{
    int nBlanks;
    int ch;

    nBlanks = *addSpace = *longBlanks = *ntab = 0;

    for ( ;*to-1 >= from && ((*this)[*to-1] == ' ' || (*this)[*to-1] == '\t'); 
									(*to)--) 
	;

    AutoTextIter ti(this,from,*to);
    int w = TextWidth(from,*to);
    while ((ch = ti()) != cEOT) {
	if (ch == '\t') {
	    (*ntab)++;
	    nBlanks = 0;
	}
	else if (ch == ' ')
	    nBlanks++;
    }
    if (nBlanks) {
	*longBlanks = (width - w) % nBlanks;
	*addSpace = (width - w) / nBlanks;
    }
}

void Text::Cut(int from,int to)
{
    if (marks)
	marks->Cut(from,to - from);
}

void Text::Paste(TextPtr t, int from, int to)
{
    if (marks) {
	if (from != to)
	    marks->Cut(from,to - from);
	marks->Paste(from,t->Size()); 
    }
}

void Text::Copy(Text *, int , int)
{
    AbstractMethod("Copy");
}

void Text::CopyInStr(byte*, int, int, int)
{
    AbstractMethod("CopyInStr");
}

void Text::ReplaceWithStr(byte*, int)
{
    AbstractMethod("ReplaceWithStr");
}

void Text::SetFStringVL(char *, va_list)
{
    AbstractMethod("SetFStringVL");
}

void Text::SetFString(char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    SetFStringVL(va_(fmt), ap);
    va_end(ap);    
}

byte *Text::GetTextAt(int, int)
{
    AbstractMethod("GetTextAt");
    return 0;
}

int Text::Search(RegularExp *rex, int *nMatched, int start, int range, 
						 bool forward)
{
    int pos;
    char *s = AsString();
    if (forward) 
	pos= rex->SearchForward (s, nMatched, start, Size(), range, 0);
    else
	pos= rex->SearchBackward (s, nMatched, start, Size(), range, 0); 
    delete s;
    return pos;
}

Text *Text::GetScratchText(int) 
{
    return (TextPtr)this->New();
}

TextPtr Text::Save(int, int)
{
    AbstractMethod("Save");
    return 0;
}

void Text::Insert(byte, int from, int to)
{
    if (marks) {
	if (from != to)
	    marks->Cut(from,to - from);
	marks->Paste(from,1); 
    }
}

void Text::Append(byte c)
{
    Insert(c, Size(), Size());
}

static byte dummyc;

byte& Text::operator[](int )
{
    AbstractMethod("operator[]");
    return dummyc;
}

void Text::Empty (int)                                         
{
    AbstractMethod("Empty");
}

int Text::Size ()                                         
{
    AbstractMethod("Size");
    return 0;
}

void Text::GetWordBoundaries(int at, int *start, int *end)
{
    register int i;

    if (!CheckRange(Size(), at, at))
	return;

    for (i = at-1; i >= 0 && Isinword((*this)[i]); i--)
	;
    *start = i+1; 
    for (i = at; i < Size() && Isinword((*this)[i]); i++)
	; 
    *end = i;
}

void Text::GetParagraphBoundaries(int at,int *start,int *end)
{
    register int i, ch;

    if (!CheckRange(Size(), at, at))
	return;

    for (i = at-1; i >= 0; i--) {
	ch= (*this)[i];
	if (ch == '\n' || ch == '\r')
	    break;
    }
    *start = i+1; 
    for (i = at; i < Size(); i++) {
	ch= (*this)[i];
	if (ch == '\n' || ch == '\r')
	    break; 
    }
    *end= min(Size(), i+1);
}

int Text::Tabulate(int x)
{
    if (tabWidth > 0) {
	int n = x / tabWidth;
	return ((n+1) * tabWidth - x);
    }
    return 0;
}

int Text::SetTabWidth(int t)
{
    int ot= tabWidth;
    tabWidth= t;
    return ot;
}

TextIter *Text::MakeIterator(int,int)
{
    AbstractMethod("MakeIterator ");
    return 0;    
}

void Text::SetFont(FontPtr fp) 
{
    font= fp;
}

FontPtr Text::GetFont(int)
{
    return font;
}

void Text::SetInk(Ink *c) 
{
    ink= c;
}

Ink *Text::GetInk(int)
{
    return ink;
}

FontPtr Text::SetDrawingState(int at)
{
    FontPtr fp= GetFont();
    GrSetFont(GetFont(at));
    GrSetTextPattern(GetInk());
    return fp;        
}

void Text::ResetCurrentStyle()
{
}

int Text::TextWidth(int, int)
{
    AbstractMethod("TextWidth ");
    return 0;
}

void Text::DrawText(int, int, Rectangle)
{
    AbstractMethod("DrawText ");
}

void Text::DrawTextJust (int from, int to, int w, Point start, Rectangle)
{
    register byte ch;
    int ntab, longBlanks, addSpace, seenTabs;

    if (!CheckRange(Size(), from, to))
	return;

    ntab= longBlanks= seenTabs = 0;

    CalcIws(w, from, &to, &addSpace, &longBlanks, &ntab);    

    FontPtr font= SetDrawingState(from);
    GrTextMoveto(start);

    for (int i = from, nBlanks = 0; i < to; i++) {
	switch (ch = (*this)[i]) {
	case '\t':
	    GrTextAdvance(Tabulate(GrGetTextPos().x-start.x));
	    seenTabs++;
	    break;
	case ' ':
	    GrTextAdvance(font->Width(' '));
	    if (seenTabs == ntab) {
		GrTextAdvance(addSpace);
		if (nBlanks < longBlanks)
		    GrTextAdvance(1);
		nBlanks++;
	    }
	    break;
	default:
	    GrDrawChar(ch);
	    break;
	}
    }
}

void Text::JustifiedMap(int from, int to, int w, int stopAt, int posX, 
							   int *charPos, int *x)
{
    register byte ch;
    int ntab, longBlanks, addSpace, seenTabs, cx, wx;
    FontPtr font = GetFont();
    if (!CheckRange(Size(), from, to))
	return;

    wx= cx= ntab= longBlanks= addSpace= seenTabs= 0;

    CalcIws(w, from, &to, &addSpace, &longBlanks, &ntab);    

    for (int i = from, nBlanks = 0; i < to && i < stopAt; i++) {
	switch (ch = (*this)[i]) {
	case '\t':
	    wx = Tabulate(cx);
	    seenTabs++;
	    break;
	case ' ':
	    wx = font->Width(' ');
	    if (seenTabs == ntab) {
		wx += addSpace;
		if (nBlanks < longBlanks)
		    wx++; 
		nBlanks++;
	    }
	    break;
	default:
	    wx = font->Width(ch); 
	    break;
	}
	if (cx + (wx/2) > posX)
	    break;
	cx += wx;
    }
    if (x)
	*x = cx;
    if (charPos)
	*charPos = i;
}

void Text::Map(int from, int to, int stopAt, int posX, int *charPos, int *x)
{
    int ch,cx,wx,i;

    cx= wx= 0;

    AutoTextIter ti(this,from,to);

    for (i= from; (ch = ti(&wx)) != cEOT && i < stopAt ; i++) {
	if (ch == '\t') 
	    wx = Tabulate(cx);
	if (cx + (wx/2) > posX) 
	    break;
	cx += wx;
    }    
    if (charPos)
	*charPos = i;
    if (x)
	*x = cx;
}

char *Text::AsString()
{
    int s= Size();
    char *buf= new char[s+1];
    CopyInStr((byte*)buf, s+1, 0, s);
    return buf;    
}

int Text::AsInt()
{
    int val;
    if (Size() == 0)
	return 0;
    char *p= AsString();
    sscanf(p, "%d", &val);
    delete p;
    return val; 
}

float Text::AsFloat()
{
    float val;
    char *p = AsString();
    sscanf(p,"%f",&val);
    delete p;
    return val; 
}

ostream& Text::PrintOnAsPureText(ostream &s)
{
    AbstractMethod("PrintOnAsPureText");
    return s;
}

istream& Text::ReadFromAsPureText(istream &s, long)
{
    AbstractMethod("ReadFromAsPureText");
    return s;
}

bool Text::IsEmpty() 
{
    return Size() == 0;
}

int Text::GrowBy(int desiredSize)
{
    int s= 0;

    if (Size() >= cMaxInt)
	Error("GrowBy", "cannot expand text");
    else 
	s= range(2, cMaxInt - desiredSize, desiredSize);
    return Size()+s;
} 

void Text::AddMark(Mark *m)
{
    if (marks == 0)
	marks= new MarkList;
    marks->Add(m);
}

Mark *Text::RemoveMark(Mark *m)
{
    if (marks == 0)
	marks= new MarkList;
    return (Mark*)marks->Remove(m);
}

Iterator *Text::GetMarkIter()
{
    if (marks == 0)
	marks= new MarkList;
    return marks->MakeIterator();
}

MarkList *Text::GetMarkList()
{
    if (marks == 0)
	marks= new MarkList;
    return marks;
}

void Text::InspectorId(char *b, int s)
{
    int c;
    AutoTextIter next(this, 0, s-1);
    for (int i= 0; (c= next()) != cEOT; i++)
	b[i]= c;
    b[i]= '\0';
}

//---- observing ----------------------------------------------------------
    
Collection *Text::MakeObserverColl()
{
    observers= new OrdCollection(4);
    return observers;
}

Collection *Text::GetObservers()
{    
    return observers;
}

void Text::DestroyObserverColl()
{
    SafeDelete(observers);
}

void Text::SetObserverColl(Collection *cp)
{
    SafeDelete(observers);
    observers= cp;
}

//----- class TextIter ----------------------------------------------------

TextIter::TextIter(Text *s,int from,int to)
{ 
    ct= s; 
    ce= max(from,0); 
    upto= min(to,s->Size());
}

void TextIter::Reset(Text *s,int from,int to)
{ 
    ct= s; 
    ce= max(from,0); 
    upto= min(to,s->Size());
}

int TextIter::operator()(int *,LineDesc*)  
{
    return cEOT;
}

int TextIter::operator()()  
{
    return cEOT;
}

int TextIter::Line(LineDesc*)  
{
    return cEOT;
}

int TextIter::GetPos()  
{
    return ce;
}

Font *TextIter::FontAt(int)
{ 
    return ct->GetFont(); 
} 

int TextIter::Unget()
{
    return ce= unget;
}

int TextIter::Token(int *,LineDesc*)  
{
    return cEOT;
}

int TextIter::GetLastPos()                 // get last position
{ 
    return unget; 
}

void TextIter::SetPos(int newPos)
{ 
    ce = newPos; 
}

AutoTextIter::AutoTextIter(Text *t, int from, int to)
{ 
    ti= t->MakeIterator(from,to); 
}
