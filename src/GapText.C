//$GapText,GapTextIter$
#include "GapText.h" 
#include "RegularExp.h"
#include "Port.h"
#include "Error.h"

const int cShrinkFactor = 2,
	  cMaxOutput    = 500,
	  cInitialSize  = 16;
char *cMemOverflow= "memory overflow",
     *cGapTextName= "CheckPtr";

static TextChanges changeRec;

//---- GapText -----------------------------------------------------------------

MetaImpl(GapText, (T(size), T(length), T(part1len), T(gaplen),
			TV(body, part1len), TV(body2, part2len), TP(font), 0));

void GapText::MoveGap(int to)
{
    if (to == part1len)
	return;

    if (part1len > to)
	BCOPY(body+to, body+to+gaplen, part1len-to);
    else 
	BCOPY(body+part1len+gaplen, body+part1len, to-part1len);

    part1len= to; 
}            

void GapText::CopyTo(byte *dst, int from, int to)
{
    // Copy Text between 'from' and 'to' to 'dst'

    int beforegap= max(0, part1len-from) - max(0, part1len-to);
    int aftergap = max(0, to - part1len) - max(0, from-part1len);

    if (beforegap) 
	BCOPY(body+from, dst, beforegap);
    if (aftergap)
	BCOPY(part2body+max(part1len, from), dst+beforegap, aftergap);
}

void GapText::Expand(int to, int moveto)
{ 
    // Expand size of Text to 'to'
    byte *pos, *part2;

    if (to == 0)
	to= size * 2;

    if (to < size) 
	return;

    if (moveto > length)
	moveto= length;

    size= to;
    pos= new byte[size];
    CheckPtr(pos);
    part2= pos + size -length;

    if (moveto < part1len) {
	BCOPY(body, pos, moveto);
	BCOPY(body + moveto, part2 + moveto, part1len - moveto);
	BCOPY(part2body + part1len, part2 + part1len, length -part1len);
    } else {
	BCOPY(body, pos, part1len);
	BCOPY(part2body + part1len, pos + part1len, moveto - part1len);
	BCOPY(part2body + moveto, part2 + moveto, length - moveto);
    }

    delete body;
    body= pos;

    Update(moveto);
}

void GapText::Shrink(int to)
{ 
    // Reduce size of Text to 'to'
    // If to = 0 then size = size / cShrinkFactor + 1

    byte *pos;

    if ((to == 0) || (to < length))
	to= size / cShrinkFactor + 1;

    if ( (to > size) || (to < length) ) 
	return;

    size= to;

    pos= new byte[size];
    CheckPtr(pos);

    BCOPY(body, pos, part1len);
    BCOPY(body+part1len+gaplen, pos+part1len, length-part1len);

    delete body;
    body= pos;

    Update(length);
}

GapText::GapText(int s, FontPtr fd)
{ 
    size= max(cInitialSize, s);
    body= new byte[size];
    CheckPtr(body);
    Init(0, fd);
}

GapText::GapText(byte *buf, int len, bool ic, FontPtr fd)
{   
    if (len < 0)
	len= strlen((char*)buf);
    size= max(cInitialSize, len);
    if (ic)
	body= buf;
    else {
	body= new byte[size];
	CheckPtr(body);
	BCOPY(buf, body, len);
    }
    Init(len, fd);
}

void GapText::Init(int l, FontPtr fd)
{
    length= l;
    Update(l);
    font= fd;
    ink= ePatBlack;
    SetTabWidth(fd->Width(' ')*8);
}

GapText::~GapText()
{
    SafeDelete(body);
    part2body= 0;
}

void GapText::Paste(TextPtr paste,int from,int to)
{
    if (!CheckRange(length,from,to))
	return;

    GapText *ft;
    byte *buf= 0;

    if (!paste->IsKindOf(GapText)) {  // convert the text into a GapText
	int s = paste->Size();
	buf= new byte[s+1];
	paste->CopyInStr(buf, s+1, 0, s);
	ft= new GapText(buf, s, TRUE);
    } else
	ft= (GapText *)paste;

    int shift= ft->length - (to - from);

    if (HighWaterMark(shift))
	Expand(GrowBy(size + shift), from);
    else
	MoveGap(from);

    ft->CopyTo(body+from, 0, ft->length);

    length    += shift;
    gaplen    -= shift;
    part1len  += ft->length;
    part2body -= shift;
    part2len= length - part1len;
    body2= part2body + part1len;

    if (LowWaterMark())
	Shrink();   
    if (buf)
	delete ft;
    Text::Paste(paste, from, to);
    Send(cIdNone, eTextReplaced, changeRec(from, to, paste->Size()));
}

void GapText::Cut(int from,int to)
{
    int shift= to - from;

    if (!CheckRange(length, from, to))
	return;

    if (from <= part1len && to >= part1len) // Gap is in between
	part1len= from;
    else  
	MoveGap(from); 
    length -= shift;
    gaplen += shift;
    part2body += shift;
    part2len= length - part1len;
    body2= part2body + part1len;

    if (LowWaterMark())
	Shrink();

    Text::Cut(from,to);   
    Send(cIdNone, eTextDeleted, changeRec(from, to));     
}  

