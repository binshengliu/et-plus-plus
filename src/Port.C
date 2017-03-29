//$Port$
#include "Port.h"
#include "String.h"
#include "Error.h"
#include "Picture.h"
#include "WindowSystem.h"
#include "DevBitmap.h"

#define InstallInk(ink) ink->SetInk(this);

Port *port, *tbport;
bool fullscreen;

ImageCache::ImageCache()
{
    b= 0;
    invalid= TRUE;
}

ImageCache::~ImageCache()
{
    SafeDelete(b);
}

bool ImageCache::Open(Rectangle r)
{
    return ::port->DevImageCacheBegin(this, r);
}

void ImageCache::Close()
{
    ::port->DevImageCacheEnd(this);
}

/*
static int dither2[2][2] = {
	0,      2,
	3,      1
};

static int dither3[3][3] = {
	2,      6,      4,
	5,      0,      1,
	8,      3,      7
};

static int dither4[4][4] = {
	0,      8,      2,      10,
	12,     4,      14,     6,
	3,      11,     1,      9,
	15,     7,      13,     5
};

static int dither5[5][5] = {
	21,     10,     17,     14,     23,
	15,     2,      6,      4,      9,
	20,     5,      0,      1,      18,
	12,     8,      3,      7,      13,
	24,     16,     19,     11,     22
};
*/

const int sz= 8;
const int levels= sz*sz;

DevBitmap *patbms[levels];

static int dither8[sz][sz] = {
	0,      32,     8,      40,     2,      34,     10,     42,
	48,     16,     56,     24,     50,     18,     58,     26,
	12,     44,     4,      36,     14,     46,     6,      38,
	60,     28,     52,     20,     62,     30,     54,     22,
	3,      35,     11,     43,     1,      33,     9,      41,
	51,     19,     59,     27,     49,     17,     57,     25,
	15,     47,     7,      39,     13,     45,     5,      37,
	63,     31,     55,     23,     61,     29,     53,     21
};

void Port::SetColor(RGBColor *cp)
{
    if (HasColor())
	DevSetColor(cp);
    else
	DevSetGrey(cp->AsGreyLevel() / 255.0);
}

void Port::DevSetGrey(float f)
{
    int level;
    
    if (f < 0)
	f= 0.0;
    else if (f > 1.0)
	f= 1.0;
    f= 1.0 - f;
    
    level= (int)(f * levels + 0.5);
    if (level <= 0) {
	DevSetOther(1);
	return;
    }
    if (level >= levels) {
	DevSetOther(2);
	return;
    }
    if (patbms[level] == 0) {
	DevBitmap *bm= patbms[level]= gWindowSystem->MakeDevBitmap(sz, 0, 1);
	for (int y= 0; y < sz; y++) {
	    for (int x= 0; x < sz; x++)
		if (level > dither8[x][y])
		    bm->SetPixel(x, y, 1);
		else
		    bm->SetPixel(x, y, 0);
	}
    }
    DevSetPattern(patbms[level]);
}

//---- Port --------------------------------------------------------------------

MetaImpl0(Port);

Port::Port() : SysEvtHandler(-1)
{
    PortInit();
    hascolor= FALSE;
}

Port::~Port()
{
    if (port == this)
	port= 0;
    if (tbport == this)
	tbport= 0;
}

void Port::PortInit()
{
    cliprect= Rectangle(0, 0, 32000, 32000);
    origin= gPoint0; 
    SetNormal();
    SetPenNormal();
    SetTextNormal();
}

//---- abstract cursor methods -------------------------------------------------

GrCursor Port::SetCursor(GrCursor c)
{ 
    return c;
}

GrCursor Port::SetWaitCursor(unsigned int, GrCursor c)
{
    return SetCursor(c);
}

GrCursor Port::GetCursor()
{
    return eCrsNone;
}

//---- graphics context --------------------------------------------------------

void Port::SetPenNormal()
{
    pensize= 1;
    penink= gInkBlack;
    pencap= eDefaultCap;
}

void Port::SetTextNormal()
{
    textink= gInkBlack;
    textfont= gSysFont;
}

//---- clipping ----------------------------------------------------------------

