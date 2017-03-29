#include "SunWindowPort.h"
#include "String.h"
#include "SunBitmap.h"
#include "SunFont.h"
#include "Storage.h"
#include "Error.h"

#include "pixrect.h"
#include "pixfont.hh"
#include "pixwin.hh"

#define MaxTextBatchCnt 400

#define setbit(a,i) ((a)[(i) >> 5])|= 1 << (31-((i) & 31))

extern "C" {
    extern void _sunwindow_setmouse(int fd, int, int, bool);
    extern void _sunwindow_setmask(int fd);
    extern void _sunwindow_setcursor(int, struct _cursor *);
}

extern suncolormap *Cmap;
extern bool fullscreen;
extern SunWindowPort *overlayport;
extern int cmseq, rootFd;
extern struct pixrect *tmppr, *tmppr2;


static struct pixrect *MakePen(int psz);


static struct pr_prpos bat[MaxTextBatchCnt], *bp;
static struct rect batchrect;
static int gapybits[50];
static int shallowsteep[100];
static int steepshallow[100];

static struct pr_chain left1= { 0, { 0, 0 }, steepshallow },
		       leftgapy= { &left1, { 0, 0 }, gapybits },
		       left0= { &leftgapy, { 0, 0 }, shallowsteep },
		       right1= { 0, { 0, 0 }, steepshallow },
		       rightgapy= { &right1, { 0, 0 }, gapybits },
		       right0= { &rightgapy, { 0, 0 }, shallowsteep };
		
static struct pr_fall left_oct = {
    { 0, 0 },
    &left0
};
static struct pr_fall right_oct = {
    { 0, 0 },
    &right0
};
	       
struct pr_trap oval = {
    &left_oct,
    &right_oct,
    0,
    0
};

inline Rectangle *r2R(rect *r)
{
    return (Rectangle*) r;
}

inline struct rect *R2r(Rectangle *r)
{
    return (rect*) r;
}

void SunWindowPort::TestDamage()
{
    if (overlay)
	return;
    struct rect s;
    win_getrect(fd, &s);
    pw_damaged(opw);
    if (s.r_width != myrect.extent.x || s.r_height != myrect.extent.y) {
	pw_donedamaged(opw);
	myrect= *r2R(&s);
	Damage(eFlgDamage1, (Rectangle*) &s);
    } else {                        
	s= opw->pw_clipdata->pwcd_clipping.rl_bound;
	if (s.r_width > 0 && s.r_height > 0)
	    Damage(eFlgDamage2, (Rectangle*) &s);
	pw_donedamaged(opw);
    }
}

bool NewPixrect(pixrect **prp, Point e, bool exact, bool clear, int depth)
{
    struct pixrect *pr= *prp;
    
    if (pr == 0 || pr->pr_size.x != e.x || pr->pr_size.y != e.y || depth != pr->pr_depth) {
	if (pr == 0 || pr->pr_size.x < e.x || pr->pr_size.y < e.y || depth != pr->pr_depth || exact) {
	    if (pr)
		pr_destroy(pr);
	    if ((*prp= pr= mem_create(e.x, e.y, depth)) == 0)
		Error("NewPixrect", "can't allocate pixrect");
	    return TRUE;
	}
    }
    if (clear) {
	pr_rop(pr, 0, 0, e.x, e.y, PIX_CLR|PIX_DONTCLIP, 0, 0, 0);
	return TRUE;
    }
    return FALSE;
}

pixrect *SunWindowPort::GetPattern(Point e)
{
    if (patbm)
	pat= patbm->Expand(e);
    return pat;
}

void SunWindowPort::Stencil(Rectangle *rr, pixrect *ppr)
{
    rect *r= (rect*) rr;
    lock(rr);
    if (ink == eInkColor || ink == eInkPat) {
	if (pr)
	    pr_stencil(pr, r->r_left+fsorigin.x, r->r_top+fsorigin.y, r->r_width, r->r_height,
		tmode, ppr, 0, 0, GetPattern(rr->extent), AlignX(r->r_left), AlignY(r->r_top));
	else
	    pw_stencil(pw, r->r_left, r->r_top, r->r_width, r->r_height,
		tmode, ppr, 0, 0, GetPattern(rr->extent), AlignX(r->r_left), AlignY(r->r_top));
    } else {
	if (pr)
	    pr_rop(pr, r->r_left+fsorigin.x, r->r_top+fsorigin.y, r->r_width, r->r_height,
						    tmode, ppr, 0, 0);
	else
	    pw_rop(pw, r->r_left, r->r_top, r->r_width, r->r_height,
						    tmode, ppr, 0, 0);
    }
    unlock();
}

