//$StyledText,StyledTextIter,Style,StyleTable,TextRunArray$

#include "StyledText.h"
#include "RunArray.h"
#include "Mark.h"
#include "Port.h"
#include "Error.h"
#include "ObjectTable.h"

//------ StyleSpec ------------------------------------------------------

StyleSpec::StyleSpec()
{ 
    font= eFontChicago; 
    face= eFacePlain; 
    size= 12; 
    ink= ePatBlack; 
    xor= TRUE; 
}
    
StyleSpec::StyleSpec(Font *fp, GrPattern c, bool mode)
{ 
    font= fp->Fid(); 
    face= fp->Face(); 
    size= fp->Size(); 
    ink= c; 
    xor= mode; 
}

StyleSpec::StyleSpec(GrFont ft, GrFace fc, int sz, GrPattern c, bool mode)
{ 
    font= ft; 
    face= fc; 
    size= sz; 
    ink= c; 
    xor= mode; 
}

//------ StyleTable ------------------------------------------------------

class StyleTable: public OrdCollection {
public:
    MetaDef(StyleTable);
    StyleTable() : OrdCollection(cCollectionInitCap)
	{ ObjectTable::AddRoot(this); }
    Style *MakeStyle(GrFont ft, GrFace fc, int size, GrPattern ink);
    Style *MakeStyle(Style *);
};

static StyleTable *Styles;

MetaImpl0(StyleTable); 

Style *StyleTable::MakeStyle(GrFont ft, GrFace fc, int size, GrPattern ink)
{
    Iter next(this);
    register Style *sp;
    while (sp= (Style*)next()) {
	register Font *f= sp->fp;
	if (f->Fid() == ft && f->Size() == size && f->Face() == fc &&
	    sp->ink == ink) 
	    return sp;
    }
    sp= new_Style(0, new_Font(ft, size, fc), ink);
    Add(sp);
    return sp;
}

Style *StyleTable::MakeStyle(Style *asp)
{
    Iter next(this);
    Style *sp;
    while (sp= (Style*)next()) 
	if (sp->IsEqual(asp))
	    return sp;
    Add(asp);
    return asp;
}

//------ Style -----------------------------------------------------------

MetaImpl(Style, (TP(fp), TP(ink), 0)); 

Style::Style(int, FontPtr f, GrPattern c)
{
    fp= f;  
    ink= c;  
}

Style *new_Style(int i, FontPtr f, GrPattern c)
{
    return new Style(i, f, c);
}

Style *new_Style(FontPtr f, GrPattern c)
{
    if (Styles == 0)
	Styles= new StyleTable;
    return Styles->MakeStyle(f->Fid(), f->Face(), f->Size(), c);
}

Style *new_Style(GrFont ft, GrFace fc, int size, GrPattern ink)
{
    if (Styles == 0)
	Styles= new StyleTable;
    return Styles->MakeStyle(ft, fc, size, ink);
}

Style::~Style()
{ 
    this= 0;
}

ostream &Style::PrintOn(ostream &s) 
{    
    return s << fp SP << ink SP;
}

istream &Style::ReadFrom(istream &s) 
{ 
    char c;
    
    s >> fp;
    s.eatwhite();
    s.get(c);
    s.putback(c);
    if (c != '}')       // new Style with ink
	s >> ink;
    else                // old Style without ink
	ink= ePatBlack;
    return s;
}

ostream &Style::DisplayOn(ostream &s) 
{
    return s << fp->AsString() NL;
}

bool Style::IsEqual(Object* op)
{
    if (!op->IsKindOf(Style))
	return FALSE;

    Style *sp= (Style*)op;
    register Font *f= sp->fp;
    return (f->Fid() == fp->Fid() && 
	    f->Size() == fp->Size() && 
	    f->Face() == fp->Face() &&
	    ink == sp->ink);
}

void Style::GetStyle(GrFont *fid, GrFace *face, int *size, GrPattern *col)
{
    *fid= fp->Fid();
    *face= fp->Face();
    *size= fp->Size();
    *col= ink;
}

ObjPtr Style::DeepClone()
{
    return this;
}

