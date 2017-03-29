//$RcBoxShape,RcDiaStretcher$
#include "RcBoxShape.h"
#include "OvalShape.h"

//---- RcDia Stretcher ---------------------------------------------------------

class RcDiaStretcher: public DrawCommand {
    RcBoxShape *rc;
    Point newdia, olddia;
public:
    RcDiaStretcher(DrawView *dv, RcBoxShape *rc1) : DrawCommand(dv, 1, "set radius")
	{ rc= rc1; olddia= rc->GetDia(); }
    Command *TrackMouse(TrackPhase atp, Point ap, Point, Point np);
    void TrackFeedback(Point, Point, bool);
    void DoIt()
	{ rc->SetDia(newdia); }
    void UndoIt()
	{ rc->SetDia(olddia); }
    void RedoIt()
	{ rc->SetDia(newdia); }
};
    
Command *RcDiaStretcher::TrackMouse(TrackPhase atp, Point ap, Point pp, Point np)
{
    Command *cmd= DrawCommand::TrackMouse(atp, ap, pp, np);
    newdia= olddia+2*delta;
    newdia= Min(rc->bbox.extent, Max(gPoint0, newdia));
    rc->SetDia(newdia, FALSE);
    return cmd;
}

void RcDiaStretcher::TrackFeedback(Point, Point, bool)
{
    Point p1= rc->bbox.NW(), p2= p1+newdia/2;
    rc->Outline(p1, rc->bbox.SE());
    GrLine(Point(p1.x, p2.y), p2);
    GrLine(Point(p2.x, p1.y), p2);
}

//---- Round Corner Box Shape Methods ------------------------------------------

static short RcBoxImage[]= {
#   include  "images/RcBoxShape.im"
};

MetaImpl(RcBoxShape, (T(rcdia), 0));

RcBoxShape::RcBoxShape()
{
    rcdia= max(2*pensize, 30);
}

short *RcBoxShape::GetImage()
{
    return RcBoxImage;
}

void RcBoxShape::SetSpan(Rectangle r)
{
    Shape::SetSpan(r);
    rcdia= Min(rcdia, bbox.extent);
}

void RcBoxShape::SetDia(Point d, bool redraw)
{
    rcdia= Min(d, bbox.extent);
    if (redraw) {
	Invalidate();
	Changed();
    }
}

void RcBoxShape::Draw(Rectangle)
{
    GrFillRoundRect(bbox, rcdia);
    GrStrokeRoundRect(bbox, rcdia);
}

bool RcBoxShape::ContainsPoint(Point p)
{
    Point b= (bbox.extent-rcdia)/2;
    p= Abs(p-bbox.Center()) - b;
    if (! (p > gPoint0))
	return TRUE;
    if (Len(rcdia/2, p) <= 1.0) {
	if (penink == 0)
	    return (bool) (Len(rcdia/2-(pensize+2), p) >= 1.0);
	return TRUE;
    }
    return FALSE;
}

Point *RcBoxShape::MakeHandles(int *n)
{
    BoxShape::MakeHandles(n);
    spts[8]= spts[0]+rcdia/2;
    (*n)++;
    return spts;
}

ShapeStretcher *RcBoxShape::NewStretcher(DrawView *dv, int handle)
{
    if (handle == 8)
	return (ShapeStretcher*) new RcDiaStretcher(dv, this);
    return BoxShape::NewStretcher(dv, handle);
}

Point RcBoxShape::Chop(Point p)
{
    Point p1= p;
    p= Shape::Chop(p);
    Point pc= p - bbox.Center();
    Point b= (bbox.extent-rcdia)/2;
    if (! (Abs(pc) > b))
	return p;
	
    Rectangle inner(bbox.Inset(rcdia.Half()));
    int a= bbox.PointToAngle(p1-bbox.Center());
    Rectangle ee(rcdia);
    Point p3= ee.AngleToPoint(a) - ee.Center();
    return bbox.Center() + inner.extent.Half() + p3;
}

void RcBoxShape::Outline(Point p1, Point p2)
{
    Shape::Outline(p1, p2);
    GrStrokeRoundRect(NormRect(p1, p2), rcdia);
}

ostream& RcBoxShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << rcdia SP;
}
	
istream& RcBoxShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    return s >> rcdia;
}