void SunWindowPort::line(int psz, int x0, int y0, int x1, int y1)
{
    int w= psz/2;
    
    if (psz <= 1 && pat == 0) {
	Vector(x0, y0, x1, y1);
	return;
    }
    if (x0 == x1) {
	if (y0 <= y1)
	    BitBlt(x0-w, y0, psz, y1-y0);
	else
	    BitBlt(x1-w, y1, psz, y0-y1);
    } else if (y0 == y1) {
	if (x0 <= x1)
	    BitBlt(x0, y0-w, x1-x0, psz);
	else
	    BitBlt(x1, y1-w, x0-x1, psz);
    } else if (psz <= 1)
	Bresenham(0, x0, y0, x1, y1, psz);
    else {
	struct pr_pos pts[4];
	Point d= PolarToPoint(Phi(Point(x1-x0, y1-y0)), (double)psz/2, (double)psz/2);
	pts[0].x= x0 - d.y;
	pts[0].y= y0 + d.x;
	pts[1].x= x0 + d.y;
	pts[1].y= y0 - d.x;
	pts[2].x= x1 + d.y;
	pts[2].y= y1 - d.x;
	pts[3].x= x1 - d.y;
	pts[3].y= y1 + d.x;
	Polygon(4, pts, 0, 0);
    }
}

void SunWindowPort::PolyDot(pixrect *pr1, int dx, int dy, pr_pos *pts, int n, int psz)
{
    register int i, x, y, v;
    register struct pr_pos *p;
    register struct pixrect *patpr;
    int oox= 0, ooy= 0;
    
    patpr= pat;
    
    if (pr1) 
	patpr= 0;
    else if (pr) {
	pr1= pr;
	oox= fsorigin.x;
	ooy= fsorigin.y;
    }

    if (psz <= 1) {
	if (patpr) {
	    for (p= pts, i= 0; i<n; i++, p++) {
		x= p->x + dx;
		y= p->y + dy;
		v= pr_get(pat, AlignX(x), AlignY(y));
		if (pr1)
		    pr_put(pr1, x+oox, y+ooy, v);
		else
		    pw_put(pw, x, y, v);
	    }
	} else {
#ifndef sparc    /* Circumvent GFX (sparc) bug */
	    if (pr1)
		pr_polypoint(pr1, dx+oox, dy+ooy, n, pts, cmode);
	    else
		pw_polypoint(pw, dx, dy, n, pts, cmode);
#else /* sparc */
	    for (p= pts, i= 0; i<n; i++, p++) {
		x= p->x + dx;
		y= p->y + dy;
		if (pr1)
		    pr_rop(pr1, x+oox, y+ooy, psz, psz, cmode, 0, 0, 0);
		else
		    pw_rop(pw, x, y, psz, psz, cmode, 0, 0, 0);
	    }
#endif /* sparc */
	}
    } else {
	register struct pixrect *penpr= 0;
	int rop= cmode;
	
	if (psz >= 2 && psz < MAXPENS) {
	    if (pens[psz] == 0)
		pens[psz]= MakePen(psz);
	    penpr= pens[psz];
	    rop= PIX_OR;
	}
	oox+= dx;
	ooy+= dy;
	for (p= pts, i= 0; i < n; i++, p++) {
	    if (pr1)
		pr_rop(pr1, p->x+oox, p->y+ooy, psz, psz, rop, penpr, 0, 0);
	    else
		pw_rop(pw, p->x, p->y, psz, psz, rop, penpr, 0, 0);
	}
    }
}

void SunWindowPort::Bresenham(pixrect *pr1, int fromX, int fromY, int X2, int Y2, int psz)
{
    register int i, j, k, p, q, d, dx, dy;
    struct pr_pos *pts, *pp;

    i= (fromX < X2) ? 1 : -1;
    j= (fromY < Y2) ? 1 : -1;
    dx= abs(fromX - X2);
    dy= abs(fromY - Y2);
    
    pp= pts= (struct pr_pos*) Alloca(sizeof(struct pr_pos) * (dx+dy+1));
    
    if (dx >= dy) { /* flat line */
	d= p= dy+dy;
	q= 2*(dy-dx);
	for(k= 0; k <= dx; fromX+= i, k++) {
	    pp->x= fromX;
	    pp++->y= fromY;
	    if(d <= dx)
		d+= p;
	    else {
		d+= q;
		fromY+= j;
	    }
	}
    } else {          /* steep line */
	d= p= dx+dx;
	q= 2*(dx-dy);
	for(k= 0; k <= dy; fromY+= j, k++) {
	    pp->x= fromX;
	    pp++->y= fromY;
	    if (d <= dy)
		d+= p;
	    else {
		d+= q;
		fromX+= i;
	    }
	}
    }
    PolyDot(pr1, 0, 0, pts, k, psz);
    Freea(pp);
}

void SunWindowPort::PolyEllipse(pixrect *pr1, int x0, int y0, int x1, int x2,
					int y1, int y2, int a, int b, int psz)
{
    register struct pr_pos *qp;
    register int x, y, d, t6, t7;
    int t1, t2, t3, t4, t5, t8;
    struct pr_pos *q, *qe;
    
    x= a;
    y= 0;
    t1= a*a; t2= t1 << 1;
    t3= b*b; t4= t3 << 1;
    t5= a * t3; t6= t5 << 1; t7= 0;
    t8= a + b;
    qp= q= (struct pr_pos*) alloca(sizeof (struct pr_pos) * t8);
    qe= &q[t8];
    d= (t1+t3)/4-t5;
    
    while (qp < qe) {
	qp->x= x;
	qp++->y= y;
	if (d < 0) {
	    y++;
	    t7+= t2;
	    d+= t7;
	} else {
	    x--;
	    t6-= t4;
	    d-= t6;
	}
    }
    
    x1+= x0; x2+= x0;
    y1+= y0; y2+= y0;
    
    PolyDot(pr1, x1, y2, q, t8, psz);
    for (qp= q; qp < qe; qp++)
	qp->x= -qp->x;
    PolyDot(pr1, x2, y2, q, t8, psz);
    for (qp= q; qp < qe; qp++)
	qp->y= -qp->y;
    PolyDot(pr1, x2, y1, q, t8, psz);
    for (qp= q; qp < qe; qp++)
	qp->x= -qp->x;
    PolyDot(pr1, x1, y1, q, t8, psz);
}

