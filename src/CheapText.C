//$CheapText,CheapTextIter$
#include "CheapText.h"
#include "RegularExp.h"
#include "Port.h"
#include "Error.h"

const int cInitCap= 16;

static TextChanges changeRec;

//---- CheapText ------------------------------------------------------------

MetaImpl(CheapText, (T(next), T(size), TV(cont,next), TP(font), 0));

CheapText::CheapText(int s, FontPtr fd)
{ 
    if (s < 0)
	Error("CheapText::CheapText", "initial size < 0");
    size= max(cInitCap, s);
    cont= new byte[size];
    next= 0; 
    Init(next, fd);
}

CheapText::CheapText(byte *t, int len, FontPtr fd)
{   
    int ll= len;

    if (ll == -1)
	next= strlen((char*)t); 
    else
	next= len;

    size= max (cInitCap,(next / 5 + 1)* 6); // give some initial free space
    cont= new byte[size];
    BCOPY(t, cont, next);
    Init(next, fd);
    font= fd;
    SetTabWidth(fd->Width((byte*)"        ",8));
}

void CheapText::Init(int, FontPtr fd)
{
    font= fd;
    SetTabWidth(fd->Width((byte*)"        ",8));
    ink= ePatBlack;
}

CheapText::~CheapText()
{
    SafeDelete(cont);
}

void CheapText::Cut(int from,int to)
{
    if (!CheckRange(next, from, to))
	return;

    BCOPY(&cont[to], &cont[from], next-to);

    next-= (to - from);
    if (LowWaterMark()) 
	Expand(size/2);
    Text::Cut(from, to);
    Send(cIdNone, eTextDeleted, changeRec(from, to));     
}

void CheapText::Paste(TextPtr paste, int from, int to)
{
    CheapText *ct;
    byte *buf= 0;

    if (!paste->IsKindOf(CheapText)) { // try to convert
	int s= paste->Size();
	buf= new byte[s+1];
	paste->CopyInStr(buf, s+1, 0, s);
	ct= new CheapText(buf, s);
    } else
	ct = (CheapText *)paste; 

    int shift= ct->next - (to - from);

    if (!CheckRange(next, from, to))
	return;

    if (HighWaterMark(shift))
	Expand(GrowBy(size+shift));

    if (shift < 0)
	BCOPY(&cont[to], &cont[to+shift], next-to);
    else if (shift > 0)
	BCOPY(&cont[from], &cont[from+shift], next-from);

    //---- insert pasted text
    BCOPY(ct->cont, &cont[from], ct->next);
    next += shift;
    if (LowWaterMark())
	Expand(size/2);
    if (buf) {
	delete buf;
	delete ct;
    }
    Text::Paste(paste, from, to);
    Send(cIdNone, eTextReplaced, changeRec(from, to, paste->Size()));     
}

void CheapText::Copy(Text *save, int from, int to)
{
    if (!CheckRange(next,from,to) || save == 0)
	return;

    if (!save->IsKindOf(CheapText)) { // convert
	int s= to - from;
	byte *buf= new byte[s+1];
	BCOPY(&cont[from], buf, s);
	save->ReplaceWithStr(buf, s);
	delete buf;
	return;
    }

    CheapText *ct= (CheapText *)save;    
    register int nSave= to-from;

    if (ct->size < nSave) 
	ct->Expand(nSave);

    BCOPY(&cont[from], ct->cont, nSave);
    ct->next= nSave;
}

void CheapText::CopyInStr(byte *str, int size, int from, int to)
{
    if (!CheckRange(next, from, to) || str == 0)
	return;

    register int l= min(to, from+size-1) - from;
    BCOPY(&cont[from], str, l);
    str[l]= '\0';
}

void CheapText::ReplaceWithStr(byte *str, int len)
{
    if (len == -1)
	len= strlen((char*)str);
    if (size < len) 
	Expand(len);
    BCOPY(str, cont, len);
    next= len;
}

void CheapText::SetFStringVL(char *fmt, va_list va)
{
    Empty();
    char *buf= strvprintf(fmt, va);
    int len= strlen(buf); 
    if (size < len) 
	Expand(len);
    BCOPY((byte*)buf, (byte*)cont, len);
    next= len;
    SafeDelete(buf);
}

byte *CheapText::GetTextAt(int from, int to)
{
    if (!CheckRange(next, from, to))
	return 0;
    return &cont[from];
}

int CheapText::Search(RegularExp *rex, int *nMatched, int start, int range, 
						 bool forward)
{
    int pos;

    if (forward) 
	pos= rex->SearchForward((char*)cont, nMatched, start, Size(), range, 0);
    else
	pos= rex->SearchBackward((char*)cont, nMatched, start, Size(), range, 0); 
    return pos;
}

TextPtr CheapText::Save(int from, int to)
{
    if (!CheckRange(next, from, to))
	return 0;
    return new CheapText(&cont[from], to-from);
}

void CheapText::Insert(byte c, int from, int to)
{
    int shift= to - from + 1;

    if (!CheckRange(next, from, to))
	return;

    if (HighWaterMark(shift))
	Expand(GrowBy(size+shift));

    if (shift < 0)
	BCOPY(&cont[to], &cont[to+next], size-to);
    else if (shift > 0)
	BCOPY(&cont[from], &cont[from+shift], next-from);

    cont[from]= c;
    next+= shift;
    if (LowWaterMark())
	Expand(size/2);
    Text::Insert(c, from, to);
    Send(cIdNone, eTextReplaced, changeRec(from, to, 1));     
}

