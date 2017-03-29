//$LineShape$
#include "LineShape.h"
#include "DrawView.h"

//---- LineShape Methods -------------------------------------------------------

static short LineImage[]= {
#   include  "images/LineShape.im"
};

MetaImpl(LineShape, (T(p1), T(p2), 0));

LineShape::LineShape()
{
}

short *LineShape::GetImage()
{
    return LineImage;
}

Rectangle LineShape::GetSpan()
{
    return Rectangle(p1, p2);
}

void LineShape::SetSpan(Rectangle r)
{
    p1= r.origin;
    p2= r.extent;
    Shape::SetSpan(r);
}

void LineShape::Moveby(Point delta)
{
    p1+= delta;
    p2+= delta;
    Shape::Moveby(delta);
}

void LineShape::Flip(int n)
{
    int t;
    Point pt;
    
    switch (n) {
    case 1:     // vertical
	t= p1.y;
	p1.y= p2.y;
	p2.y= t;
	break;
    case 2:     // horizontal
	t= p1.x;
	p1.x= p2.x;
	p2.x= t;
	break;
    case 3:
	Swap(p1, p2);
	break;  // horizontal & vertical
    default:
	return;
    }
    Changed();
}

void LineShape::Init(Point pp1, Point pp2)
{
    Shape::Init(p1= pp1, p2= pp2);
}

int LineShape::PointOnHandle(Point p)
{
    int n= Shape::PointOnHandle(p);
    if (n == 0)
	return 0;
    if (n == 1)
	return 7;
    return -1;
}

Rectangle LineShape::InvalRect()
{
    if (arrows != eDefaultCap)
	return bbox.Expand(Max(HandleSize,Point(4*pensize))/2+1);
    return bbox.Expand(Max(HandleSize,Point(pensize))/2+1);
}

Point *LineShape::MakeHandles(int *n)
{
    spts[0]= p1-bbox.origin;
    spts[1]= p2-bbox.origin;
    *n= 2;
    return spts;
}

Point LineShape::GetConnectionPoint(Point p)
{
    if (p == gPoint0)
	return Shape::GetConnectionPoint(p);
    if (Length(p1-p) < Length(p2-p))
	return p1;
    return p2;
}

Point LineShape::Chop(Point p)
{
    return p;
}

Rectangle LineShape::GetTextRect()
{
    if (p1.x == p2.x)       // vertical
	return Rectangle(bbox.Center(), 0).Expand(Point(50, 0));
	
    if (p1.y == p2.y) {     // horizontal
	Rectangle r= bbox;
	r.origin= p1;
	r.origin.y+= 10;
	return r;
    }
    return Shape::GetTextRect();                    
}

bool LineShape::ContainsPoint(Point p)
{
    return PointAtLine(p, p1, p2);
}

void LineShape::Outline(Point p1, Point p2)
{
    GrLine(p1, p2);
}

void LineShape::Draw(Rectangle)
{
    GrLine(p1, p2);
}

bool LineShape::PointAtLine(Point p, Point p1, Point p2)
{
    int dx= p2.x - p1.x, dy= p2.y - p1.y, a1, a2;
    Point delta= Max(HandleSize,Point(pensize))/2+1;
    
    if (! NormRect(p1, p2).Expand(delta).ContainsPoint(p))
	return FALSE;
    if (dx == 0 || dy == 0)
	return TRUE;
	
    a1= (dy*(p.x-p1.x)/dx)+p1.y-p.y;
    a2= (dx*(p.y-p1.y)/dy)+p1.x-p.x;
    
    return abs(a1) <= delta.x || abs(a2) <= delta.y;
}

ostream& LineShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << p1 SP << p2 SP;
}

istream& LineShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    return s >> p1 >> p2;
}