//---- SunWindowPort -----------------------------------------------------------

MetaImpl0(SunWindowPort);

SunWindowPort::SunWindowPort(InpHandlerFun nf, void *priv1, bool ov, bool bl, bool wm)
					: WindowPort(nf, priv1, ov, bl, wm)
{
    if (!overlay)
	fd= win_getnewwindow();
    if (! (ov && bl)) {
	SetResourceId(fd);
	gSystem->AddFileInputHandler(this);
   }
}

SunWindowPort::~SunWindowPort()
{
    if (!overlay) {
	win_remove(fd);
	if (rpw)
	    pw_close(rpw);
	if (opw)
	    pw_close(opw);
	if (rpr)
	    pr_close(rpr);
	if (pr)
	    pr_close(pr);
	close(fd);
    }
}

void SunWindowPort::Notify(SysEventCodes, int)
{
    SunWindowPort *p= overlayport ? overlayport : this;
    if (p) {
	Token t;
	p->GetEvent(&t, -1, FALSE);
	p->SendInput(&t);
    }
}

void SunWindowPort::lock(Rectangle *r) 
{
    flushcolormap();
    if (! inbatch)
	pw_lock(pw, r);
}

void SunWindowPort::unlock() 
{
    if (! inbatch)
	pw_unlock(pw);
}

void SunWindowPort::Replrop(int x, int y, int w, int h)
{
    if (pr)
	pr_replrop(pr, x+fsorigin.x, y+fsorigin.y, w, h, cmode, pat, Align2X(x), Align2Y(y));
    else
	pw_replrop(pw, x, y, w, h, cmode, pat, Align2X(x), Align2Y(y));
}

void SunWindowPort::Rop(int x, int y, int w, int h)
{
    if (pr)
	pr_rop(pr, x+fsorigin.x, y+fsorigin.y, w, h, cmode, 0, 0, 0);
    else
	pw_rop(pw, x, y, w, h, cmode, 0, 0, 0);
}

void SunWindowPort::BitBlt(int x, int y, int w, int h)
{
    if (pat)
	Replrop(x, y, w, h);
    else
	Rop(x, y, w, h);
}

void SunWindowPort::Vector(int x1, int y1, int x2, int y2)
{
    if (pr)
	pr_vector(pr, x1+fsorigin.x, y1+fsorigin.y, x2+fsorigin.x, y2+fsorigin.y, cmode, 1);
    else
	pw_vector(pw, x1, y1, x2, y2, cmode, 1);
}

void SunWindowPort::Traprop(int l, int t, int w, int h, int radx)
{
    if (pr)
	pr_traprop(pr, l+radx+fsorigin.x, t+fsorigin.y, oval, cmode,
			    GetPattern(Point(w, h)), radx+AlignX(l), AlignY(t));
    else
	pw_traprop(pw, l+radx, t, oval, cmode,
			    GetPattern(Point(w, h)), radx+AlignX(l), AlignY(t));
}

void SunWindowPort::Polygon(int n, pr_pos *pts, int l, int t)
{
    if (pr)
	pr_polygon_2(pr, l+fsorigin.x, t+fsorigin.y, 1, &n, pts, cmode,
						    pat, AlignX(l), AlignY(t));
    else
	pw_polygon_2(pw, l, t, 1, &n, pts, cmode, pat, AlignX(l), AlignY(t));
}   

void SunWindowPort::DevClip(Rectangle r, Point)
{
    if (fullscreen) {   // reset clipping
	if (pr) {
	    pr= opr;
	    fsorigin= gPoint0;
	}
	if (! overlay)
	    pw= opw;
    } else {
	cliprect= r;
	if (pr) {
	    if (rpr)
		pr_close(rpr);
	    pr= rpr= pr_region(opr, r.origin.x, r.origin.y, r.extent.x, r.extent.y);
	    fsorigin= -r.origin; // simulate missing function pr_set_xy_offset()
    
	}
	if (! overlay) {
	    if (rpw)
		pw_set_region_rect(rpw, (rect*) &r, 1, 0);
	    else
		rpw= pw_region(opw, r.origin.x, r.origin.y, r.extent.x, r.extent.y);
	    pw= rpw;
	    pw_set_xy_offset(pw, r.origin.x, r.origin.y);
	}
    }
}

void SunWindowPort::DevImageCacheEnd2(DevBitmap *b, Rectangle *r)
{
    SunBitmap *dbm= (SunBitmap*) b;
    NewPixrect(&dbm->spr, r->extent, FALSE, FALSE, gDepth);
    pr_rop(dbm->spr, 0, 0, r->extent.x, r->extent.y, PIX_SRC, pr,
			r->origin.x+fsorigin.x, r->origin.y+fsorigin.y);
}

