//$WindowPort$
#include "WindowPort.h"
#include "Error.h"
#include "System.h"
#include "WindowSystem.h"
#include "Math.h"
#include "Storage.h"

const int MaxInvals= 20;

extern bool fullscreen;
extern bool gBatch;

WindowPort *focusport;
u_long lasttime;
int Clicks;

static Token lastClick;
static WindowPort *grabport;

//---- WindowPort --------------------------------------------------------------

MetaImpl0(WindowPort);

WindowPort::WindowPort(InpHandlerFun ih, void *p1, bool ov, bool bl, bool)
{
    ihf= ih;
    privdata= p1;
    overlay= ov;
    block= bl;
    state= eWsHidden;
    cursor= eCrsBoldArrow;
    hascolor= gColor;
    
    done= FALSE;
    havepushbacktoken= FALSE;
    
    if (invalRects == 0)
	invalRects= new Rectangle[MaxInvals];
    inval= 0;
}

WindowPort::~WindowPort()
{
    if (focusport == this)
	focusport= 0;
    if (state == eWsClosed)
	return;
    state= eWsClosed;
    SafeDelete(invalRects);
    gWindowSystem->RemoveWindow(this);
}

void WindowPort::InvalidateRect(Rectangle r)
{
    if (inval == 0) {
	invalRects[inval++]= invalBounds= r;
	return;
    }
    invalBounds.Merge(r);
    for (int i= 0; i < inval; i++) {
	if (invalRects[i].ContainsRect(r))
	    return;
	if (r.ContainsRect(invalRects[i])) {
	    invalRects[i]= r;
	    return;
	} 
	if (r.Intersects(invalRects[i])) {
	    invalRects[i].Merge(r);
	    return;
	}
    }
    if (inval >= MaxInvals)
	invalRects[inval-1].Merge(r);
    else
	invalRects[inval++]= r;
}

//---- cursor stuff ------------------------------------------------------------

GrCursor WindowPort::SetCursor(GrCursor c)
{
    if (c != cursor && c >= eCrsNone && c <= eCrsUpDownArrow) {
	GrCursor oldc= cursor;
	cursor= c;
	DevSetCursor(c);
	GiveHint(eHintFlush, 0, 0);
	return oldc;
    }
    return c;
}

GrCursor WindowPort::GetCursor()
{
    return cursor;
}

GrCursor WindowPort::SetWaitCursor(unsigned int, GrCursor c)
{
    return SetCursor(c);
}

//---- input handling ----------------------------------------------------------

void WindowPort::GetEvent(Token *t, int timeout, bool overread)
{
    if (havepushbacktoken) {
	*t= pushbacktoken;
	havepushbacktoken= FALSE;
    } else
	DevGetEvent(t, timeout, overread);
	
    if (t->Code != eEvtDamage && t->Code != eEvtNone && t->Code != eEvtIdle) {
	if (t->Code != eEvtEnter && t->Code != eEvtExit)
	    lastpos= t->Pos;
	lasttime= t->At;
    } else if (t->Code == eEvtNone) {
	t->Pos= lastpos;
	t->At= lasttime;
    }
};

void WindowPort::SendInput(Token *t)
{
    if (state != eWsShown)
	return;
    
    if (t->Code != eEvtDamage && t->Code != eEvtNone && t->Code != eEvtIdle) {
	if (t->Code != eEvtEnter && t->Code != eEvtExit)
	    lastpos= t->Pos;
	lasttime= t->At;
    } else if (t->Code == eEvtNone) {
	t->Flags= 0;
	t->Pos= lastpos;
	t->At= lasttime;
    }
	
    WindowPort *wp= grabport;

    if (t->IsKey() && t->Flags == (eFlgShiftKey|eFlgCntlKey|eFlgMetaKey)
						 && t->MapToAscii() == 'u')
	Exit(1, FALSE);
    
    switch (t->Code) {
    case eEvtEnter:
	focusport= this;
	break;
    case eEvtExit:
	focusport= 0;
	break;
    }
    if (wp && (wp != this)) {
	switch (t->Code) {
	case eEvtIdle:
	case eEvtNone:
	case eEvtLocMove:
	case eEvtLocMoveBut:
	case eEvtLocStill:
	case eEvtEnter:
	    // ignore events
	    return;
	case eEvtExit:
	case eEvtDamage:
	    // allow damages
	    break;
	default:
	    wp->Bell(50);
	    return;
	}  
    }
    switch (t->Code) {
    case eEvtDamage:
	switch (t->Flags) {
	case eFlgDamage1:
	    InvalidateRect(t->ext);
	    break;
	case eFlgDamage2:
	    InvalidateRect(t->DamageRect());
	    break;
	case eFlgDamage3:
	    InvalidateRect(t->DamageRect());
	    return;
	}
	break;
    default:
	if (t->IsMouseButton()) {
	    if (lastClick.DoubleClick(*t))
		Clicks++;
	    else
		Clicks= 1;
	    lastClick= *t;
	}
	break;
    }
    (*ihf)(privdata, t);
}

