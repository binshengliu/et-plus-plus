//$PictPort$
#include "PictPort.h"
#include "Picture.h"

MetaImpl0(PictPort);

PictPort::PictPort(Picture *p) : PrintPort("PICT")
{
    pict= p;
}

PictPort::~PictPort()
{
    pict->Close();
}

void PictPort::DevStrokeLine(int psz, Rectangle*, GrLineCap cap, Point p1, Point p2)
{
    pict->StrokeLine(psz, cap, p1, p2);
}

bool PictPort::DevShowChar(FontPtr fp, Point, byte c, bool, Point pos)
{
    pict->PutGlyph(c, pos, fp);
    return FALSE;
}

void PictPort::DrawObject(char type, Rectangle *r, Point e, int psz, GrLineCap cap)
{
    pict->DrawObject(type, r, e, psz, cap);
}

void PictPort::DevShowTextBatch(Rectangle *r, Point)
{
    pict->Merge(r);
}

void PictPort::DevShowBitmap(Rectangle *r, struct Bitmap *bmp)
{
    pict->PutBitmap(r, bmp);
}

void PictPort::DevSetPattern(struct DevBitmap*)
{
}

bool PictPort::DevSetColor(RGBColor*)
{
    return TRUE;
}

void PictPort::DevSetOther(int pid)
{
    pict->PutPat(pid);
}

