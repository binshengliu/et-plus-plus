//$Picture$
#include "Picture.h"
#include "String.h"
#include "ObjArray.h"

const byte cPicStart        = 128+64,
	   cPicEnd          = cPicStart + 1,
	   cPicFont         = cPicStart + 2,
	   cPicPattern      = cPicStart + 3,
	   cPicExtent       = cPicStart + 4,
	   cPicStartAngle   = cPicStart + 7,
	   cPicEndAngle     = cPicStart + 8,
	   cPicMove         = cPicStart + 9,
	   cPicXMove        = cPicStart + 10,
	   cPicYMove        = cPicStart + 11,
	   cPicxMove        = cPicStart + 12,
	   cPicyMove        = cPicStart + 13,
	   cPicPensize      = cPicStart + 14,
	   cPicCornerDia    = cPicStart + 15,
	   cPicPenCap       = cPicStart + 16,
	   cPicLine         = cPicStart + 17,
	   cPicStrokeRect   = cPicStart + 18,
	   cPicStrokeRRect  = cPicStart + 19,
	   cPicStrokeOval   = cPicStart + 20,
	   cPicStrokeWedge  = cPicStart + 21,
	   cPicFillRect     = cPicStart + 22,
	   cPicFillOval     = cPicStart + 23,
	   cPicFillRRect    = cPicStart + 24,
	   cPicFillWedge    = cPicStart + 25,
	   cPicComment      = cPicStart + 26,
	   cPicEsc          = cPicStart + 27,
	   cPicShowBitmap   = cPicStart + 28;

byte *bufp;
static byte *pagebuf;
static Point lastpos, lastdia, lastextent;
static Font *lastfont;
static int lastpsz, laststartangle, lastendangle;
static GrLineCap lastcap;

MetaImpl(Picture, (T(length), T(bmcnt), T(ftcnt), TP(bms), T(bbox), 0));

Picture::Picture()
{
    Init();
}

Picture::Picture(char *name)
{
    Init();
    istream is(name);
    ReadFrom(is);
}

Picture::~Picture()
{
    SafeDelete(buf);
    SafeDelete(bms);
}

void Picture::Init()
{
    buf= 0;
    bufp= pagebuf= new byte[10000];
    lastfont= 0;
    lastextent= lastpos= gPoint0;
    lastdia= gPoint0;
    lastpsz= 1;
    laststartangle= lastendangle= 0;
    lastcap= eDefaultCap;
    bmcnt= ftcnt= 0;
}

void Picture::Close()
{
    if (pagebuf) {
	PutCode(cPicEnd);
	length= bufp-pagebuf;
	buf= 0;
	if (length > 0) {
	    buf= new byte[length];
	    BCOPY(pagebuf, buf, length);
	}
	SafeDelete(pagebuf);
    }
}

ostream &Picture::PrintOn(ostream &os)
{
    Object::PrintOn(os);
    os << bbox SP << bms SP << ftcnt SP;
    for (int i= 0; i < ftcnt; i++)
	os << fts[i];
    return PrintString(os, buf, length);
}

istream &Picture::ReadFrom(istream &is)
{
    Object::ReadFrom(is);
    is >> bbox >> bms >> ftcnt;
    for (int i= 0; i < ftcnt; i++)
	is >> fts[i];
    return ReadString(is, &buf, &length);
}

short Picture::GetShort()
{
    int high= GetByte();
    return (high << 8) + GetByte();
}

Point Picture::GetPoint()
{
    Point p;
    p.x= GetShort();
    p.y= GetShort();
    return p;
}

void Picture::Show(Rectangle *rp, register Port *port)
{
    register byte b;
    Rectangle r;
    Ink *pat= gInkBlack;
    GrLineCap cap= eDefaultCap;
    Font *font= gSysFont;
    int psz= 1, startangle= 0, endangle= 0;
    Point dia, p;
    
    bp= buf;
    r.origin= rp->origin - bbox.origin;
    while (1) {
	switch (b= GetByte()) {
	case cPicStart:
	    break;
	case cPicEnd:
	    return;
	case cPicFont:
	    font= fts[GetByte()];
	    break;
	case cPicPattern:
	    // pat= GetByte();
	    GetByte();
	    break;
	case cPicExtent:
	    r.extent= GetPoint();
	    break;
	case cPicStartAngle:
	    startangle= GetShort();
	    break;
	case cPicEndAngle:
	    endangle= GetShort();
	    break;
	case cPicMove:
	    r.origin+= GetPoint();
	    break;
	case cPicXMove:
	    r.origin.x+= GetShort();
	    break;
	case cPicYMove:
	    r.origin.y+= GetShort();
	    break;
	case cPicxMove:
	    r.origin.x+= GetChar();
	    break;
	case cPicyMove:
	    r.origin.y+= GetChar();
	    break;
	case cPicPensize:
	    psz= GetByte();
	    break;
	case cPicCornerDia:
	    dia= GetPoint();
	    break;
	case cPicPenCap:
	    cap= (GrLineCap) GetByte();
	    break;
	case cPicLine:
	    p= r.origin+GetPoint();
	    port->StrokeLine(pat, psz, cap, r.origin, p);
	    r.origin= p;
	    break;
	case cPicStrokeRect:
	    port->StrokeRect(pat, psz, &r);
	    break;
	case cPicStrokeRRect:
	    port->StrokeRRect(pat, psz, &r, dia);
	    break;
	case cPicStrokeOval:
	    port->StrokeOval(pat, psz, &r);
	    break;
	case cPicStrokeWedge:
	    port->StrokeWedge(pat, psz, cap, &r, startangle, endangle);
	    break;
	case cPicFillRect:
	    port->FillRect(pat, &r);
	    break;
	case cPicFillOval:
	    port->FillOval(pat, &r);
	    break;
	case cPicFillRRect:
	    port->FillRRect(pat, &r, dia);
	    break;
	case cPicFillWedge:
	    port->FillWedge(pat, &r, startangle, endangle);
	    break;
	case cPicShowBitmap:
	    port->ShowBitmap(pat, &r, (Bitmap*) bms->At(GetByte()));
	    break;
	case cPicComment:
	    break;
	case cPicEsc:
	    r.origin.x+= port->ShowChar(font, pat, r.origin, GetByte());
	    break;
	default:
	    if (b >= 128 && b < 128+64)
		r.origin.x+= b-128-32;
	    else
		r.origin.x+= port->ShowChar(font, pat, r.origin, b);
	    break;
	}
    }
}