//---- class TextRunArray --------------------------------------------------

MetaImpl(TextRunArray, (TP(st), 0)); 

TextRunArray::TextRunArray(StyledText *sp, int elements) : RunArray(elements)
{
    st= sp;
}

ObjPtr TextRunArray::RunAt(int i, int *start, int *end, int *size ,int *lenat)
{
    // requests for styles beyond the end of the text receive the
    // same style information as at the end of the text
    int at= i;
    i= min(i, Size()-1);
    if (i < 0) { // take styleHere into account
	*start= *end= at;
	*size= *lenat= 0;
	return (ObjPtr)st->GetCurrentStyle();
    }
    return RunArray::RunAt(i, start, end, size, lenat);
}

ostream &TextRunArray::PrintOn(ostream &s) 
{
    RunArray::PrintOn(s);
    return s << st SP;
}

istream &TextRunArray::ReadFrom(istream &s) 
{ 
    RunArray::ReadFrom(s);
    return s >> st;
}

Object *TextRunArray::ReadItem(istream& s)
{
    Style *sp;
    s >> sp;
    if (Styles == 0)
	Styles= new StyleTable;
    sp= Styles->MakeStyle(sp);
    return sp;    
}

//----- class StyledTextIter ---------------------------------------------

StyledTextIter::StyledTextIter(Text *s, int from, int to) 
						      : GapTextIter(s, from, to)
{
    if (!s->IsKindOf(StyledText))
	Error("StyledTextIter::StyledTextIter", "StyledText expected (%s received)",
							    s->ClassName());
    nextFontChange= ce;
    NextFontChange();
    if (sp == 0) {
	sp= new_Style(gSysFont);
	Error("StyledTextIter::StyledTextIter", "style is nil");
    }
}

void StyledTextIter::Reset(Text *s, int from, int to)
{ 
    if (!s->IsKindOf(StyledText))
	Error("StyledTextIter::Reset", "StyledText expected (%s received)",
								s->ClassName());
    GapTextIter::Reset(s,from,to);
    nextFontChange= ce;
    NextFontChange();
    if (sp == 0) {
	sp= new_Style(gSysFont);
	Error("StyledTextIter::Reset", "style is nil");
    }
}

int StyledTextIter::operator()()  
{
    return GapTextIter::operator()();
}

int StyledTextIter::Line(LineDesc *ld)
{
    register StyledText *pt = (StyledText*)ct;
    unget = ce;
    NextFontChange();
    if (ld) 
	ld->FromFont(sp->GetStyle());

    if (ce == upto) { // special case if last line is empty
	int last = pt->CharAt(ce-1);
	ce++;
	if (last == '\n' || last == '\r')
	    return upto;
    }
    if (ce > upto)
	return cEOT;

    while (ce < upto) {
	int ch = pt->CharAt(ce);
	if (NextFontChange() && ld)
	    ld->Max(sp->GetStyle());
	if (DoEscape(ce,ch)) 
	    pt->CalcEscape(ce,ld);
	ce++;
	if (ch == '\n' || ch == '\r')
	    break;
    }
    return ce;
}

int StyledTextIter::operator()(int *w, LineDesc *ld)  
{
    register StyledText *pt = (StyledText*)ct;

    *w = 0;
    if (ce == upto )
	return cEOT;

    NextFontChange();
    int ch = CharAt(ce);
    if (DoEscape(ce,ch))
	*w = pt->CalcEscape(ce,ld);
    else {      
	*w = sp->GetStyle()->Width(ch);
	if (ld)
	    ld->FromFont(sp->GetStyle());
    }
    ce++;
    return ch;
}

int StyledTextIter::Token(int *w,LineDesc *ld)  
{
    register StyledText *pt = (StyledText*)ct;

    unget= ce;
    *w = 0;
    NextFontChange();
    if (ld) 
	ld->FromFont(sp->GetStyle());
    if (ce >= upto)
	return cEOT;
    register int ch= CharAt(ce);
    if (Isspace(ch)) {
	*w = sp->GetStyle()->Width(ch);
	ce++;
	return (ch);
    } else if (DoEscape(ce,ch)) {
	*w = pt->CalcEscape(ce,ld);
	ce++;
	return (ch);
    }
    while (ce < upto && !Isspace(CharAt(ce)) && !DoEscape(ce,CharAt(ce))) {
	if (NextFontChange() && ld)
	    ld->Max(sp->GetStyle());
	ch = CharAt(ce++);
	*w += sp->GetStyle()->Width(ch);
    }
    return (ch);
}