void SunWindowPort::DevImageCacheCopy2(DevBitmap *b, Rectangle *r, Point p)
{
    SunBitmap *dbm= (SunBitmap*) b;
    if (pr)
	pr_rop(pr, r->origin.x+fsorigin.x, r->origin.y+fsorigin.y, 
		    r->extent.x, r->extent.y, PIX_SRC, dbm->spr, p.x, p.y);
    else
	pw_rop(pw, r->origin.x, r->origin.y, 
		    r->extent.x, r->extent.y, PIX_SRC, dbm->spr, p.x, p.y);
}

void SunWindowPort::DevStrokeLine2(int psz, Rectangle *r, GrLineCap, Point p1, Point p2)
{
    if (psz <= 0)
	psz= 1;
    lock(r);
    line(psz, p1.x, p1.y, p2.x, p2.y);
    unlock();
}

void SunWindowPort::DevStrokeRect2(int psz, Rectangle* r)
{
    register int l= r->origin.x, t= r->origin.y, wd= r->extent.x, ht= r->extent.y;
    
    if (psz <= 0)
	psz= 1;
    lock(r); 
    BitBlt(l+psz, t, wd-psz, psz);
    BitBlt(l+wd-psz, t+psz, psz, ht-psz);
    BitBlt(l, t+ht-psz, wd-psz, psz);
    BitBlt(l, t, psz, ht-psz);
    unlock();
}

void SunWindowPort::DevFillRect(Rectangle* r)
{
    lock(r);
    BitBlt(r->origin.x, r->origin.y, r->extent.x, r->extent.y);
    unlock();
}

void SunWindowPort::DevStrokeRRect2(int psz, Rectangle* r, Point dia)
{
    int ww, hh, cx, cy, w, h, rad1, rad2, l, t, r1, r2;
    struct pixrect *cpr= 0;
	
    if (psz <= 0)
	psz= 1;
	
    ww= r->extent.x;
    hh= r->extent.y;
    
    w= ww - psz;
    h= hh - psz;
    
    rad1= (dia.x - psz)/2;
    rad2= (dia.y - psz)/2;
    rad1= max(1, rad1);
    rad2= max(1, rad2);
    
    r1= dia.x/2 -1;
    r2= dia.y/2;
    
    cx= ww-r1-r1;
    cy= hh-r2-r2;
    
    if (psz > 1) {
	if (! findpr('a', &cpr, ww, hh, rad1, rad2, psz)) {
	    PolyEllipse(cpr, 0, 0, w-rad1, rad1, rad2, h-rad2, rad1, rad2, psz);
	    if (cx) {
		pr_rop(cpr, r1, 0, cx, psz, PIX_SET|PIX_DONTCLIP, 0, 0, 0);
		pr_rop(cpr, r1, h, cx, psz, PIX_SET|PIX_DONTCLIP, 0, 0, 0);
	    }
	    if (cy) {
		pr_rop(cpr, 0, r2, psz, cy, PIX_SET|PIX_DONTCLIP, 0, 0, 0);
		pr_rop(cpr, w, r2, psz, cy, PIX_SET|PIX_DONTCLIP, 0, 0, 0);
	    }
	}
	Stencil(r, cpr);
    } else {
	lock(r);
	l= r->origin.x;
	t= r->origin.y;
	PolyEllipse(0, l, t, w-rad1, rad1, rad2, h-rad2, rad1, rad2, psz);
	if (cx) {
	    BitBlt(l+r1, t,   cx, psz);
	    BitBlt(l+r1, t+h, cx, psz);
	}
	if (cy) {
	    BitBlt(l,   t+r2, psz, cy);
	    BitBlt(l+w, t+r2, psz, cy);
	}
	unlock();
    }
}

static bool MakeQuadrant(int radx, int rady, int gapx, int gapy, int h)
{
    static int lastradx, lastrady, lastgapx, lastgapy;
    register int d, m, n, t6, t7;
    int i, t1, t2, t3, t4, t5, t8;

    if (lastradx==radx && lastrady==rady && lastgapx==gapx && lastgapy==gapy)
	return FALSE;
    lastradx= radx; lastrady= rady;
    lastgapx= gapx; lastgapy= gapy;

    left0.size.x= right1.size.x= -radx;
    right0.size.x= left1.size.x= radx;
    left0.size.y= right0.size.y= left1.size.y= right1.size.y= rady;
    leftgapy.size.y= rightgapy.size.y= gapy;
    right_oct.pos.x= gapx;
    oval.y0= 0;
    oval.y1= h;
	
    radx--;
    rady--;
    
    t1= radx * radx; t2= t1 + t1;
    t3= rady * rady; t4= t3 + t3;
    t5= radx * t3; t6= t5 + t5; t7= 0;
    t8= radx + rady;
    d= (t1+t3)/4-t5;
    
    for (i= 0; i < 100; i++)
	shallowsteep[i]= steepshallow[i]= 0;

    setbit(shallowsteep, 0);
    for (m= 1, n= t8; m <= t8; m++, n--) {
	if (d < 0) {
	    t7+= t2;
	    d+= t7;
	} else {
	    setbit(steepshallow, m);
	    setbit(shallowsteep, n);
	    t6-= t4;
	    d-= t6;
	}
    }
    return TRUE;
}