void WindowPort::PushEvent(Token t)
{
    if (t.Code != eEvtNone) {
	pushbacktoken= t;
	havepushbacktoken= TRUE;
    }
}

void WindowPort::Damage(EventFlags f, Rectangle *r)
{
    Token dt(f, *r);
    dt.Dump();
    SendInput(&dt);
}
	    
//---- window management -------------------------------------------------------

void WindowPort::Hide()
{
    if (state != eWsShown)
	return;
    state= eWsHidden;
    
    inval= 0;
	
    FlushAnyText();
    
    if (grabport || block)
	done= TRUE;
    else
	DevHide1();
};

void WindowPort::Show(WindowPort *father, Rectangle r)
{
    Point delta;
    Rectangle rr= r;
    
    if (state != eWsHidden)
	return;
    state= eWsShown;
    
    inval= 0;
    
    FlushAnyText();

    if (father)
	rr.origin+= father->GetRect().origin;
    else if (r.origin == gPoint_1)
	r.origin= Half(gScreenRect.extent - r.extent);
    delta= rr.AmountToTranslateWithin(gScreenRect);
    rr.origin+= delta;
    
    DevShow1(father, rr);
    
    if (grabport || block) {
	WindowPort *oldgrabport= grabport;
	grabport= this;
	if (delta != gPoint0)
	    MoveMousePos(delta);
	done= FALSE;
	if (havepushbacktoken) {
	    havepushbacktoken= FALSE;
	    SendInput(&pushbacktoken);
	}
	while (! done)
	    gSystem->InnerLoop();
	DevHide1();
	grabport= oldgrabport;
    }
};

void WindowPort::DevHide1()
{
    state= eWsHidden;
    DevHide();
    GiveHint(eHintFlush, 0, 0);      
}

void WindowPort::DevShow1(WindowPort *father, Rectangle r)
{
    state= eWsShown;
    DevShow(father, r);
    GiveHint(eHintFlush, 0, 0);      
}

void WindowPort::Grab(bool g, bool fs)
{
    if (fs) {
	FlushAnyText();
	fullscreen= g;
	if (g)   // make sure that clipping is turned off
	    Clip(gRect0, gPoint0);
    }
    DevGrab(g, fs);
}

Rectangle WindowPort::GetRect()
{
    return DevGetRect();
}

void WindowPort::SetOrigin(Point o)
{
    DevSetOrigin(o);
}

void WindowPort::SetExtent(Point e)
{
    DevSetExtent(e);
}

//---- graphic functions -------------------------------------------------------

void WindowPort::ScrollRect(Rectangle r, Point delta)
{
    Rectangle r1, rl[4];
    register int i, n;
    
    FlushMyText();
    r+= origin;
    r.Clip(cliprect);
    r1= r + delta;
    n= Difference(rl, r, r1);
    for (i= 0; i < n; i++)
	InvalidateRect(rl[i]);
    if (r.Clip(r1))
	DevScrollRect(r, delta);
}

bool WindowPort::DevImageCacheBegin(ImageCache *bb, Rectangle r)
{
    if (gBatch) {
	FlushMyText();
	r.origin += origin;
	if (r.Clip(cliprect)) {
	    r.origin -= origin;
	    bb->r= r;
	    if (bb->invalid || !bb->vr.ContainsRect(r)) {
		bb->vr= r;
		if (bb->b == 0)
		    bb->b= new Bitmap(r.extent, gDepth);
		return TRUE;
	    }
	}
	r.origin += origin;
	DevImageCacheCopy2(bb->b->GetDevBitmap(), &r, bb->r.origin - bb->vr.origin);
	return FALSE;
    }
    return TRUE;
}

void WindowPort::DevImageCacheEnd(ImageCache *bb)
{
    if (gBatch) {
	Rectangle r= bb->vr;
	r.origin += origin;
	if (bb->b == 0)
	    bb->b= new Bitmap(r.extent, gDepth);
	DevImageCacheEnd2(bb->b->GetDevBitmap(), &r);
	bb->invalid= FALSE;
    }
}