bool StyledTextIter::NextFontChange ()
{ 
    if (ce == nextFontChange) {
	sp = (Style*)  ((StyledText*)ct)->styles->RunAt(ce,&start,&end,&size,&lenat);
	nextFontChange = ce + lenat;
	return TRUE;
    }
    else
	return FALSE;
}

//----- class StyledText --------------------------------------------------

MetaImpl(StyledText, (TP(styles), TP(sp), TP(styleHere), T(applyStyleHere),
    /* T(escape), */ T(nextFontChange), T(start), T(end), 
    T(size), T(lenat), 0));    

static TextChanges changeRec;

StyledText::StyledText()
{
    styles= new TextRunArray(this);
    Init(gSysFont, TRUE);
}

StyledText::StyledText(int size, FontPtr fd) : GapText(size, fd)
{
    styles= new TextRunArray(this);
    Init(fd, TRUE);
}

StyledText::StyledText(byte *buf, int len, bool ic, FontPtr fd)  
						     : GapText(buf, len, ic, fd)
{
    styles= new TextRunArray(this);

    if (Size()) {
	styles->Insert(new_Style(fd), 0, 0, Size());
	Init(gSysFont, FALSE);
    } else 
	Init(fd, TRUE);
}

StyledText::StyledText(TextRunArray *st, byte *buf, int len, bool ic)
							: GapText(buf, len, ic)
{
    if (st->Size() != Size())
	Error("StyledText", "StyleArray does not correspond to the text");
    styles= st;
    Init(gSysFont, FALSE);
}

StyledText::StyledText(FontPtr fd, char* va_(fmt), ...)
{
    Style *st= new_Style(fd);
    char *buf;

    va_list ap;
    va_start(ap,va_(fmt));
    buf= strvprintf(va_(fmt), ap);
    va_end(ap);
    BuildStylesFromString(st, buf);
    Init(fd, FALSE);
    SafeDelete(buf);
}

StyledText::~StyledText()
{
    SafeDelete(styles);
    SafeDelete(styleHere);
}

void StyledText::InitNew()
{
    GapText::InitNew();
    styles= new TextRunArray(this);
    Init(gSysFont, TRUE);
}

void StyledText::Init(FontPtr fd, bool nullStyle)
{
    styleHere= new_Style(fd);
    if (nullStyle)
	applyStyleHere= nullStyle;
    escape= '0';
}

void StyledText::ReplaceWithStr(byte *str,int len)
{
    GapText::ReplaceWithStr(str, len);
    SafeDelete(styles);
    Init(GetFont(), FALSE);   
}

void StyledText::Cut(int from,int to)
{
    int start, end, size, lenat;
    DoDelayChanges dc(this);
    // if we delete a complete run |xxxx|XXXXXXX
    //                             ^         ^ we will set styleHere

    Style *sp= (Style*)styles->RunAt(from,&start,&end,&size,&lenat);
    applyStyleHere = FALSE;
    if (start == from && size <= to - from) {
	applyStyleHere = TRUE;
	styleHere = sp;
    }
    GapText::Cut(from,to);
    styles->Cut(from,to,FALSE);
}

void StyledText::Paste(Text* t,int from,int to)
{
    DoDelayChanges dc(this);
    GapText::Paste(t,from,to);
    if (t->IsKindOf(StyledText)) { 
	StyledText *st = (StyledText *)t;
	if (styles)
	    styles->Paste(st->styles, from, to, FALSE);
	else
	    Error("Paste", "styles == 0");
    }  else {
	int start, end, size, lenat;
	if (from != to) {
	    Style *sp= (Style*)styles->RunAt(from, &start, &end, &size, &lenat);
	    if (start == from && size <= to - from) {
		applyStyleHere= TRUE;
		styleHere= sp;
	    }
	    styles->Cut(from,to,FALSE);
	}
	if (applyStyleHere && t->Size()) { // insert new run with styleHere
	    //styles->Insert (new_Style(styleHere->GetStyle()), from, from, t->Size());
	    styles->Insert (styleHere,from ,from , t->Size());
	    applyStyleHere= FALSE;
	}
	else 
	    styles->ChangeRunSize(from, t->Size());
    }       
}

