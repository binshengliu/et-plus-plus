//$DynShape2$
#include "DynShape2.h"

//---- DynShape2 ---------------------------------------------------------------

MetaImpl0(DynShape2);

DynShape2::DynShape2()
{
}
    
void DynShape2::Draw(Rectangle r)
{
    DynShape::Draw(r);
    GrLine(bbox.N(), bbox.S());
    GrLine(bbox.W(), bbox.E());
}

void DynShape2::Outline(Point p1, Point p2)
{
    DynShape::Outline(p1, p2);
    Rectangle rr= NormRect(p1, p2);
    GrLine(rr.N(), rr.S());
    GrLine(rr.W(), rr.E());
}