static struct pixrect *MakePen(int psz)
{
    struct pixrect *pr= mem_create(psz, psz, 1);
    int gap, rad;
    
    rad= psz/2;
    gap= psz - 2*rad;
    MakeQuadrant(rad, rad, gap, gap, psz);
    pr_traprop(pr, rad, 0, oval, PIX_SET|PIX_DONTCLIP, 0, 0, 0);
    return pr;
}

void SunWindowPort::DevFillRRect2(Rectangle *r, Point dia)
{
    Point rad= dia/2;
    Point gap= r->extent - 2*rad;

    MakeQuadrant(rad.x, rad.y, gap.x, gap.y, r->extent.y);
    lock(r);
    Traprop(r->origin.x, r->origin.y, r->extent.x, r->extent.y, rad.x);
    unlock();
}

void SunWindowPort::DevStrokePolygon2(Rectangle *r, Point* p, int n, GrPolyType, int psz, GrLineCap)
{
    register int i, dx, dy, x, y, x0, y0, x1, y1, ox, oy;
    bool xormode= (ink == eInkXor);
    struct pixrect *pr1= 0;
    Rectangle rr;
    
    if (psz <= 0)
	psz= 1;

    if (psz > 1 || pat != 0) {
	rr= r->Expand(psz/2+1);
	NewPixrect(&tmppr, rr.extent, FALSE, TRUE, 1);
	pr1= tmppr;
	ox= oy= 0;
    } else {
	ox= r->origin.x;
	oy= r->origin.y;
	lock(r);
    }
    
    x0= p[0].x + ox;
    y0= p[0].y + oy;
    
    for (i= 1; i < n; i++) {
	x= x1= p[i].x + ox;
	y= y1= p[i].y + oy;
	
	if (xormode && i < n-1) {
	    dx= x1-x0;
	    dy= y1-y0;
	    if (dx >= abs(dy))
		x--;
	    else if (-dx >= abs(dy))
		x++;
	    if (dy >= abs(dx))
		y--;
	    else if (-dy >= abs(dx))
		y++;
	}
	if (pr1 == 0)
	    Vector(x0, y0, x1, y1);
	else
	    Bresenham(pr1, x0, y0, x, y, psz);
	x0= x1;
	y0= y1;
    }
    if (pr1)
	Stencil(&rr, pr1);
    else
	unlock();
}

void SunWindowPort::DevFillPolygon2(Rectangle *r, Point* pts, int npts, GrPolyType)
{
    struct pr_pos p[1000];
    register int i;

    for (i= 0; i < npts; i++) {
	p[i].x= pts[i].x;
	p[i].y= pts[i].y;
    }

    lock(r);
    Polygon(npts, p, r->origin.x, r->origin.y);
    unlock();
}

void SunWindowPort::DevShowBitmap(Rectangle *r, Bitmap *b)
{
    SunBitmap *dbm= (SunBitmap*) b->GetDevBitmap();

    dbm->DevScaleBitmap(r->extent.x, r->extent.y);
    
    if (dbm->cmap) {
	if (cmap && dbm->GetCmapSeq() < cmseq)
	    dbm->MapColors(this, cmap, cmseq);
	if (pr)
	    pr_rop(pr, r->origin.x+fsorigin.x, r->origin.y+fsorigin.y,
			    r->extent.x, r->extent.y, PIX_SRC, dbm->Pr(), 0, 0);
	else
	    pw_rop(pw, r->origin.x, r->origin.y, r->extent.x, r->extent.y,
						      PIX_SRC, dbm->Pr(), 0, 0);
    } else
	Stencil(r, dbm->Pr());
}

bool SunWindowPort::DevShowChar(Font *fdp, Point, byte c, bool isnew, Point pos)
{
    static int lastdx, lastdy;
    register struct pixchar *pcc;
    register int dx, dy;
    
    if (isnew) {
	bp= bat;
	lastdx= lastdy= 0;
    } else if (bp-bat >= MaxTextBatchCnt)
	return TRUE;
	
    pcc= &((SunFont*)fdp)->pf->pf_char[c];
    if (pcc->pc_pr == 0)
	fdp->CheckChar(c);
    bp->pr= pcc->pc_pr;
    dx= pos.x + pcc->pc_home.x;
    dy= pos.y + pcc->pc_home.y;
    bp->pos.x= dx - lastdx;
    bp++->pos.y= dy - lastdy;
    lastdx= dx;
    lastdy= dy;
    
    return FALSE;
}

void SunWindowPort::DevShowTextBatch(Rectangle *r, Point)
{
    int n= bp-bat;

    if (n <= 0)
	return;

    bat[0].pos.x-= r->origin.x;
    bat[0].pos.y-= r->origin.y;
    
    if (ink == eInkPat || ink == eInkColor) {
	NewPixrect(&tmppr, r->extent, FALSE, TRUE, 1);
	pr_batchrop(tmppr, 0, 0, PIX_OR|PIX_DONTCLIP, bat, n);
	Stencil(r, tmppr);
    } else {
	lock(r);
	if (pr)
	    pr_batchrop(pr, r->origin.x+fsorigin.x, r->origin.y+fsorigin.y, tmode, bat, n);
	else
	    pw_batchrop(pw, r->origin.x, r->origin.y, tmode, bat, n);
	unlock();
    }
}