void StyledText::Copy(Text* save,int from, int to)
{
    if (!CheckRange(Size(),from,to) || save == 0)
	return;
    GapText::Copy(save,from,to);

    if (!save->IsKindOf(StyledText)) 
	return;

    StyledText *st = (StyledText *)save;
    if (applyStyleHere && Size() == 0) {
	st->applyStyleHere = TRUE;
	st->styleHere = styleHere;
    } else 
	styles->Copy(st->styles,from,to);
}

TextPtr StyledText::Save(int from, int to)
{
    if (!CheckRange(Size(),from,to))
	return 0;

    StyledText* t= new StyledText(to-from);
    Copy(t, from, to);
    return t;
}

void StyledText::SetFStringVL(char *fmt, va_list ap)
{
    char *buf= strvprintf(fmt, ap);
    FontPtr fd= GetFont();
    Empty();
    Style *st= GetStyle(0);
    BuildStylesFromString(st, buf);
    SafeDelete(buf);
    Init(fd, FALSE);
}

void StyledText::Insert(byte c, int from,int to)
{
    DoDelayChanges dc(this);
    GapText::Insert(c,from,to);
    if (applyStyleHere)  { // insert new run with styleHere 
	styles->Insert(new_Style(styleHere->GetStyle()), from, from, 1);
	applyStyleHere= FALSE;
    } else 
	styles->ChangeRunSize(from,1);
}

Text *StyledText::GetScratchText(int size)
{
    return new GapText(size);
}

void StyledText::DrawText(int from, int to, Rectangle clip)
{
    if (!CheckRange(Size(), from ,to))
	return;
    int ch;
    nextFontChange= from;
    NextFontChange(from);
    GrSetFont(sp->GetStyle());
    GrSetTextPattern(sp->GetInk());

    Point start= GrGetTextPos();
    for (int i= from; i < to; i++) {
	if (NextFontChange(i)) {
	    GrSetFont(sp->GetStyle());
	    GrSetTextPattern(sp->GetInk());
	}
	if ((ch= CharAt(i)) == '\t') 
	    GrTextAdvance(Tabulate(GrGetTextPos().x-start.x)); 
	else if (DoEscape(i,ch)) {
	    DrawEscape(i,clip);
	    NextFontChange(nextFontChange = i); // reload current style
	    GrSetFont(sp->GetStyle());
	    GrSetTextPattern(sp->GetInk());
	} else
	    GrDrawChar(ch);
    }
}

void StyledText::DrawTextJust (int from, int to, int w, Point start, Rectangle r)
{
    register byte ch;
    int ntab, longBlanks, addSpace, seenTabs;

    if (!CheckRange(Size(), from, to))
	return;

    ntab= longBlanks= seenTabs = 0;

    CalcIws(w, from, &to, &addSpace, &longBlanks, &ntab);    
    GrTextMoveto(start);
    NextFontChange(nextFontChange= from);
    GrSetFont(sp->GetStyle());
    GrSetTextPattern(sp->GetInk());
    for (int i = from, nBlanks = 0; i < to; i++) {
	if (NextFontChange(i)) {
	    GrSetFont(sp->GetStyle());
	    GrSetTextPattern(sp->GetInk());
	}
	switch (ch = CharAt(i)) {
	case '\t':
	    GrTextAdvance(Tabulate(GrGetTextPos().x-start.x));
	    seenTabs++;
	    break;
	case ' ':
	    GrTextAdvance(sp->GetStyle()->Width(' '));
	    if (seenTabs == ntab) {
		GrTextAdvance(addSpace);
		if (nBlanks < longBlanks)
		    GrTextAdvance(1);
		nBlanks++;
	    }
	    break;
	default:
	    if (DoEscape(i,ch)) {
		DrawEscape(i,r);
		NextFontChange(nextFontChange = i); // reload current style
		GrSetFont(sp->GetStyle());
		GrSetTextPattern(sp->GetInk());
	    }
	    else
		GrDrawChar (CharAt(i));
	}
    }
}