//---- window system independent graphic methods -------------------------------

Point WindowPort::DrawArrow(int psz, Point s, Point e)
{
    int arrowwidth= max(6, psz*4+2), arrowlen= max(9, psz*7);
    Point c, pts[3], ee(arrowwidth/2, arrowlen);
    
    if (Length(e-s) < arrowlen)
	return e;     // too short, give up

    double ph= Phi(e-s), rot= Phi(ee), hyp= Length(ee);

    pts[0]= e;
#ifdef PETERFIX
// special case: going straight down
    pts[1]= e + Point(-(arrowwidth/2),-arrowlen);
    pts[2]= e + Point(arrowwidth/2,-arrowlen);
#else
    pts[1]= e + PolarToPoint(ph+rot, hyp, hyp);
    pts[2]= e + PolarToPoint(ph-rot, hyp, hyp);
#endif
    c= Half(pts[1]+pts[2]);
    
    Rectangle rr= BoundingBox(3, pts);
    DevFillPolygon2(&rr, pts, 3, ePolyDefault);
    
    return c;
}

void WindowPort::DevStrokeLine(int psz, Rectangle *r,
					GrLineCap cap, Point start, Point end)
{
    if (cap & eStartArrow)
	start= DrawArrow(psz, end, start);
    if (cap & eEndArrow)
	end= DrawArrow(psz, start, end);
    DevStrokeLine2(psz, r, cap, start, end);
}

void WindowPort::DevStrokeRect(int psz, Rectangle *r)
{
    int rpsz= (psz <= 0) ? 1 : psz;
	
    rpsz*= 2;
    if (rpsz >= r->extent.x || rpsz >= r->extent.y)
	DevFillRect(r);
    else
	DevStrokeRect2(psz, r);
}

//---- splines -----------------------------------------------------------------

/*
 * bez: Subdivide a Bezier spline, until it is thin enough to be
 *      considered a line. Store line point in static array points.
*/

#define FIX(a) (((int) (a)) << 16)
#define INT(a) (((a) + (1 << 15)) >> 16 )
#define DELTA 1

