//$ImageShape$
#include "ImageShape.h"

//---- Image Shape --------------------------------------------------------------

MetaImpl(ImageShape, (TP(bm), 0));

ImageShape::ImageShape(Bitmap *b)
{
    bm= b;
    bbox= Rectangle(gPoint0, bm->Size());
}

ImageShape::~ImageShape()
{
    SafeDelete(bm);
}

void ImageShape::Draw(Rectangle)
{
    GrFillRect(bbox);
    GrPaintBitMap(bbox, bm, InkPalette[penink]);
}

void ImageShape::Outline(Point p1, Point p2)
{
    Shape::Outline(p1, p2);
    GrStrokeRect(NormRect(p1, p2));
}

ostream& ImageShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << bm;
}

istream& ImageShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    SafeDelete(bm);
    return s >> bm;
}
