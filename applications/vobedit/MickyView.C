//$MickyView$

#include "MickyView.h"

//---- MickyView -------------------------------------------------------------------

MetaImpl0(MickyView);

MickyView::MickyView(Rectangle itsExtent) : View(0, itsExtent)
{
}

MickyView::MickyView() : View(0, gPoint0)
{
}

Command *MickyView::DoLeftButtonDownCommand(Point, Token, int)
{
    return new Command; // for autoscrolling
}

Metric MickyView::GetMinSize()
{
    return Point(200);
}

void MickyView::Draw(Rectangle)
{
    GrFillOval(Rectangle(72, 74, 55, 65));
    GrEraseOval(Rectangle(74, 84, 51, 54));
    GrStrokeOval(Rectangle(84, 109, 31, 20));
    GrEraseOval(Rectangle(84, 108, 31, 14));
    GrStrokeOval(Rectangle(87, 98, 9, 9));
    GrFillOval(Rectangle(90, 101, 3, 3));
    GrStrokeOval(Rectangle(104, 98, 9, 9));
    GrFillOval(Rectangle(107, 101, 3, 3));
    GrFillOval(Rectangle(97, 111, 6, 6));
    GrFillOval(Rectangle(52, 53, 38, 38));
    GrFillOval(Rectangle(110, 53, 38, 38));
}

ostream& MickyView::PrintOn(ostream &s)
{
    View::PrintOn(s);
    return s << GetExtent() SP;
}

istream& MickyView::ReadFrom(istream &s)
{
    Point p;
    View::ReadFrom(s);
    s >> p;
    SetExtent(p);
    return s;
}
