//$BoxShape$
#include "BoxShape.h"

//---- Box Shape Methods -------------------------------------------------------

MetaImpl0(BoxShape);

static short BoxImage[]= {
#   include  "images/BoxShape.im"
};

BoxShape::BoxShape()
{
}

short *BoxShape::GetImage()
{
    return BoxImage;
}

bool BoxShape::ContainsPoint(Point p)
{
    if (ink == 0)
	return (bool) (! bbox.Inset(pensize+2).ContainsPoint(p));
    return TRUE;
}

void BoxShape::Draw(Rectangle r)
{
    Shape::Draw(r);
    GrFillRect(bbox);
    GrStrokeRect(bbox);
}

void BoxShape::Outline(Point p1, Point p2)
{
    Shape::Outline(p1, p2);
    GrStrokeRect(NormRect(p1, p2));
}
