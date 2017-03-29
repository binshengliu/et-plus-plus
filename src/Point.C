//$Point,PointClass$
#define FakeExternalDecl
#include "Point.h"
#include "Class.h"
#include "Math.h"
#include "String.h"

/* In order to become independent from the order of global constructors
   we fake C++ by using plain old C (which gets initialized by the loader)
*/

struct C_Point {
    short x, y;
};

C_Point gPoint_1 = { -1, -1 },
	gPoint0,
	gPoint1 = { 1, 1 },
	gPoint2 = { 2, 2 },
	gPoint3 = { 3, 3 },
	gPoint4 = { 4, 4 },
	gPoint8 = { 8, 8 },
	gPoint10 = { 10, 10 },
	gPoint16 = { 16, 16 },
	gPoint32 = { 32, 32 },
	gPoint64 = { 64, 64 };

SimpleMetaImpl(Point)
{
    sprintf(buf, "%s", ((Point*) addr)->AsString());
}

Point Min(Point p1, Point p2)
{
    return Point(min(p1.x, p2.x), min(p1.y, p2.y));
}

Point Max(Point p1, Point p2)
{ 
    return Point(max(p1.x, p2.x), max(p1.y, p2.y));
}

Point Range (Point lb, Point ub, Point x)
{
    return Point(range(lb.x, ub.x, x.x), range(lb.y, ub.y, x.y));
}

Point Point::Half() const
{
    return Point(x >> 1, y >> 1);
}

Point Half (Point p)
{
    return Point(p.x >> 1, p.y >> 1);
}

Point Abs(Point p)
{
    return Point(abs(p.x), abs(p.y));
}

Point Sign(Point p)
{
    return Point(sign(p.x), sign(p.y));
}

Point Scale(Point p, Point num, Point denom)
{
    Point r;
    
    if (denom.x)
	r.x= (int)((float)(p.x*num.x) / (float)denom.x + 0.5);
    if (denom.y)
	r.y= (int)((float)(p.y*num.y) / (float)denom.y + 0.5);
	
    return r;
}

Point Scale(Point p, float sx, float sy)
{
    return Point((float)((p.x*sx)+0.5), (float)((p.y*sy)+0.5));
}

float Phi(Point p)
{
    return Atan2(p.y, p.x);
}

float Length(Point p)
{
    return (float) Hypot(p.x, p.y);
}

Point PolarToPoint(double ang, double fx, double fy)
{
    double si, co;
    Sincos(ang, &si, &co);
    return Point((int)(fx*co+0.5), (int)(fy*si+0.5));
}

Point polartopoint(double ang, double fx, double fy)
{
    double si, co;
    Sincos(ang, &si, &co);
    return Point((int)(fx*co+0.5), (int)(fy*si+0.5));
}

void Swap(Point &p1, Point &p2)
{
    Point tmp= p1;
    p1= p2;
    p2= tmp;
} 
  
ostream& operator<< (ostream& s, Point p)
{
    s.put('(');
    s << p.x;
    s.put(',');
    s << p.y;
    s.put(')');
    return s;
}

istream& operator>> (istream& s, Point& p)
{
    int x, y;
    char c= 0;
    
    s >> c;
    if (c == '(') {
	s >> x >> c;
	if (c == ',') {
	    s >> y >> c;
	    if (c != ')')
		s.setstate(_bad);
	} else
	    s.setstate(_bad);
    } else
	s.setstate(_bad);
    if (s)
	p= Point(x,y);
    return s;
}

char *Point::AsString() const
{
    return form("x: %d y: %d", x, y);
}

