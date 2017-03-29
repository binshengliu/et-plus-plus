//$PictureShape$
#include "PictureShape.h"
#include "Picture.h"

//---- PictureShape ------------------------------------------------------------

MetaImpl(PictureShape, (TP(pict), 0));

PictureShape::PictureShape(Picture *p)
{
    pict= p;
    bbox= pict->bbox;
}

PictureShape::~PictureShape()
{
    SafeDelete(pict);
}

void PictureShape::Draw(Rectangle)
{
    GrShowPicture(bbox, pict);
}

void PictureShape::Outline(Point p1, Point p2)
{
    Shape::Outline(p1, p2);
    GrStrokeRect(NormRect(p1, p2));
}

ostream& PictureShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << pict;
}

istream& PictureShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    SafeDelete(pict);
    return s >> pict;
}