void GapText::Copy(Text *save,int from, int to)
{
    if (!CheckRange(length,from,to) || (save == 0))
	return;

    if (!save->IsKindOf(GapText)){ // convert
	int s= to -from ;
	byte *buf= new byte[s+1];
	CopyTo(buf, from, to); 
	buf[to-from]= '\0';
	save->ReplaceWithStr(buf,s); 
	delete buf;
	return;
    }

    GapText *ft= (GapText*) save;
    int nSize= to - from;

    ft->Empty();

    if (ft->size < nSize) 
	ft->Expand(nSize, 0);

    CopyTo(ft->body, from, to);

    ft->length= to-from;
    ft->Update(to-from);
}

void GapText::CopyInStr(byte *str,int strsize,int from, int to)
{
    if (!CheckRange(length,from,to) || (str == 0))
	return;
    to= min(to,from + strsize-1);   
    CopyTo(str, from, to);
    str[to-from] = '\0';
}

void GapText::ReplaceWithStr(byte *str, int len)
{
    if (len < 0)
	len= strlen((char*)str);
    Empty();
    if (size < len) 
	Expand(len,0);
    BCOPY(str, body, len);
    length= len;
    Update(len);
}

void GapText::SetFStringVL(char *fmt, va_list ap)
{
    Empty();
    char *buf= strvprintf(fmt, ap);
    int l= strlen(buf); 
    if (size < l) 
	Expand(l,0);
    BCOPY((byte*)buf, body, l);
    length= l;
    Update(l);
    SafeDelete(buf);
}

int GapText::Search(RegularExp *rex, int *nMatched, int start, int range, 
						 bool forward)
{
    int pos;

    if (forward) 
	pos= rex->SearchForward2((char*)body, part1len,
					    (char*)(body+part1len+gaplen), 
			length-part1len, start, range, 0, nMatched);
    else {
	// BUG in RegularExp::SearchBackward2 ??
	MoveGap(length);
	pos= rex->SearchBackward((char*)body, nMatched, start, length, range, 0);
    } 
    return pos;
}

byte *GapText::GetTextAt(int from, int to)
{    
    if (!CheckRange(length,from,to))
	return 0;

    if ((part1len < to) && (part1len >= from))
	// Gap is in between 
	if ( part1len > (( to + from) >> 1) )   
	    MoveGap(to);
	else
	    MoveGap(from);

    if (part1len < from) 
	return &part2body[from];
    return &body[from];
}

TextPtr GapText::Save(int from, int to)
{
    if (!CheckRange(length,from,to))
	return 0;

    GapText* t= new GapText(to-from);
    Copy(t, from, to);
    return t;
}

void GapText::Insert(byte c, int from, int to)
{
    int shift= 1 - (to - from);

    if (!CheckRange(length,from,to))
	return;

    if (HighWaterMark(shift))
	Expand(GrowBy(size+shift), from);
    else     
	MoveGap(from);
    body[from] = c;

    length    += shift;
    gaplen    -= shift;
    part1len  += shift;
    part2body -= shift;
    part2len= length - part1len;
    body2= part2body + part1len;

    if (LowWaterMark())
	Shrink();

    Text::Insert(c, from, to);
    Send(cIdNone, eTextReplaced, changeRec(from, to, 1));     
}

byte& GapText::operator[](int i)
{
    //----> should notify about possible changes but how?? 
    i= min(i, length-1);
    if (i < part1len)
	return body[i];
    return part2body[i];
}

void GapText::Empty(int initSize)                                         
{
    if (initSize > 0) {
	delete body;
	body= new byte[size= initSize];
	CheckPtr(body);
    }
    length= 0;
    Update(0);
}

bool GapText::IsEqual(ObjPtr text)                                         
{
    if (!text->IsKindOf(GapText))
	return FALSE;
    GapText *t= (GapText *) text; 

    if (length != t->length) 
	return FALSE;

    MoveGap(length);
    t->MoveGap(t->length);
    return (strncmp((char*)body, (char*)t->body, length) == 0);
}

int GapText::Size()
{ 
    return length; 
}                                         

bool GapText::IsEmpty()
{   
    return length == 0; 
}

unsigned long GapText::Hash()                                         
{
    register unsigned long hash;
    register byte *p;

    for (hash= 0, p= body; p < body+part1len; p++)
	hash= (hash << 1) ^ *p;
    for (p= body+part1len+gaplen; p < body+size; p++)
	hash= (hash << 1) ^ *p;
    return hash;
}

bool GapText::IsEqualStr(byte *t)                                         
{
    if (length != strlen((char*)t)) 
	return FALSE;
    MoveGap(length);
    return strncmp((char*)body, (char*)t, length) == 0;
}

