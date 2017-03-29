//$Rectangle,RectangleClass$
#include "Rectangle.h"
#include "Class.h"
#include "Error.h"
#include "Math.h"
#include "String.h"

const Rectangle gRect0;

SimpleMetaImpl(Rectangle)
{
    sprintf(buf, "%s", ((Rectangle*) addr)->AsString());
}

Rectangle::Rectangle()
{
    origin.x= origin.y= extent.x= extent.y= 0;
}

Rectangle::Rectangle(short x, short y, short w, short h)
{
    origin.x= x;
    origin.y= y;
    extent.x= w;
    extent.y= h;
}

Rectangle::Rectangle(Point o, Point e)
{
    origin= o;
    extent= e;
}
    
Rectangle::Rectangle(Point e)
{
    origin= 0;
    extent= e;
}

Rectangle::Rectangle(short w, short h)
{   
    origin= 0;
    extent.x= w;
    extent.y= h;
}

Rectangle NormRect(Point p1, Point p2)
{
    Rectangle r;
    
    r.origin.x= min(p1.x, p2.x);
    r.origin.y= min(p1.y, p2.y);
    r.extent.x= max(p1.x, p2.x)+1-r.origin.x;
    r.extent.y= max(p1.y, p2.y)+1-r.origin.y;
    return r;
}

bool Rectangle::Intersects(const Rectangle &r) const
{
    return origin.x < r.origin.x + r.extent.x
		    && r.origin.x < origin.x + extent.x
		    && origin.y < r.origin.y + r.extent.y
		    && r.origin.y < origin.y + extent.y;
}

bool Rectangle::ContainsRect(const Rectangle &r) const
{
    Point c1= origin + extent, c2= r.origin + r.extent;
    
    return    origin.x <= r.origin.x && c1.x >= c2.x
	   && origin.y <= r.origin.y && c1.y >= c2.y;
}

bool Rectangle::OvalContainsRect(const Rectangle &r) const
{
    int rad= min(extent.x, extent.y)/2;
    Point c= Center();
    
    if (Length(r.NW()-c) >= rad)
	return FALSE;
    if (Length(r.NE()-c) >= rad)
	return FALSE;
    if (Length(r.SW()-c) >= rad)
	return FALSE;
    if (Length(r.SE()-c) >= rad)
	return FALSE;
    return TRUE;
}

Rectangle Rectangle::Merge(const Rectangle &r)
{ 
    register int x1, x2, y1, y2;
    
    if (IsEmpty())
	return *this= r;
    if (r.IsEmpty())
	return *this;
    x1= origin.x + extent.x; 
    y1= origin.y + extent.y; 
    x2= r.origin.x + r.extent.x; 
    y2= r.origin.y + r.extent.y; 
    origin.x= min(origin.x, r.origin.x);
    origin.y= min(origin.y, r.origin.y);
    extent.x= max(x1, x2) - origin.x;
    extent.y= max(y1, y2) - origin.y;
    return *this;
}

Rectangle Union(const Rectangle &r1, const Rectangle &r2)
{
    Rectangle rr= r1;
    return rr.Merge(r2);
}

bool Rectangle::Clip(const Rectangle &r)
{
    register int p1x, p1y, p2x, p2y;
    
    if (IsEmpty())
	return FALSE;
    if (r.IsEmpty()) { 
	*this= gRect0; // g++ 1.37.2.beta bug on 68020 cpu ?
	return FALSE;
    }
    p1x= origin.x + extent.x;
    p1y= origin.y + extent.y;
    p2x= r.origin.x + r.extent.x;
    p2y= r.origin.y + r.extent.y;
    origin.x= max(origin.x, r.origin.x);
    origin.y= max(origin.y, r.origin.y);
    extent.x= min(p1x, p2x) - origin.x;
    extent.y= min(p1y, p2y) - origin.y;
    return IsNotEmpty();
}

Rectangle Inter(const Rectangle &r1, const Rectangle &r2)
{
    Rectangle rr= r1;
    rr.Clip(r2);
    return rr;
}

Rectangle Rectangle::Intersect(const Rectangle &r)
{
    Clip(r);
    return *this;
}

Rectangle BoundingBox(int npts, Point *pts, Point *np)
{
    Point min= pts[0], max= pts[0];
    Rectangle r;
    register int i;
    
    if (np == 0)
	np= pts;
    
    for (i= 1; i<npts; i++) {
	min= Min(min, pts[i]);
	max= Max(max, pts[i]);
    }
    r.origin= min;
    r.extent= max-min+1;
    for (i= 0; i<npts; i++)
	np[i]= pts[i]-min;
	
    return r;
}

Rectangle Rectangle::WedgeBBox(int s, int d)
{
    Point mi, ma, p;
    register int i;
    
    if (d >= 270)
	return *this;
    
    mi= ma= Center();
    p= AngleToPoint(s);
    mi= Min(p, mi);
    ma= Max(p, ma);
    i= 1;
    for (i= 1; i < d-1; i++) {
	switch (s+i) {
	case 0:
	    mi.y= origin.y;
	    break;
	case 90:
	    ma.x= origin.x + extent.x;
	    break;
	case 180:
	    ma.y= origin.y + extent.y;
	    break;
	case 270:
	    mi.x= origin.x;
	    break;
	default:
	    continue;
	}
    }
    p= AngleToPoint(s+d);
    return NormRect(Min(p, mi), Max(p, ma));
}

