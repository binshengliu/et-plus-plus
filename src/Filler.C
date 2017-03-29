//$Filler$

#include "Filler.h"

//---- Filler ------------------------------------------------------------------

MetaImpl(Filler, (T(minsize), TP(ink), 0));

Filler::Filler(Point extent, int flags)
{   
    ink= gInkNone;
    contentRect.extent= minsize= extent;
    SetFlag(flags & (eVObjHFixed | eVObjVFixed));
}

Filler::Filler(Point extent, Ink *ik, int flags)
{   
    ink= ik;
    contentRect.extent= minsize= extent;
    SetFlag(flags & (eVObjHFixed | eVObjVFixed));
}

Metric Filler::GetMinSize()
{
    return Metric(minsize);
}

void Filler::Draw(Rectangle r)
{
    if (ink != ePatNone)
	GrPaintRect(r, ink);
}

Ink *Filler::SetInk(Ink *ik, bool redraw)
{
    Ink *oldink= ink;
    ink= ik;
    if (redraw)
	ForceRedraw();
    return oldink;
}

ostream& Filler::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << minsize SP << ink SP;
}

istream& Filler::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    return s >> minsize >> ink;
}

