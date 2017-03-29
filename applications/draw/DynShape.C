//$DynShape$
#include "DynShape.h"

//---- Dyn Shape ---------------------------------------------------------------

MetaImpl0(DynShape);

static short DynImage[]= {
#   include  "images/DynShape.im"
};

DynShape::DynShape()
{
}
    
short *DynShape::GetImage()
{
    return DynImage;
}

void DynShape::Draw(Rectangle r)
{
    GrFillRect(bbox);
    GrStrokeRect(bbox);
    GrLine(bbox.NW(), bbox.SE());
    GrLine(bbox.SW(), bbox.NE());
    /*
    Shape::Draw(r);
    GrFillRect(bbox);
    GrStrokeRect(bbox);
    */
}

void DynShape::Outline(Point p1, Point p2)
{
    Shape::Outline(p1, p2);
    GrStrokeRect(NormRect(p1, p2));
    /*
    Rectangle rr= NormRect(p1, p2);
    GrStrokeRect(rr);
    GrLine(rr.NW(), rr.SE());
    GrLine(rr.NE(), rr.SW());
    */
}
