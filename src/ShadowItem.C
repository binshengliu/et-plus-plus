//$ShadowItem$

#include "ShadowItem.h"

//---- ShadowItem ---------------------------------------------------------------

MetaImpl(ShadowItem, (TP(inner), T(lineWidth), T(shadowWidth), 0));

ShadowItem::ShadowItem(int id, VObject *in, int sw, int lw) 
						  : CompositeVObject(id, in, 0)
{
    shadowWidth= sw;
    lineWidth= lw;
    inner= in;
}

Metric ShadowItem::GetMinSize()
{
    Metric m= inner->GetMinSize();
    m.extent+= 2*lineWidth + shadowWidth;
    m.base+= lineWidth;
    return m;
}

void ShadowItem::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    inner->SetOrigin(at+lineWidth);
}

void ShadowItem::SetExtent(Point e)
{
    VObject::SetExtent(e);
    inner->SetExtent(e-2*lineWidth - shadowWidth);
}

int ShadowItem::Base()
{
    return inner->Base()+lineWidth;
}

void ShadowItem::DrawHighlight(Rectangle)
{
    GrFillRect(inner->contentRect);
}

void ShadowItem::DrawInner(Rectangle r, bool highlight)
{
    Rectangle rr= contentRect;
    if (shadowWidth > 0) {
	rr.extent-= shadowWidth;
	GrPaintRect(rr+shadowWidth, ePatGrey50);
	GrEraseRect(rr);
    }
    GrStrokeRect(rr);
    CompositeVObject::DrawInner(r, highlight);
}

ostream& ShadowItem::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << shadowWidth SP << lineWidth SP;
}

istream& ShadowItem::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    inner= At(0);
    return s >> shadowWidth >> lineWidth;
}