void SunWindowPort::DevGiveHint(int code, int, void *vp)
{
    struct rect *r= (struct rect*) vp;
    Rectangle *rr= (Rectangle*) vp;
    
    switch (code) {
    case eHintLock:
	if (! inbatch)
	    pw_lock(pw, r);
	break;
	
    case eHintUnlock:
	if (! inbatch)
	    pw_unlock(pw);
	break;
	
    case eHintBatch:
	if (inbatch || overlay)
	    return;
	inbatch= TRUE;
	batchrect= *r;
	NewPixrect(&tmppr2, rr->origin + rr->extent, FALSE, FALSE, gDepth);
	pr= opr= tmppr2;
	fsorigin= gPoint0;
	break;
	
    case eHintUnbatch:
	if (! inbatch || overlay)
	    return;
	inbatch= FALSE;
	pw_rop(pw, batchrect.r_left, batchrect.r_top,
			batchrect.r_width, batchrect.r_height, PIX_SRC,
				    tmppr2, batchrect.r_left, batchrect.r_top);
	pr= opr= 0;
	fsorigin= gPoint0;
	break;
	
    default:
	break;
    }
}

void SunWindowPort::DevSetCursor(GrCursor c)
{
    int ic= (int) c;
    struct _cursor curs;

    curs.cur_function= gColor ? PIX_OR : (PIX_SRC ^ PIX_DST); // good idea ????
    if (ic <= 0) {
	curs.cur_shape= 0;
	curs.cur_yhot= curs.cur_xhot= 0;
    } else {
	curs.cur_xhot= -cursors->pf_char[ic-1].pc_home.x;
	curs.cur_yhot= -cursors->pf_char[ic-1].pc_home.y;
	curs.cur_shape= cursors->pf_char[ic-1].pc_pr;
    }
    curs.flags= 0;
    _sunwindow_setcursor(fd, &curs);
}

static void grab(int fd, bool m)
{
    static bool fdgrab[FD_SETSIZE];

    if ((fdgrab[fd] != 0) != (m != 0)) {
	if (m) {
	    fdgrab[fd]= TRUE;
	    win_grabio(fd);
	} else {
	    fdgrab[fd]= FALSE;
	    win_releaseio(fd);
	}
    }
}

void SunWindowPort::DevGrab(bool m, bool fs)
{
    if (overlay)
	return;
    
    if (fs) {
	if (m) {
	    if ((FS= fullscreen_init(fd)) == NULL)
		Fatal("SunWindowPort::Fullscreen", "can't get fullscreen access");
	    pw= FS->fs_pixwin;
	} else {
	    fullscreen_destroy(FS);
	    pw= opw;
	    FS= 0;
	}
    } else
	grab(fd, m);
}

void SunWindowPort::DevScrollRect(Rectangle r, Point p)
{
    register int l= r.origin.x, t= r.origin.y;
    bool hard= FALSE;
    
    if (hard= (p.x != 0 && p.y != 0))
	NewPixrect(&tmppr2, r.extent, FALSE, FALSE, gDepth);
    
    lock(&cliprect);
    if (overlay) {
	l+= fsorigin.x;
	t+= fsorigin.y;
	if (hard) {
	    pr_rop(tmppr2, 0, 0, r.extent.x, r.extent.y, PIX_SRC, pr, l-p.x,t-p.y);
	    pr_rop(pr, l, t, r.extent.x, r.extent.y, PIX_SRC, tmppr2, 0, 0);
	} else
	    pr_rop(pr, l, t, r.extent.x, r.extent.y, PIX_SRC, pr, l-p.x, t-p.y);
    } else {
	register struct rectnode *rlp;
	struct rect rl[20];
	int i, j;
	
	if (hard)
	    pw_read(tmppr2, 0, 0, r.extent.x, r.extent.y, PIX_SRC, pw, l-p.x, t-p.y);
	else
	    pw_copy(pw, l, t, r.extent.x, r.extent.y, PIX_SRC, pw, l-p.x, t-p.y);

	for(i= 0, rlp= pw->pw_fixup.rl_head; rlp; rlp= rlp->rn_next, i++) {
	    rl[i]= rlp->rn_rect;
	    rl[i].r_left+= l;
	    rl[i].r_top+= t;
	    if (p.x > 0)
		rl[i].r_left-= p.x;
	    if (p.y > 0)
		rl[i].r_top-= p.y;
	}
	if (hard)
	    pw_rop(pw, l, t, r.extent.x, r.extent.y, PIX_SRC, tmppr2, 0, 0);
	for(j= 0; j < i; j++)
	    Damage(eFlgDamage3, (Rectangle*) &rl[j]);
    }
    unlock();
}

void SunWindowPort::DevTop(bool top)
{
    if (! overlay) {
	if (top)
	    wmgr_top(fd, rootFd);
	else
	    wmgr_bottom(fd, rootFd);
    }
}

