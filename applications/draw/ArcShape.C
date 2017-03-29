//$ArcShape$

#include "ArcShape.h"

//---- ArcShape ----------------------------------------------------------------

static short ArcImage[]= {
#   include  "images/ArcShape.im"
};

MetaImpl0(ArcShape);

ArcShape::ArcShape()
{
    startangle= 0;
    anglen= 90;
    ink= 0;
}

void ArcShape::Init(Point p1, Point p2)
{
    Shape::Init(p1, p2);
    if (p2.x > p1.x)
	startangle= (p2.y > p1.y) ? 0 : 90;
    else
	startangle= (p2.y > p1.y) ? 270 : 180;
    anglen= 90;
}

short *ArcShape::GetImage()
{
    return ArcImage;
}

bool ArcShape::CanSplit()
{
    return FALSE;
}

Point ArcShape::GetConnectionPoint(Point p)
{
    if (p == gPoint0)
	return Shape::GetConnectionPoint(p);
    Rectangle rr= RealRect(bbox, startangle);
    Point p1= rr.OvalAngleToPoint(startangle+anglen),
	  p2= rr.OvalAngleToPoint(startangle);
    if (Length(p1-p) < Length(p2-p))
	return p1;
    return p2;
}

Point ArcShape::Chop(Point p)
{
    return p;
}

Rectangle ArcShape::RealRect(Rectangle rr, int s)
{
    Point e= rr.extent;
    rr.extent+= e;
    
    switch (s) {
    case 0:
	rr.origin.x-= e.x;
	break;
    case 90:
	rr.origin-= e;
	break;
    case 180:
	rr.origin.y-= e.y;
	break;
    }
    return rr;
}