void StyledText::JustifiedMap(int from, int to, int w, int stopAt, int posX, 
							   int *charPos, int *x)
{
    register byte ch;
    int ntab, longBlanks, addSpace, seenTabs, cx, wx;

    if (!CheckRange(Size(), from, to))
	return;

    wx= cx= ntab= longBlanks= addSpace= seenTabs= 0;

    CalcIws(w, from, &to, &addSpace, &longBlanks, &ntab);    
    nextFontChange = from;

    for (int i = from, nBlanks = 0; i < to && i < stopAt; i++) {
	NextFontChange(i); 
	switch (ch = CharAt(i)) {
	case '\t':
	    wx = Tabulate(cx);
	    seenTabs++;
	    break;
	case ' ':
	    wx= sp->GetStyle()->Width(' ');
	    if (seenTabs == ntab) {
		wx += addSpace;
		if (nBlanks < longBlanks)
		    wx++; 
		nBlanks++;
	    }
	    break;
	default:
	    if (DoEscape(i,ch)) 
		wx = CalcEscape(i);
	    else 
		wx = sp->GetStyle()->Width(ch); 
	    break;
	}
	if (cx + (wx/2) > posX)
	    break;
	cx += wx;
    }
    if (x)
	*x = cx;
    if (charPos)
	*charPos= i;
}

int StyledText::TextWidth(int from, int to)
{
    if (!CheckRange(Size(), from ,to))
	return 0;

    int w= 0, ch;

    nextFontChange= from;
    NextFontChange(from);

    for (int i= from; i < to; i++) {
	NextFontChange(i);
	ch= CharAt(i);
	if (ch == '\t') 
	    w+= Tabulate(w); 
	else {
	    if (DoEscape(i,ch))
		w+= CalcEscape(i);
	    else 
		w+= sp->GetStyle()->Width (ch);
	}
    }
    return w;
}

TextIter *StyledText::MakeIterator(int from, int to)
{ 
    return new StyledTextIter(this, from, to); 
}

ostream &StyledText::PrintOn(ostream &s) 
{
    GapText::PrintOn(s);
    return s << styles SP << (int)escape SP;
}

istream &StyledText::ReadFrom(istream &s) 
{
    int esc;

    GapText::ReadFrom(s);
    s >> styles >> esc;
    escape= esc;
    applyStyleHere= (Size() == 0);
    return s;
}

istream& StyledText::ReadFromAsPureText(istream &s, long sizeHint)
{
    GapText::ReadFromAsPureText(s, sizeHint);
    applyStyleHere= (Size() == 0);
    styleHere= new_Style(GapText::GetFont(0));
    styles->Insert(new_Style(GapText::GetFont(0)), 0, 0, Size());
    return s;
}

void StyledText::SetStyle(StChangeStyle mode, int from, int to, StyleSpec st)
{
    int start, end, size;

    if (from == to) {
	Style *sp= applyStyleHere ? styleHere : GetStyle(max(0, from-1));
	applyStyleHere= TRUE;
	styleHere= ApplySpec(sp, st, mode);
    } else {
	Style **spp;
	RunArray newStyles(2); 
	styles->Copy(&newStyles, from, to);
	RunArrayIter next(&newStyles);
	while (spp= (Style**)next.RunPtr(&start, &end, &size)) 
	    *spp= ApplySpec(*spp, st, mode);
	styles->Paste(&newStyles, from, to, FALSE);
	GetMarkList()->RangeChanged(from, to - from);
	Send(cIdNone, eTextChangedRange, changeRec(from, to));     
    }
}
  