void Port::Clip(Rectangle r, Point o)
{
    /*
    if (fullscreen) {
	r= Rectangle(0, 0, 32000, 32000);
	o= gPoint0;
    }
    */
    if (cliprect != r || origin != o) {
	FlushMyText();
	cliprect= r;
	origin= o;
	DevClip(cliprect, origin);
    }
}

//---- graphic primitives ------------------------------------------------------

void Port::StrokeLine(Ink *ink, int psz, GrLineCap cap, Point p1, Point p2)
{
    FlushMyText();
    p1+= origin;
    p2+= origin;
    Rectangle rr= NormRect(p1,p2).Expand(psz/2);
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevStrokeLine(psz, &rr, cap, p1, p2);
    }
}

void Port::StrokeRect(Ink *ink, int psz, Rectangle *r)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevStrokeRect(psz, &rr);
    }
}

void Port::FillRect(Ink *ink, Rectangle *r)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevFillRect(&rr);
    }
}

void Port::StrokeOval(Ink *ink, int psz, Rectangle *r)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevStrokeOval(psz, &rr);
    }
}

void Port::FillOval(Ink *ink, Rectangle *r)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevFillOval(&rr);
    }
}

void Port::StrokeRRect(Ink *ink, int psz, Rectangle *r, Point dia)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevStrokeRRect(psz, &rr, dia);
    }
}

void Port::FillRRect(Ink *ink, Rectangle *r, Point dia)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevFillRRect(&rr, dia);
    }
}

void Port::StrokeWedge(Ink *ink, int psz, GrLineCap cap, Rectangle *r,
								int s, int l)
{
    if (l >= 360) {
	StrokeOval(ink, psz, r);
	return;
    }
    Rectangle rr= *r+origin;
    while (s < 0)
	s+= 360;
    while (s >= 360)
	s-= 360;
    FlushMyText();
    if (Visible(ink, rr.WedgeBBox(s, l))) {
	InstallInk(ink);
	DevStrokeWedge(psz, cap, &rr, s, l);
    }
}

void Port::FillWedge(Ink *ink, Rectangle *r, int s, int l)
{
    if (l >= 360) {
	FillOval(ink, r);
	return;
    }
    Rectangle rr= *r+origin;
    while (s < 0)
	s+= 360;
    while (s >= 360)
	s-= 360;
    FlushMyText();
    if (Visible(ink, rr.WedgeBBox(s, l))) {
	InstallInk(ink);
	DevFillWedge(&rr, s, l);
    }
}

void Port::StrokePolygon(Point at, Ink *ink, Point *pts,
				int npts, GrPolyType t, int psz, GrLineCap cap)
{
    if (pts && npts > 0) {
	Point *p= (Point*) Alloca(npts * sizeof(Point));
	
	FlushMyText();
	Rectangle r= BoundingBox(npts, pts, p);
	r.origin+= at+origin;
	if (Visible(ink, r)) {
	    InstallInk(ink);
	    DevStrokePolygon(&r, p, npts, t, psz, cap);
	}
	Freea(p);
    }
}

void Port::FillPolygon(Point at, Ink *ink, Point *pts, int npts,
					    GrPolyType t)
{
    if (pts && npts > 0) {
	Point *p= (Point*) Alloca(npts * sizeof(Point));
	
	FlushMyText();
	Rectangle r= BoundingBox(npts, pts, p);
	r.origin+= at+origin;
	if (Visible(ink, r)) {
	    InstallInk(ink);
	    DevFillPolygon(&r, p, npts, t);
	}
	Freea(p);
    }
}

void Port::ShowBitmap(Ink *ink, Rectangle *r, Bitmap *bm)
{
    FlushMyText();
    Rectangle rr= *r+origin;
    if (Visible(ink, rr)) {
	InstallInk(ink);
	DevShowBitmap(&rr, bm);
    }
}

void Port::ShowPicture(Rectangle *r, Picture *pic)
{
    if (pic) {
	FlushMyText();
	Rectangle rr= *r+origin;
	//if (Visible(gInkBlack, rr)) {
	    DevShowPicture(r, pic);
	//}
    }
}

void Port::DevShowPicture(Rectangle *r, Picture *pic)
{
    pic->Show(r, this);
}

