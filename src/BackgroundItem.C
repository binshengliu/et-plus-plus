//$BackgroundItem$

#include "BackgroundItem.h"

//---- BackgroundItem ----------------------------------------------------------

MetaImpl(BackgroundItem, (TP(ink), 0));

BackgroundItem::BackgroundItem(Ink *i, VObject *ip) : CompositeVObject(cIdNone, ip, 0)
{
    ink= i;
    SetFlag(eVObjHFixed | eVObjVFixed);
}

void BackgroundItem::DrawInner(Rectangle r, bool highlight)
{
    GrPaintRect(r, ink);
    CompositeVObject::DrawInner(r, highlight);
}

ostream& BackgroundItem::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << ink SP;
}

istream& BackgroundItem::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    return s >> ink;
}