Style *StyledText::ApplySpec(Style *ostyle, StyleSpec st, StChangeStyle mode)
{
    GrFont fid;
    GrFace face;
    int    fsize;
    GrPattern fink;
    
    ostyle->GetStyle (&fid, &face, &fsize, &fink);
    if ((mode & eStFont) == eStFont)
	fid= st.font;
    if ((mode & eStFace) == eStFace)
	if (st.face == eFacePlain)
	    face= st.face;
	else {
	    if (st.xor)
		face= (GrFace)(face ^ st.face);
	    else
		face= (GrFace)(face | st.face);
	}
    if ((mode & eStSize) == eStSize)
	fsize= st.size;
    if ((mode & eStAddSize) == eStAddSize)
	fsize+= st.size;
    if ((mode & eStInk) == eStInk)
	fink= st.ink;
    if (Styles == 0)
	Styles= new StyleTable;
    return Styles->MakeStyle(fid, face, fsize, fink);
}

void StyledText::BuildStylesFromString(Style *st, char *buf)
{
    GrFace face= eFacePlain;
    GrFont fid; 
    int run= 0, size;
    GrPattern ink;
    char *p;

    styles = new TextRunArray(this);
    for (p= buf; *p; *p++) {
	if (*p == '@') {
	    if (*(p+1) && index("OUSBIP",*(p+1))) {
	       if (run) {
		   st->GetStyle(&fid, &face, &size, &ink);
		   int sz= styles->Size();
		   styles->Insert(st->DeepClone(), sz, sz, run);
		   run = 0;
		}
		p++;
		switch (*p) {
		case 'P':
		    face= eFacePlain;
		    break;
		case 'O':
		    face= (GrFace) (face ^ eFaceOutline);
		    break;
		case 'S':
		    face= (GrFace) (face ^ eFaceShadow);
		    break;
		case 'U':
		    face= (GrFace) (face ^ eFaceUnderline);
		    break;
		case 'B':
		    face= (GrFace) (face ^ eFaceBold);
		    break;
		case 'I':
		    face= (GrFace) (face ^ eFaceItalic);
		    break;
		}
		st= new_Style(fid, face, size);
	     } else { 
		 p++;
		 if (!*p)
		     break;
		 GapText::Insert(*p, Size(), Size());
		 run++;
	     }
	} else {
	    GapText::Insert(*p, Size(), Size());
	    run++;
	}
    }
    if (run) {
	int sz= styles->Size();
	styles->Insert(st->DeepClone(), sz, sz, run);
    }
}

Style *StyledText::GetStyle(int at)
{
    int dummy;
    return (Style*) styles->RunAt(at, &dummy, &dummy, &dummy, &dummy);
}

void StyledText::SetFont(FontPtr fp)
{
    SetStyle(eStAll, 0, Size(), StyleSpec(fp));
}

FontPtr StyledText::GetFont(int at)
{
    Style *st = GetStyle(at);
    if (st)
	return st->GetStyle();
    else
	return GapText::GetFont(at);
}

void StyledText::ReplaceStyles(RunArray *st, int from, int to)
{
    if (to-from != st->Size())
	Error("ReplaceStyle", "RunArray does not match text");
    styles->Paste(st,from,to,FALSE);
    GetMarkList()->RangeChanged(from,to - from);
    Send(cIdNone, eTextChangedRange, changeRec(from, to));     
}

void StyledText::CopyStyles(RunArray *st,int from, int to)
{
    styles->Copy(st,from,to);
}

void StyledText::ResetCurrentStyle()
{
    if (Size())
	applyStyleHere = FALSE;
}

const Style *StyledText::GetCurrentStyle()
{
    if (applyStyleHere)
	return styleHere;
    return 0;    
}

TextRunArray *StyledText::GetStyles()
{
    return styles; 
}

TextRunArray *StyledText::SetStyles(TextRunArray *st)
{
    TextRunArray *old = styles;
    styles = st;
    return old;
}

void StyledText::SetEscapeChar(byte ch)
{
    escape = ch;
}

byte StyledText::GetEscapeChar()
{
    return escape;
}

int StyledText::CalcEscape (int, LineDesc*)
{
    return 0;
}

void StyledText::DrawEscape (int,Rectangle)
{
}

bool StyledText::IsEscape (int)
{
    return FALSE;
}