int Difference(Rectangle *rp, const Rectangle &r1, const Rectangle &ra)
{
    register int p2x, p2y;
    Rectangle r2, dr[4];
    int n, i;

    r2= Inter(r1, ra);
    if (r2.IsNotEmpty()) {
	p2x= r2.origin.x+r2.extent.x;
	p2y= r2.origin.y+r2.extent.y;
	
	dr[0].origin= r1.origin;
	dr[0].extent.x= r1.extent.x;
	dr[0].extent.y= r2.origin.y-r1.origin.y;
	
	dr[1].origin.x= r1.origin.x;
	dr[1].origin.y= r2.origin.y;
	dr[1].extent.x= r2.origin.x-r1.origin.x;
	dr[1].extent.y= r2.extent.y;
	
	dr[2].origin.x= p2x;
	dr[2].origin.y= r2.origin.y;
	dr[2].extent.x= r1.origin.x+r1.extent.x-p2x;
	dr[2].extent.y= r2.extent.y;
	
	dr[3].origin.x= r1.origin.x;
	dr[3].origin.y= p2y;
	dr[3].extent.x= r1.extent.x;
	dr[3].extent.y= r1.origin.y+r1.extent.y-p2y;
    } else 
	dr[0]= r1;
    
    for (n= i= 0; i<4; i++)
	if (dr[i].IsNotEmpty())
	    rp[n++]= dr[i];
    return n;
}

Rectangle Rectangle::Inset(Point p)
{
    Rectangle r;
    
    r.origin.x= origin.x + p.x;
    r.origin.y= origin.y + p.y;
    r.extent.x= extent.x - 2 * p.x;
    r.extent.y= extent.y - 2 * p.y;
    return r;
}

Rectangle Rectangle::Expand(Point p)
{
    Rectangle r;
    
    r.origin.x= origin.x - p.x;
    r.origin.y= origin.y - p.y;
    r.extent.x= extent.x + 2 * p.x;
    r.extent.y= extent.y + 2 * p.y;
    return r;
}
	    
Point Rectangle::Constrain(Point p)
{
    return Max(Min(p, origin+extent), origin);
}
	    
Point ConstrainMoveRect(const Rectangle &r1, const Rectangle &r2, Point delta)
{
    Point p= r1.origin + r1.extent - r2.extent;
    return Max(r1.origin, Min(p, r2.origin+delta))-r2.origin;
}

Point Rectangle::AmountToTranslateWithin(const Rectangle &r) const
{
    Point delta(0), rUL= NW(), rLR= SE(), vrUL= r.NW(), vrLR= r.SE();
    
    for (int v= 0; v <= 1; v++) {
	if (rUL[v] < vrUL[v] && rLR[v] > vrLR[v])
	    continue;
	if (rLR[v] - vrUL[v] < extent[v])
	    delta[v]= vrUL[v] - rUL[v];
	if (vrLR[v] - rUL[v] < extent[v])
	    delta[v]= vrLR[v] - rLR[v];
    }
    return delta;
}

ostream& operator<<(ostream& s, const Rectangle &r)
{
    return s << "<" << r.origin << "," << r.extent << ">";
}                  

istream& operator>> (istream& s, Rectangle &r)
{
    Point o, e;
    char c= 0;
    
    s >> c;
    if (c == '<') {
	s >> o >> c;
	if (c == ',') {
	    s >> e >> c;
	    if (c != '>')
		s.setstate(_bad);
	} else
	    s.setstate(_bad);
    } else
	s.setstate(_bad);
    if (s)
	r= Rectangle(o,e);
    return s;
}

int Rectangle::PointToAngle(Point p) const
{
    p-= Center();
    return (int) intatan2(p.y*extent.x, p.x*extent.y);
}

Point Rectangle::OvalAngleToPoint(int ang) const
{
    return Center() + PolarToPoint(ang, extent.x/2, extent.y/2);
}

Point Rectangle::AngleToPoint(int a) const
{
    double si, co;
    register int x= 0, y= 0;

    Sincos(a, &si, &co);
    
    if (si != 0.0) {
	x= (int) ((1.0 + co/abs(si))/2.0 * extent.x);
	x= range(0, extent.x, x);
    } else if (co >= 0.0)
	x= extent.x;
    if (co != 0.0) {
	y= (int) ((1.0 + si/abs(co))/2.0 * extent.y);
	y= range(0, extent.y, y);
    } else if (si >= 0.0)
	y= extent.y;
    return origin + Point(x, y);
}

// Corners:
//         7|0|1
//        -------
//         6| |2
//        -------
//         5|4|3

int Rectangle::PointToCorner(Point p) const
{
    return ((PointToAngle(p)+22)/45) % 8;
}

Point Rectangle::CornerToPoint(int n) const
{
    return AngleToPoint((n % 8)*45);
}

char *Rectangle::AsString() const
{
    return form("x: %d y: %d w: %d h: %d", origin.x, origin.y, extent.x, extent.y);
}