void Port::GiveHint(int code, int len, void *vp)
{
    Rectangle r;
    
    switch (code) {
    case eHintLock:
	break;
	
    case eHintUnlock:
	FlushMyText();
	break;

    case eHintTextBatch:
	break;
	
    case eHintTextUnbatch:
	FlushMyText();
	break;
	
    case eHintBatch:
	r= *((Rectangle*) vp);
	r.origin+= origin;
	DevGiveHint(code, len, &r);
	break;
    
    case eHintFlush:
	if (tbport)
	    tbport->flushtext();
    
    default:
	DevGiveHint(code, len, vp);
	break;
    }
}

//---- Lines -------------------------------------------------------------------

void Port::Lineto(Point p)
{
    StrokeLine(penink, pensize, pencap, penpos, p);
    penpos= p;
}

//---- Text ---------------------------------------------------------------------

void Port::SetFamily(GrFont fid)
{
    textfont= new_Font(fid, textfont->Size(), textfont->Face());
}

void Port::SetSize(int ps)
{
    textfont= new_Font(textfont->Fid(), ps, textfont->Face());
}

void Port::SetFace(GrFace face)
{
    textfont= new_Font(textfont->Fid(), textfont->Size(), face);
}

//---- text batch --------------------------------------------------------------

static Ink *tbpat;
static Point tbpos;
static Rectangle tbbbox;
static Point tblastpos;
static bool tbnew= TRUE;

bool Port::DevShowChar(FontPtr, Point, byte, bool, Point)
{
    return FALSE;
}

void Port::flushtext()
{
    InstallInk(tbpat);
    DevShowTextBatch(&tbbbox, tbpos+origin);  
    tbport= 0;
}

void Port::DrawChar(byte c)
{
    Rectangle rr;
    int w;

    rr.origin= textpos + origin;
    rr.origin.y-= textfont->Ascender();
    rr.extent.x= w= textfont->Width(c);
    rr.extent.y= textfont->Spacing();

    if (Visible(textink, rr)) {
	if (this != tbport || textink != tbpat)
	    FlushAnyText();
	
	if (tbport == 0) {
restart:
	    tbport= this;
	    tbpat= textink;
	    tblastpos= tbpos= textpos;
	    tbbbox= rr;
	    tbnew= TRUE;
	} else 
	    tbbbox.Merge(rr);
	if (DevShowChar(textfont, textpos-tblastpos, c, tbnew, textpos+origin)) {
	    FlushAnyText();
	    goto restart;
	}
	tbnew= FALSE;
	tblastpos= textpos;
	tblastpos.x+= w;
    }
    textpos.x+= w;
}

int Port::ShowChar(Font *fdp, Ink *ink, Point pos, byte c)
{
    Rectangle rr;
    int w;

    rr.origin= pos + origin;
    rr.origin.y-= fdp->Ascender();
    rr.extent.x= w= fdp->Width(c);
    rr.extent.y= fdp->Spacing();

    if (Visible(ink, rr)) {
	if (this != tbport || ink != tbpat)
	    FlushAnyText();
	
	if (tbport == 0) {
restart:
	    tbport= this;
	    tbpat= ink;
	    tblastpos= tbpos= pos;
	    tbbbox= rr;
	    tbnew= TRUE;
	} else 
	    tbbbox.Merge(rr);
	if (DevShowChar(fdp, pos-tblastpos, c, tbnew, pos+origin)) {
	    FlushAnyText();
	    goto restart;
	}
	tbnew= FALSE;
	tblastpos= pos;
	tblastpos.x+= w;
    }
    return w;
}

void Port::ShowString(Font *fdp, Ink *ink, Point pos, byte *s, int l)
{
    Rectangle rr;
    register int i, w;
    byte c;

    if (s == NULL)
	return;
	
    if (this != tbport || ink != tbpat)
	FlushAnyText();
	
    if (l < 0)
	l= strlen((char*)s);
	
    rr.origin= pos + origin;
    rr.origin.y-= fdp->Ascender();
    rr.extent.y= fdp->Spacing();

    for(i= 0; i < l; i++) {
	c= *s++;
	rr.extent.x= w= fdp->Width(c);
	if (Visible(ink, rr)) {
	    if (tbport == 0) {
    restart:
		tbport= this;
		tbpat= ink;
		tblastpos= tbpos= pos;
		tbbbox= rr;
		tbnew= TRUE;
	    } else
		tbbbox.Merge(rr);
	    if (DevShowChar(fdp, pos-tblastpos, c, tbnew, pos+origin)) {
		FlushAnyText();
		goto restart;
	    }
	    tbnew= FALSE;
	    tblastpos= pos;
	    tblastpos.x+= w;
	}
	rr.origin.x+= w;
	pos.x+= w;
    }
}