void Picture::Goto(Point pos)
{
    Point delta= pos-lastpos;
    lastpos= pos;
    if (delta.x && delta.y) {
	PutCode(cPicMove);
	PutPoint(delta);
	return;
    }
    if (delta.x) {
	if (delta.x >= -32 && delta.x < 32) {
	    PutCode(128+32+delta.x);
	} else if (delta.x >= -128 && delta.x < 128) {
	    PutCode(cPicxMove);
	    PutByte(delta.x);
	} else {
	    PutCode(cPicXMove);
	    PutShort(delta.x);
	}
    }
    if (delta.y) {
	if (delta.y >= -128 && delta.y < 128) {
	    PutCode(cPicyMove);
	    PutByte(delta.y);
	} else {
	    PutCode(cPicYMove);
	    PutShort(delta.y);
	}
    }
}

void Picture::PutGlyph(int c, Point pos, Font *fp)
{
    Goto(pos);
    PutFont(fp);
    if (c >= 128)   // need escape
	PutCode(cPicEsc);
    PutChar(c);
    lastpos.x+= fp->Width(c);
}

void Picture::PutExtent(Point e)
{
    if (lastextent != e) {
	PutCode(cPicExtent);
	PutPoint(e);
	lastextent= e;
    }
}

void Picture::StrokeLine(int psz, GrLineCap cap, Point p1, Point p2)
{
    bbox.Merge(NormRect(p1, p2));
    p2-= p1;
    PutSize(psz);
    PutCap(cap);
    Goto(p1);
    PutCode(cPicLine);
    PutPoint(p2);
    lastpos+= p2;
}

void Picture::DrawObject(char type, Rectangle *r, Point e, int psz, GrLineCap)
{
    bbox.Merge(*r);
    if (psz >= 0)
	PutSize(psz);
    Goto(r->origin);
    PutExtent(r->extent);
    switch (type) {
    case 'b':
	PutCode(cPicStrokeRect);
	break;
    case 'B':
	PutCode(cPicFillRect);
	break;
    case 'o':
	PutCode(cPicStrokeOval);
	break;
    case 'O':
	PutCode(cPicFillOval);
	break;
    case 'r':
	PutDia(e);
	PutCode(cPicStrokeRRect);
	break;
    case 'R':
	PutDia(e);
	PutCode(cPicFillRRect);
	break;
    case 'w':
	PutAngle(e.x, e.y);
	PutCode(cPicStrokeWedge);
	break;
    case 'W':
	PutAngle(e.x, e.y);
	PutCode(cPicFillWedge);
	break;
    }
}

void Picture::PutSize(int s)
{
    if (lastpsz != s) {
	PutCode(cPicPensize);
	PutByte(s);
	lastpsz= s;
    }
}

void Picture::PutCap(GrLineCap cap)
{
    if (cap != lastcap) {
	PutCode(cPicPenCap);
	PutByte(cap);
	lastcap= cap;
    }
}

void Picture::PutPat(int pid)
{
    PutCode(cPicPattern);
    PutByte(pid);
}

void Picture::PutAngle(int s, int e)
{
    if (s != laststartangle) {
	PutCode(cPicStartAngle);
	PutShort(s);
	laststartangle= s;
    }
    if (e != lastendangle) {
	PutCode(cPicEndAngle);
	PutShort(e);
	lastendangle= e;
    }
}

void Picture::PutDia(Point e)
{
    if (e != lastdia) {
	PutCode(cPicCornerDia);
	PutPoint(e);
	lastdia= e;
    }
}

void Picture::PutBitmap(Rectangle *r, Bitmap *bmp)
{
    int ix;
    
    bbox.Merge(*r);
    Goto(r->origin);
    PutExtent(r->extent);
    PutCode(cPicShowBitmap);
    if (bms == 0)
	bms= new ObjArray;
    if ((ix= bms->IndexOfPtr(bmp)) < 0) {
	bms->Add(bmp);
	ix= bms->IndexOfPtr(bmp);
    }
    PutByte(ix);
}

void Picture::PutFont(Font *fp)
{
    if (lastfont == fp)
	return;
    lastfont= fp;
    PutCode(cPicFont);
    for (int i= 0; i < ftcnt; i++)
	if (fts[i] == fp)
	    break;
    if (i >= ftcnt)
	fts[ftcnt++]= fp;
    PutByte(i);
}