void SunWindowPort::DevShow(WindowPort *FP, Rectangle R)
{
    SunWindowPort *fp= (SunWindowPort*) FP;
    struct rect r, fsr, rr;
    
    r= *R2r(&R);
    
    fatherport= fp;
    if (cmap == 0)
	cmap= Cmap;
    
    overlayport= 0;
    if (! overlay) {
	next= allwindows;
	allwindows= this;

	myrect.extent= gPoint_1;
	win_setrect(fd, &r);

	/*
	 * reset the links
	 */
	win_setlink(fd, WL_COVERING, WIN_NULLLINK); 
	win_setlink(fd, WL_OLDESTCHILD, WIN_NULLLINK); 
	win_setlink(fd, WL_YOUNGESTCHILD, WIN_NULLLINK); 
	win_setlink(fd, WL_PARENT, parentNo);
	win_setlink(fd, WL_COVERED, topmostNo);
	win_insert(fd);

	opw= pw= pw_open(fd);
	DevSetCursor(cursor);
	pw->pw_clipdata->pwcd_flags |= PWCD_SET_CMAP_SIZE;
	if (cmap)
	    cmap->changed= TRUE;
	flushcolormap();
	_sunwindow_setmask(fd);
	return;
    }
    if (fp == 0)
	Fatal("SunWindowPort::DevShow", "fatal error (fp == 0)");
    overlayport= this;
    fd= fp->fd;
    /* cursor= fp->cursor; */

    if (FS == 0 || (fd != FS->fs_windowfd))
	if ((FS= fullscreen_init(fd)) == NULL)
	    Fatal("SunWindowPort::DevShow", "can't get fullscreen access");

    pr= opr= pr_region(FS->fs_pixwin->pw_pixrect, r.r_left, r.r_top, r.r_width, r.r_height);
    fsorigin= gPoint0;
    
    rr= r;
    r.r_left+= FS->fs_screenrect.r_left;
    r.r_top+= FS->fs_screenrect.r_top;
    fsr= r;

    opw= pw= pw_region(FS->fs_pixwin, r.r_left, r.r_top, r.r_width, r.r_height);
    inpOffset.x= -r.r_left;
    inpOffset.y= -r.r_top;

    fsr.r_left= fsr.r_top= 0;

    pw_lock(pw, &fsr);
    savepr= pw_save_pixels(fp->pw, &r);  /* now save the pixels */
    /* reset cursor (in lock -> cursor is invisible) */
    myrect= *r2R(&rr);
    Damage(eFlgDamage1, (Rectangle*) &rr);
    if (fp->cursor != cursor)
	DevSetCursor(cursor);
    pw_unlock(pw);
}

void SunWindowPort::DevHide()
{
    if (overlay) {
	struct rect rr;
	struct SunWindowPort *fp= fatherport;
	
	/* restore the pixels */
	rr= savepr->r;
	rr.r_left= rr.r_top= 0;
	pw_lock(pw, &rr);
	pw_restore_pixels(pw, savepr);
	if (fp->overlay && (fp->cursor != cursor))
	    DevSetCursor(fp->cursor);
	pw_unlock(pw);
	
	if (! fp->overlay) {
	    fullscreen_destroy(FS);
	    FS= 0;
	}

	if (rpw)
	    pw_close(rpw);
	if (opw)
	    pw_close(opw);
	rpw= pw= opw= 0;
	if (rpr)
	    pr_close(rpr);
	if (opr)
	    pr_close(opr);
	rpr= pr= opr= 0;
	
    } else {
	if (allwindows == this)
	    allwindows= next;
	else {
	    register struct SunWindowPort *wp;
	    for (wp= allwindows; wp; wp= wp->next) {
		if (wp->next == this) {
		    wp->next= next;
		    break;
		}
	    }
	}
	next= 0;
	win_remove(fd);
    }
    overlayport= 0;
}

void SunWindowPort::DevSetExtent(Point e)
{
    struct rect oldr, newr;
    if (overlay)
	return;
    win_getrect(fd, &oldr);
    newr= oldr;
    newr.r_width= e.x;
    newr.r_height= e.y;
    if (wmgr_iswindowopen(fd))
	wmgr_completechangerect(fd, &newr, &oldr, 0, 0);
    else
	win_setrect(fd, &newr);
}

void SunWindowPort::DevSetOrigin(Point o)
{
    struct rect oldr, newr;
    if (overlay)
	return;
    win_getrect(fd, &oldr);
    newr= oldr;
    newr.r_left= o.x;
    newr.r_top= o.y;
    if (wmgr_iswindowopen(fd))
	wmgr_completechangerect(fd, &newr, &oldr, 0, 0);
    else
	win_setrect(fd, &newr);
}

Rectangle SunWindowPort::DevGetRect()
{
    struct rect s;
    
    if (overlay)
	return myrect;
    win_getrect(fd, &s);
    return Rectangle(s.r_left, s.r_top, s.r_width, s.r_height);
}

static bool myread(int fd, int millisec, struct inputevent *ie)
{
    if (gSystem->CanRead(fd, millisec))
	return TRUE;
    if (ie)
	input_readevent(fd, ie);
    return FALSE;
}