void Port::DrawString(byte *text, int l)
{
    Rectangle rr;
    register int i, w;
    byte c;

    if (text == NULL)
	return;
	
    if (this != tbport || textink != tbpat)
	FlushAnyText();
	
    if (l < 0)
	l= strlen((char*)text);
	
    rr.origin= textpos + origin;
    rr.origin.y-= textfont->Ascender();
    rr.extent.y= textfont->Spacing();

    for(i= 0; i < l; i++) {
	c= *text++;
	rr.extent.x= w= textfont->Width(c);
	if (Visible(textink, rr)) {
	    if (tbport == 0) {
restart:
		tbport= this;
		tbpat= textink;
		tblastpos= tbpos= textpos;
		tbbbox= rr;
		tbnew= TRUE;
	    } else
		tbbbox.Merge(rr);
	    if (DevShowChar(textfont, textpos-tblastpos, c, tbnew, textpos+origin)) {
		FlushAnyText();
		goto restart;
	    }
	    tbnew= FALSE;
	    tblastpos= textpos;
	    tblastpos.x+= w;
	}
	rr.origin.x+= w;
	textpos.x+= w;
    }
}

//---- device dependent methods ------------------------------------------------

void Port::DrawObject(char, Rectangle*, Point, int, GrLineCap)
{
}

void Port::DrawPolygon(char, Rectangle*, Point*, int, GrPolyType, int, GrLineCap)
{
}

void Port::DevClip(Rectangle, Point)
{
    AbstractMethod("DevClip");
}

void Port::DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point)
{
    AbstractMethod("DevStrokeLine");
}

void Port::DevStrokeRect(int psz, Rectangle *r)
{
    DrawObject('b', r, gPoint0, psz, eDefaultCap);
}

void Port::DevFillRect(Rectangle *r)
{
    DrawObject('B', r, gPoint0, -1, eDefaultCap);
}

void Port::DevStrokeOval(int psz, Rectangle *r)
{
    DrawObject('o', r, gPoint0, psz, eDefaultCap);
}

void Port::DevFillOval(Rectangle *r)
{
    DrawObject('O', r, gPoint0, -1, eDefaultCap);
}

void Port::DevStrokeRRect(int psz, Rectangle *r, Point dia)
{
    DrawObject('r', r, dia, psz, eDefaultCap);
}

void Port::DevFillRRect(Rectangle *r, Point dia)
{
    DrawObject('R', r, dia, -1, eDefaultCap);
}

void Port::DevStrokeWedge(int psz, GrLineCap cap,
						    Rectangle *r, int s, int d)
{
    DrawObject('w', r, Point(s,d), psz, cap);
}

void Port::DevFillWedge(Rectangle *r, int s, int d)
{
    DrawObject('W', r, Point(s,d), -1, eDefaultCap);
}

void Port::DevStrokePolygon(Rectangle *r, Point *pts,
			    int npts, GrPolyType t, int psz, GrLineCap cap)
{
    DrawPolygon('P', r, pts, npts, t, psz, cap);
}

void Port::DevFillPolygon(Rectangle *r, Point *pts, int npts, GrPolyType t) 
{
    DrawPolygon('P', r, pts, npts, t, -1, eDefaultCap);
}

void Port::DevShowBitmap(Rectangle*, struct Bitmap*)
{
    AbstractMethod("DevShowBitmap");
}

void Port::DevShowTextBatch(Rectangle*, Point)
{
    AbstractMethod("DevShowTextBatch");
}

void Port::DevGiveHint(int, int, void*)
{
}

void Port::DevSetPattern(struct DevBitmap*)
{
}

bool Port::DevSetColor(RGBColor *cp)
{
    cp->SetId(-1);
    return TRUE;
}

void Port::DevSetOther(int)
{
}

bool Port::DevImageCacheBegin(ImageCache*, Rectangle)
{
    return TRUE;
}

void Port::DevImageCacheEnd(ImageCache*)
{
}