static void bez(Point **gPp, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
{
    int maxx= max(x0, x3), minx= min(x0, x3), maxy= max(y0, y3), miny= min(y0, y3);
    register int tx, ty;
    
    if (x1 >= minx && x1 <= maxx && y1 >= miny && y1 <= maxy
		&& x2 >= minx && x2 <= maxx && y2 >= miny && y2 <= maxy) {
	register int ax, ay, dx= INT(x3-x0), dy= INT(y3-y0);
	if (dx == 0 || dy == 0) {
	    (*gPp)->x= INT(x3);
	    (*gPp)++->y= INT(y3);
	    return;
	}
	ax= ((dy*INT(x1-x0))/dx)+INT(y0-y1);
	ay= ((dx*INT(y1-y0))/dy)+INT(x0-x1);
	if (abs(ax*ay) <= DELTA) {
	    (*gPp)->x= INT(x3);
	    (*gPp)++->y= INT(y3);
	    return;
	}
	ax= ((dy*INT(x2-x0))/dx)+INT(y0-y2);
	ay= ((dx*INT(y2-y0))/dy)+INT(x0-x2);
	if (abs(ax*ay) <= DELTA) {
	    (*gPp)->x= INT(x3);
	    (*gPp)++->y= INT(y3);
	    return;
	}
    }
	
    tx = (x0 >> 3) + 3 * (x1 >> 3) + 3 * (x2 >> 3) + (x3 >> 3);
    ty = (y0 >> 3) + 3 * (y1 >> 3) + 3 * (y2 >> 3) + (y3 >> 3);
    bez(gPp, x0, y0, (x0 >> 1) + (x1 >> 1), (y0 >> 1) + (y1 >> 1),
		(x0 >> 2) + (x1 >> 1) + (x2 >> 2),
		(y0 >> 2) + (y1 >> 1) + (y2 >> 2),
		tx, ty);
    bez(gPp, tx, ty, (x3 >> 2) + (x2 >> 1) + (x1 >> 2),
		(y3 >> 2) + (y2 >> 1) + (y1 >> 2),
		(x3 >> 1) + (x2 >> 1), (y3 >> 1) + (y2 >> 1),
		x3, y3);
}

static int makespline(Point *pts, Point *p, int n, GrPolyType)
{
    register int i;
    Point *gPp= pts;
    
    gPp->x= p[0].x;
    (gPp++)->y= p[0].y;
    for (i= 0; i < n-1; i+= 3)
	bez(&gPp, FIX(p[i+0].x), FIX(p[i+0].y), FIX(p[i+1].x), FIX(p[i+1].y),
		 FIX(p[i+2].x), FIX(p[i+2].y), FIX(p[i+3].x), FIX(p[i+3].y));
	
    return gPp - pts;
}

void WindowPort::DevStrokePolygon(Rectangle *r,
		Point *pts, int npts, GrPolyType t, int psz, GrLineCap cap)
{
    Point *tmppts= 0;
    
    if (cap & eStartArrow)
	DrawArrow(psz, pts[1]+r->origin, pts[0]+r->origin);
    if (cap & eEndArrow)
	DrawArrow(psz, pts[npts-2]+r->origin, pts[npts-1]+r->origin);
    if (t & ePolyBezier) {
	tmppts= (Point*) Alloca(sizeof(Point) * npts * 30);
	int nn= makespline(tmppts, pts, npts, t);
	if (nn > 0) {
	    pts= tmppts;
	    npts= nn;
	}
    }
    DevStrokePolygon2(r, pts, npts, ePolyDefault, psz, cap);
    Freea(tmppts);
}

void WindowPort::DevFillPolygon(Rectangle *r, Point *pts, int npts, GrPolyType t)
{
    Point *tmppts= 0;
    if (t & ePolyBezier) {
	tmppts= (Point*) Alloca(sizeof(Point) * npts * 30);
	int nn= makespline(tmppts, pts, npts, t);
	if (nn > 0) {
	    pts= tmppts;
	    npts= nn;
	}
    }
    DevFillPolygon2(r, pts, npts, ePolyDefault);
    Freea(tmppts);
}

void WindowPort::DevStrokeWedge(int psz, GrLineCap cap, Rectangle *r, int s, int d)
{
    int rpsz= (psz <= 0) ? 1 : psz;
    rpsz*= 2;
    if (rpsz >= r->extent.x || rpsz >= r->extent.y) {
	DevFillWedge(r, s, d);
	return;
    }
    if (r->extent.x < 4 || r->extent.y < 4) {
	DevStrokeRect(psz, r);
	return;
    }
    if (cap & eStartArrow) {
	DrawArrow(psz, r->OvalAngleToPoint(s+10), r->OvalAngleToPoint(s));
	// s+= 10; d-= 10;
    }
    if (cap & eEndArrow) {
	DrawArrow(psz, r->OvalAngleToPoint(s+d-10), r->OvalAngleToPoint(s+d));
	// d-= 10;
    }
    DevStrokeWedge2(psz, cap, r, s, d);
}

void WindowPort::DevFillWedge(Rectangle *r, int s, int d)
{
    if (r->extent.x < 4 || r->extent.y < 4)
	DevFillRect(r);
    else
	DevFillWedge2(r, s, d);
}

static void Wedge2Polygon(Point *pts, int &n, Point e2, float start, float len)
{
    if (len < 10)    // consider it a line
	pts[n++]= PolarToPoint(start, e2.x, e2.y) + e2;
    else {                  // subdivide
	Wedge2Polygon(pts, n, e2, start, len/2);
	Wedge2Polygon(pts, n, e2, start+len/2, len/2);
    }
}

void WindowPort::DevStrokeWedge2(int psz, GrLineCap cap, Rectangle *r, int start, int len)
{
    Point *pts= (Point*) Alloca(sizeof(Point) * len);
    Point e2= (r->extent-psz)/2;
    int n= 0;

    Wedge2Polygon(pts, n, e2, start, len);
    pts[n++]= PolarToPoint(start+len, e2.x, e2.y) + e2;
    r->origin+= psz/2;
    DevStrokePolygon2(r, pts, n, ePolyDefault, psz, cap);
    Freea(pts);
}

void WindowPort::DevFillWedge2(Rectangle *r, int start, int len)
{
    Point *pts= (Point*) Alloca(sizeof(Point) * len);
    Point e2= r->extent/2;
    int n= 0;

    pts[n++]= e2;
    Wedge2Polygon(pts, n, e2, start, len);
    pts[n++]= PolarToPoint(start+len, e2.x, e2.y) + e2;
    DevFillPolygon2(r, pts, n, ePolyDefault);
    Freea(pts);
}

void WindowPort::DevStrokeRRect(int psz, Rectangle *r, Point dia)
{
    int rpsz= (psz <= 0) ? 1 : psz;
	
    rpsz*= 2;
    if (rpsz >= r->extent.x || rpsz >= r->extent.y) {
	DevFillRRect(r, dia);
	return;
    }
    if (ODD(dia.x))    // force to be even
	dia.x--;
    if (ODD(dia.y))    // force to be even
	dia.y--;
	
    if (dia.x >= r->extent.x && dia.y >= r->extent.y)
	DevStrokeOval(psz, r);
    else
	DevStrokeRRect2(psz, r, Min(dia, r->extent));
}

void WindowPort::DevFillRRect(Rectangle *r, Point dia)
{
    if (ODD(dia.x % 1))    // force to be even
	dia.x--;
    if (ODD(dia.y % 1))    // force to be even
	dia.y--;
	
    if (dia.x > r->extent.x && dia.y > r->extent.y)
	DevFillOval(r);
    else
	DevFillRRect2(r, Min(dia, r->extent));
}

void WindowPort::DevStrokeOval(int psz, Rectangle *r)
{
    int rpsz= (psz <= 0) ? 1 : psz;
    
    rpsz*= 2;
    if (rpsz >= r->extent.x || rpsz >= r->extent.y)
	DevFillOval(r);
    else if (r->extent.x < 4 || r->extent.y < 4)
	DevStrokeRect(psz, r);
    else
	DevStrokeOval2(psz, r);
}

void WindowPort::DevFillOval(Rectangle *r)
{
    if (r->extent.x < 4 || r->extent.y < 4)
	DevFillRect(r);
    else
	DevFillOval2(r);
}

//---- abstract methods --------------------------------------------------------

void WindowPort::DevStrokeRect2(int, Rectangle*)
{
    AbstractMethod("DevStrokeRect2");
}

void WindowPort::DevStrokeRRect2(int, Rectangle*, Point)
{
    AbstractMethod("DevStrokeRRect2");
}

void WindowPort::DevFillRRect2(Rectangle*, Point)
{
    AbstractMethod("DevFillRRect2");
}

void WindowPort::DevStrokePolygon2(Rectangle*, Point*, int, GrPolyType, int, GrLineCap)
{
    AbstractMethod("DevStrokePolygon2");
}

void WindowPort::DevFillPolygon2(Rectangle*, Point*, int, GrPolyType)
{
    AbstractMethod("DevFillPolygon2");
}

void WindowPort::DevStrokeLine2(int, Rectangle*, GrLineCap, Point, Point)
{
    AbstractMethod("DevStrokeLine2");
}

void WindowPort::DevStrokeOval2(int psz, Rectangle *r)
{
    DevStrokeRRect2(psz, r, r->extent);
}

void WindowPort::DevFillOval2(Rectangle *r)
{
    DevFillRRect2(r, r->extent);
}

void WindowPort::DevSetCursor(GrCursor)
{
    AbstractMethod("DevSetCursor");
}

void WindowPort::DevScrollRect(Rectangle, Point)
{
    AbstractMethod("DevScrollRect");
}

void WindowPort::DevHide()
{
    AbstractMethod("DevHide");
}

void WindowPort::DevShow(WindowPort*, Rectangle)
{
    AbstractMethod("DevShow");
}

void WindowPort::DevGrab(bool, bool)
{
    AbstractMethod("DevGrab");
}

void WindowPort::DevFullscreen(bool)
{
    AbstractMethod("DevFullScreen");
}

void WindowPort::DevTop(bool)
{
    AbstractMethod("DevTop");
};

void WindowPort::DevGetEvent(Token*, int, bool)
{
    AbstractMethod("DevGetEvent");
};

void WindowPort::DevSetMousePos(Point, bool)
{
    AbstractMethod("DevSetMousePos");
};

void WindowPort::DevBell(long)
{
    AbstractMethod("DevBell");
}

void WindowPort::DevImageCacheEnd2(DevBitmap*, Rectangle*)
{
}

void WindowPort::DevImageCacheCopy2(DevBitmap*, Rectangle*, Point)
{
}

Rectangle WindowPort::DevGetRect()
{
    return gRect0;
}

void WindowPort::DevSetExtent(Point)
{
    AbstractMethod("DevSetExtent");
}

void WindowPort::DevSetOrigin(Point)
{
    AbstractMethod("DevSetOrigin");
}

void WindowPort::DevSetTitle(char *name)
{
}