void SunWindowPort::DevGetEvent(Token *t, int timeout, bool overread)
{
    struct inputevent e, *ie= &e;
    bool first, firstpfk;

    firstpfk= TRUE;
    
    flushcolormap();
Restart:
    t->Code= eEvtNone;
    t->Flags= 0;
    
    if (overread) {
	first= TRUE;
	do {
	    if (myread(fd, timeout, ie))
		break;
	    first= FALSE;
	} while (event_id(ie) == eEvtLocMove || event_id(ie) == eEvtLocMoveBut);
	if (first)
	    return;    /* no events in queue */
    } else if (myread(fd, timeout, ie))
	return;    /* no events in queue */
	
    t->Code= event_id(ie);
    t->At= event_time(ie).tv_sec * 1000 + event_time(ie).tv_usec / 1000;
    t->Pos= Point(event_x(ie), event_y(ie)) + inpOffset;
    
    if (event_shift_is_down(ie))
	t->Flags |= (int)eFlgShiftKey;
    if (event_ctrl_is_down(ie))
	t->Flags |= (int)eFlgCntlKey;
    if (event_meta_is_down(ie)) 
	t->Flags |= (int)eFlgMetaKey;
    if (win_inputnegevent(ie)) 
	t->Flags |= (int)eFlgButDown;
	
    if (t->Code == eEvtLocMoveBut)
	t->Code= eEvtLocMove;
    else if (t->Code >= eEvtPfkFirst && t->Code <= eEvtPfkLast) {
	if (firstpfk) {
	    firstpfk= FALSE;
	    goto Restart;
	} else
	    t->Flags &= ~((int)eFlgButDown);
    }
    
    /* map cursor key escape sequences to one input token
     * (cannot map the keyboard, because mapping affects all windows !!)
     * check whether a cursor sequence (ESC '[' ( 'A' | 'B' | 'C' | 'D' ) ) occured
     */
    if (t->Code == 033) {
Start: 
	for(;;)
	    if (myread(fd, 0, ie) || event_id(ie) != 033)
		break;  /* sometimes more ESC are received */
	if (event_id(ie) != '[') {
	    t->Code= event_id(ie);
	    return;
	}
    
	for(;;) {
	    if (myread(fd, 0, ie))
		break;
	    if (event_id(ie) == 033)
		goto Start; /* the sequence restarts */
	}
	switch (event_id(ie)) {
	case 'A': 
	    t->Code= (int) eEvtCursorUp;
	    break;
	case 'B': 
	    t->Code= (int) eEvtCursorDown;
	    break;
	case 'C': 
	    t->Code= (int) eEvtCursorRight;
	    break;
	case 'D': 
	    t->Code= (int) eEvtCursorLeft;
	    break;
	default : 
	    t->Code= event_id(ie);
	    break;
	}
    }
}

void SunWindowPort::DevSetMousePos(Point p, bool m)
{
    _sunwindow_setmouse(fd, p.x, p.y, m);
}

void SunWindowPort::DevBell(long wait)
{   
    struct timeval t;                 
    
    t.tv_sec= wait / 1000;
    t.tv_usec= (wait % 1000) * 1000;
    win_bell(fd, t, 0);
}

void SunWindowPort::DevSetPattern(DevBitmap *b)
{
    SunBitmap *bm= (SunBitmap*) b;
    ink= eInkPat;
    tmode= cmode= PIX_SRC;
    patbm= bm;
    pat= GetPattern(gPoint0);
    patorigin= bm->size;
}

void SunWindowPort::DevSetOther(int code)
{
    patbm= 0;
    pat= 0;
    patorigin= gPoint16;

    switch (code) {
    case -1:
	ink= eInkXor;
	tmode= PIX_SRC ^ PIX_DST;
	cmode= PIX_NOT(PIX_DST);
	break;
    case 0:
	ink= eInkNone;
	tmode= cmode= PIX_DST;
	break;
    case 1:
	ink= eInkWhite;
	tmode= PIX_NOT(PIX_SRC) & PIX_DST;
	cmode= PIX_CLR;
	break;
    case 2:
	ink= eInkBlack;
	tmode= PIX_OR;
	cmode= PIX_SET;
	break;
    }
}

bool SunWindowPort::DevSetColor(RGBColor *cp)
{
    if (! HasColor()) {
	DevSetGrey(cp->AsGreyLevel() / 255.0);
	return TRUE;
    }
    if (cmap == 0)
	return FALSE;
    ink= eInkColor;
    patbm= 0;
    pat= 0;
    patorigin= gPoint16;
    if (cp->GetPrec() == 255) {
	register long id= cp->GetId();
	if (id < 0) {
	    int i, minuse= 0x7ffffff0;
	    
	    if (cmap == Cmap)
		cmap= new suncolormap(Cmap);
	    
	    for (i= 2; i < cmap->length-1; i++) {
		if (cmap->type[i])
		    continue;
		if (cmap->usecnt[i] < minuse) {
		    minuse= cmap->usecnt[i];
		    id= i;
		}
	    }
	    cp->SetId(id);
	    cmap->type[id]= 1;
	    cmap->usecnt[id]++;
	}
	cmap->red[id]= cp->GetRed();
	cmap->green[id]= cp->GetGreen();
	cmap->blue[id]= cp->GetBlue();
	cmap->changed= TRUE;
    }
    if (cp->GetId() < 0)
	cp->SetId(cmap->mapcolor(cp->GetRed(), cp->GetGreen(), cp->GetBlue()));
    tmode= cmode= PIX_SRC | PIX_COLOR((int)cp->GetId());
    return FALSE;
}

