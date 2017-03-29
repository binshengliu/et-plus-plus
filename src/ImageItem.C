//$ImageItem$

#include "ImageItem.h"

//---- ImageItem ---------------------------------------------------------------

MetaImpl(ImageItem, (T(base), TP(bmp), TB(deleteBitmap), 0));

ImageItem::ImageItem(Bitmap *b, int bs)
{
    deleteBitmap= FALSE;
    base= bs;
    bmp= b;
    SetFlag(eVObjHFixed | eVObjVFixed);
}

ImageItem::ImageItem(int id, Bitmap *b, int bs) : VObject(id)
{
    deleteBitmap= FALSE;
    base= bs;
    bmp= b;
    SetFlag(eVObjHFixed | eVObjVFixed);
}

ImageItem::ImageItem(short *s, Point e, int bs)
{
    deleteBitmap= TRUE;
    base= bs;
    bmp= new Bitmap(e, (u_short*)s);
    SetFlag(eVObjHFixed | eVObjVFixed);
}

ImageItem::~ImageItem()
{
    if (deleteBitmap)
	SafeDelete(bmp);
}

void ImageItem::SetBitmap(Bitmap *bm, bool redraw)
{
    if (deleteBitmap && bmp)
	delete bmp;
    deleteBitmap= FALSE;
    bmp= bm;
    SetContentRect(Rectangle(GetOrigin(), GetMinSize().extent), redraw);
}

Metric ImageItem::GetMinSize()
{
    return Metric(bmp->Size(), bmp->Size().y);
}

int ImageItem::Base()
{
    return base ? base : bmp->Size().y;
}

void ImageItem::Draw(Rectangle)
{
    GrPaintBitMap(contentRect, bmp, Enabled() ? ePatBlack : ePatGrey50);
}

void ImageItem::DoObserve(int, int part, void*, Object *op)
{
    if (op == bmp && part != cPartSenderDied)
	ForceRedraw();
}

ostream& ImageItem::PrintOn (ostream &s)
{
    VObject::PrintOn(s);
    return s << bmp SP << base SP;
}

istream& ImageItem::ReadFrom(istream &s)
{
    if (deleteBitmap)
	SafeDelete(bmp);
    deleteBitmap= TRUE;
    VObject::ReadFrom(s);
    return s >> bmp >> base;
}