byte& CheapText::operator[](int i)
{
    if (!CheckRange(next, i, i))
	i= size-1;
    return cont[i];
}

void CheapText::Empty (int)                                         
{
    next= 0; 
}

int CheapText::Size()
{
    return next;
}   

bool CheapText::IsEmpty() 
{
    return (bool) (next == 0);
}

bool CheapText::IsEqual (ObjPtr text)                                         
{
    if (!text->IsKindOf(CheapText))
	return FALSE;
    CheapText *t= (CheapText *) text;   
    if (next != t->next) 
	return FALSE;
    return (bool) (BCMP(cont, t->cont, next) == 0);
}

unsigned long CheapText::Hash ()                                         
{
    register unsigned long hash;
    register byte *p;

    for (hash= 0, p= cont; p < cont+Size(); p++)
	hash= (hash << 1) ^ *p;
    return hash;
}

bool CheapText::IsEqualStr (byte *t)                                         
{
    if (next != strlen((char*)t)) 
	return FALSE;
    return (bool) (BCMP(cont, t, next) == 0);
}

void CheapText::Expand (int newSize)                                         
{
    if (newSize == 0)
	newSize= size * 2;

    if (newSize < Size())  // texts never shrink
	return;
    cont= (byte*) Realloc(cont, newSize);
    next= min(newSize, next);
    size= newSize;
}

TextIter *CheapText::MakeIterator(int from, int to)
{
    return new CheapTextIter(this,from,to);
}

ostream& CheapText::PrintOn(ostream& s)
{
    Text::PrintOn(s); 
    s << font SP << ink SP;
    return PrintString(s, cont, next);
}

istream& CheapText::ReadFrom(istream& s)
{
    SafeDelete(cont);
    Text::ReadFrom(s);
    s >> font >> ink;
    ReadString(s, &cont, &size);
    next= size;
    return s;
}

ostream& CheapText::PrintOnAsPureText (ostream& s)
{
    int sz= Size();

    for (int i= 0; i < sz; i++)
	s.put(CharAt(i));
    return s;
}

istream& CheapText::ReadFromAsPureText (istream& s, long sizeHint)
{
    char ch;

    if (sizeHint > 0) {
	int delta= (int)(sizeHint - size); // !!!
	if (delta > 0)
	    Expand(delta + 50);
    }
    next= 0; 
    while (s.get(ch)) {
	if (HighWaterMark(1))
	    Expand(GrowBy(size+1));
	cont[next++] = ch;
    }
    if (!s.eof())
	Error("ReadFromAsPureText", "something strange happened");
    return s;
}

int CheapText::TextWidth(int from, int to)
{
    if (!CheckRange(next, from ,to))
	return 0;

    register int w= 0;
    for (int i= from; i < to; i++) {
	if (CharAt(i) == '\t') 
	    w+= Tabulate(w); 
	else
	    w+= font->Width (CharAt(i));
    }
    return w;
}

void CheapText::DrawText(int from, int to, Rectangle)
{
    if (!CheckRange(next, from, to))
	return;

    SetDrawingState(from);
    Point start= GrGetTextPos();
    byte c;
    for (int i= from; i < to; i++) 
	if ((c= CharAt(i)) == '\t') 
	    GrTextAdvance(Tabulate(GrGetTextPos().x-start.x)); 
	else
	    GrDrawChar(c);
}

//----- class CheapTextIter ---------------------------------------------

CheapTextIter::CheapTextIter(Text *s, int from, int to) : TextIter(s, from, to)
{
    if (!s->IsKindOf(CheapText))
	Error("CheapTextIter::CheapTextIter", "CheapText expected");
    font= s->GetFont();
}

int CheapTextIter::operator()()      // returns cEOT at end
{
    return (ce == upto ? cEOT : ((CheapText*)ct)->cont[ce++]);
}

int CheapTextIter::operator()(int *w, LineDesc* ld)  
{
    CheapText *ctp = (CheapText*)ct;
    int ch;

    if (ld) 
	ld->FromFont(font);
    unget= ce;
    *w= 0;
    if (ce == upto)
	return cEOT;
    ch= ctp->cont[ce++];
    *w= font->Width(ch);
    return ch;
}

int CheapTextIter::Line(LineDesc* ld)  
{
    CheapText *ctp = (CheapText*)ct;
    if (ld) 
	ld->FromFont(font);

    unget= ce;
    if (ce == upto) {
	// special case if last line is empty
	int last= ctp->cont[ce-1];
	ce++;
	if (last == '\n' || last == '\r') 
	    return upto;
    }
    if (ce > upto)
	return cEOT;

    while (ce < upto) {
	int ch= ctp->cont[ce++];
	if (ch == '\n' || ch == '\r')
	    break;
    }
    return ce;
}

int CheapTextIter::Token(int *w,LineDesc* ld)  
{
    CheapText *ctp = (CheapText*)ct;
    unget= ce;
    *w = 0;
    if (ld) 
	ld->FromFont(font);
    if (ce >= upto)
	return cEOT;

    register int ch= ctp->CharAt(ce);
    if (Isspace(ch)) {
	*w = font->Width(ch);
	ce++;
	return (ch);
    }
    while (ce < upto && !Isspace(ctp->CharAt(ce))) {
	ch = ctp->CharAt(ce++);
	*w += font->Width(ch);
    }
    return (ch);
}
