#include "NeWSWindowPort.h"

#include "NeWSWindowSystem.h"
#include "NeWSFont.h"
#include "NeWSBitmap.h"
#include "String.h"
#include <sgtty.h>


#include "psio.h"
#include "news.h"
#undef ps_flush_PostScript
#define ps_flush_PostScript() psio_flush(PostScript)

NeWSWindowPort *wports[100];
extern int lastwinid;

extern int ioctl(int,int,void*);


//---- Cursors -----------------------------------------------------------------

void NeWSWindowPort::DevSetCursor(GrCursor c)
{
}

//---- window creation/destruction ---------------------------------------------

NeWSWindowPort::NeWSWindowPort(InpHandlerFun nf, void *priv1, bool ov, bool bl)
{
    Init(nf, priv1, ov, bl);
    ps_makewin(int(ov), &psid);
    wports[psid]= this;
}

void NeWSWindowPort::DevDestroy2()
{
}

//---- window management -------------------------------------------------------

void NeWSWindowPort::DevShow(WindowPort*, Rectangle r)
{
    ps_mapwin(psid, r.origin.x, r.origin.y, r.extent.x, r.extent.y);
}

void NeWSWindowPort::DevHide()
{     
    ps_unmapwin(psid);
}

void NeWSWindowPort::DevSetRect(Rectangle *r)
{
    ps_setrect(psid, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevTop(bool top)
{
    ps_top(psid, (int) top);
}

void NeWSWindowPort::DevBell(long d)
{
}

//---- clipping ----------------------------------------------------------------

void NeWSWindowPort::DevClip(Rectangle r, Point)
{
    if (psid != lastwinid) { lastwinid= psid; ps_focus(psid); }
    ps_setclip(r.origin.x, r.origin.y, r.extent.x, r.extent.y);
} 

void NeWSWindowPort::DevResetClip()
{
}

//---- graphical primitives ----------------------------------------------------
       
void NeWSWindowPort::DevStrokeLine2(GrPattern pat, int psz,
    Rectangle*, GrLineCap, Point start, Point end)
{
    ps_strokeline(pat, psz, start.x, start.y, end.x, end.y);
}

void NeWSWindowPort::DevStrokeRect2(GrPattern pat, int psz,
    Rectangle *r)
{
    ps_strokerect(pat, psz, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevFillRect(GrPattern pat, Rectangle *r)
{
    ps_fillrect(pat, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevStrokeRRect2(GrPattern pat, int psz,
							Rectangle *r, Point dia)
{
    ps_strokerrect(pat, psz, dia.x, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevFillRRect2(GrPattern pat, Rectangle *r, Point dia)
{
    ps_fillrrect(pat, dia.x, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevStrokeOval2(GrPattern pat, int psz, Rectangle *r)
{
    ps_strokeoval(pat, psz, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevFillOval2(GrPattern pat, Rectangle *r)
{
    ps_filloval(pat, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevStrokeWedge2(GrPattern pat, int psz,
				    GrLineCap, Rectangle *r, int, int)
{
    ps_strokeoval(pat, psz, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevFillWedge2(GrPattern pat, Rectangle *r, int, int)
{
    ps_filloval(pat, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void NeWSWindowPort::DevStrokePolygon(Rectangle *r, GrPattern pat,
		Point *p, int npts, GrPolyType, int psz, GrLineCap cap)
{
    Point a= r->origin;
    if (cap & eStartArrow)
	DrawArrow(pat, psz, p[1]+a, p[0]+a);
    if (cap & eEndArrow)
	DrawArrow(pat, psz, p[npts-2]+a, p[npts-1]+a);

    ps_openpolygon(p[0].x+a.x, p[0].y+a.y);
    for (int i= 1; i < npts; i+= 3)
	ps_curveto(p[i].x+a.x, p[i].y+a.y, p[i+1].x+a.x, p[i+1].y+a.y,
						p[i+2].x+a.x, p[i+2].y+a.y);
    ps_strokepolygon(pat, psz);
}

void NeWSWindowPort::DevFillPolygon(Rectangle *r, GrPattern pat,
					    Point *p, int npts, GrPolyType)
{
    Point a= r->origin;
    ps_openpolygon(p[0].x+a.x, p[0].y+a.y);
    for (int i= 1; i < npts; i+= 3)
	ps_curveto(p[i].x+a.x, p[i].y+a.y, p[i+1].x+a.x, p[i+1].y+a.y,
						p[i+2].x+a.x, p[i+2].y+a.y);
    ps_fillpolygon(pat);
}

void NeWSWindowPort::DevShowBitmap(GrPattern pat, Rectangle *r,
    Bitmap *bm)
{
    int bid= ((NeWSBitmap*)bm)->GetPixmap();

    if (bid >= 0)
	ps_showbitmap(pat, r->origin.x, r->origin.y, r->extent.x, r->extent.y, bid);
}

//---- text batching -----------------------------------------------------------

bool NeWSWindowPort::DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, 
									    Point)
{
    if (isnew) {                // first
	bcnt= 0;
	lfont= fdp;
    } else if (fdp != lfont || delta != gPoint0)
	return TRUE;
    bbuf[bcnt++]= c;
    return FALSE;
}

void NeWSWindowPort::DevShowTextBatch(GrPattern pat, Rectangle*, Point pos)
{
    lbfont= lfont;
    bbuf[bcnt]= '\0';
    ps_showtext(((NeWSServerFont*)lbfont)->GetId(), pat, pos.x, pos.y, bbuf);      
} 

//---- scrolling ---------------------------------------------------------------

void NeWSWindowPort::DevScrollRect(Rectangle r, Point d)
{
    r.origin-= d;
    ps_scrollrect(r.origin.x, r.origin.y, r.extent.x, r.extent.y, d.x, d.y);
}

//---- input -------------------------------------------------------------------

void NeWSWindowPort::DevGetEvent(Token *tp, int, bool)
{   
    int n, arg, id, code, flags, x, y, w, h;
    register NeWSWindowPort *port;
    Token t;
    
    if (block >= 0) {
	n= psio_availinputbytes(PostScriptInput);
	if (n >= 3)
	    goto aha;
	ioctl(psio_fileno(PostScriptInput), FIONREAD, &arg);
	if (n+arg >= 3)
	    goto aha;
	tp->Flags= 0;
	tp->Code= eEvtNone;
	return;
    }

aha:
    for (;;) {
	if (ps_getevent(&id, &code, &flags, &t.At, &x, &y, &w, &h)) {
	    port= wports[id];
	    t.Code= code;
	    t.Flags= flags;
	    t.Pos.x= x;
	    t.Pos.y= y;
	    t.ext.x= w;
	    t.ext.y= h;
	    if (code == eEvtDamage) {
		port->Send(&t);
		continue;
	    }
	    if (port == this) {
		*tp= t;
		return;
	    }
	}
    }
}

void NeWSWindowPort::DevGrab(bool on, bool fullscreen)
{
}
    
//---- mouse position ----------------------------------------------------------

void NeWSWindowPort::DevSetMousePos(Point p, bool m)
{
}

//---- double buffering --------------------------------------------------------

void NeWSWindowPort::DevGiveHint(int code, int, void*)
{
    switch (code) {
    case eHintUnlock:
    case eHintUnbatch:
    case eHintFlush:
	ps_flush_PostScript();
	break;
    }
}