void GapText::Dump()
{   
    int i;
    cerr << "size : " << size << "\n";
    cerr << "length  : " << length << "\n";
    cerr << "part1len     : " << part1len << "\n";
    cerr << "gaplen  : " << gaplen << "\n";
    cerr << "part2body-body    : " << int(part2body-body) << "\n";
    cerr << "body  : ";
    for (i=0 ; i < size ; i++){
	if (i == part1len )
	    cerr << "[";
	if (i == (part1len + gaplen))
	    cerr << "]";
	if ((body[i]>= ' ') && (body[i] <='~'))
	    cerr.put(body[i]);
	else
	    cerr << ".";
    }
    cerr << "\n" ;
}

TextIter *GapText::MakeIterator(int from, int to)
{
    return new GapTextIter(this,from,to);
}

ostream& GapText::PrintOn(ostream& s)
{
    Text::PrintOn(s);
    s << font SP << ink SP; 
    MoveGap(length);
    return PrintString(s, body, length);
}

istream& GapText::ReadFrom(istream& s)
{
    Text::ReadFrom(s);
    s >> font >> ink;
    SafeDelete(body);
    ReadString(s, &body, &length);
    size= length;
    Update(length);
    return s;
}

ostream& GapText::PrintOnAsPureText(ostream& s)
{
    register int i;
    for (i= 0; i < length; i++)
	s.put(CharAt(i));
    return s;
}

istream& GapText::ReadFromAsPureText(istream& s, long sizeHint)
{
    char ch;

    if (sizeHint > 0 && ((int)(sizeHint - size)) > 0)
	Expand(GrowBy((int)sizeHint + 50));
    if (body == 0)
	body= new byte[size= 1024];
    length= 0;

    while (s.get(ch)) {
	if (length >= size){
	    part2body= body ;
	    body= new byte[size= (size+1)*2];
	    CheckPtr(body);
	    BCOPY(part2body, body, length);
	    delete part2body;
	}
	body[length++]= ch;
    }

    Update(length);

    if (!s.eof())
	Error("ReadFromAsPureText", "something strange happended");
    return s;
}

int GapText::TextWidth(int from, int to)
{
    if (!CheckRange(length, from ,to))
	return 0;

    register int i, w= 0;
    register byte c;
    for (i= from; i < to; i++) {
	if ((c= CharAt(i)) == '\t') 
	    w+= Tabulate(w); 
	else
	    w+= font->Width(c);
    }
    return w;
}

void GapText::DrawText(int from, int to, Rectangle)
{
    if (!CheckRange(length, from ,to))
	return;

    Point start= GrGetTextPos();
    register int i;
    register byte c;
    SetDrawingState(from);
    for (i= from; i < to; i++)
	if ((c= CharAt(i)) == '\t') 
	    GrTextAdvance(Tabulate(GrGetTextPos().x-start.x)); 
	else
	    GrDrawChar(c);
} 

//---- GapTextIter -------------------------------------------------------------

GapTextIter::GapTextIter(Text *s, int from, int to) : TextIter(s, from, to)
{
    if (!s->IsKindOf(GapText))
	Error("GapTextIter::Error", "GapText expected (%s received)", s->ClassName());
    font= s->GetFont();
} 

int GapTextIter::operator()()
{ 
    return (ce == upto ? cEOT : ((GapText*)ct)->CharAt(ce++)); 
}

int GapTextIter::operator()(int *w, LineDesc *ld)  
{
    register GapTextPtr pt= GapTextPtr(ct);
    int ch;

    if (ld) 
	ld->FromFont(font);
    *w= 0;
    if (ce == upto )
	return cEOT;
    ch= GapTextPtr(ct)->CharAt(ce++);
    *w= font->Width(ch);
    return ch;
}

int GapTextIter::Line(LineDesc *ld)
{
    register GapTextPtr pt = GapTextPtr(ct);
    if (ld) 
	ld->FromFont(font);
    unget= ce;
    if (ce == upto) {
	// special case if last line is empty
	int last= GapTextPtr(ct)->CharAt(ce-1);
	ce++;
	if (last == '\n' || last == '\r')
	    return upto;
    }
    if (ce > upto)
	return cEOT;

    while (ce < upto) {
	int ch= GapTextPtr(ct)->CharAt(ce++);
	if (ch == '\n' || ch == '\r')
	    break;
    }
    return ce;
}

int GapTextIter::Token(int *w, LineDesc *ld)
{
    unget= ce;
    register GapTextPtr pt= GapTextPtr(ct);

    *w= 0;
    if (ld) 
	ld->FromFont(font);

    if (ce >= upto)
	return cEOT;

    register int ch= pt->CharAt(ce);
    if (Isspace(ch)) {
	*w= font->Width(ch);
	ce++;
	return ch;
    }
    while (ce < upto && !Isspace(pt->CharAt(ce))) {
	ch= pt->CharAt(ce++);
	*w += font->Width(ch);
    }

    return ch;
}
