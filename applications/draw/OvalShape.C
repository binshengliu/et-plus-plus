//$OvalShape,WedgeStretcher$
#include "OvalShape.h"
#include "DrawView.h"

float Len(Point rad, Point p)
{
    return float(p.x*p.x)/(rad.x*rad.x) + float(p.y*p.y)/(rad.y*rad.y);
}

//---- Shape Methods -----------------------------------------------------------

static short OvalImage[]= {
#   include  "images/OvalShape.im"
};

MetaImpl(OvalShape, (T(startangle), T(anglen), 0));

OvalShape::OvalShape()
{
    startangle= 0;
    anglen= 360;
}

short *OvalShape::GetImage()
{
    return OvalImage;
}

void OvalShape::SetAngle(bool where, int ang)
{
    if (where)
	anglen= ang-startangle;
    else
	startangle= ang;
    Invalidate();
    Changed();
}

int OvalShape::GetAngle(bool where)
{
    return where ? (startangle+anglen) : startangle;
}

bool OvalShape::CanSplit()
{
    return (bool) !GetSplit();
}

Point OvalShape::Chop(Point p)
{
    return bbox.OvalAngleToPoint(bbox.PointToAngle(p));
}

Rectangle OvalShape::InvalRect()
{
    if (arrows)
	return bbox.Expand(Max(HandleSize, Point(4*pensize))/2+1);
    return Shape::InvalRect();
}

void OvalShape::Draw(Rectangle)
{
    Rectangle rr= RealRect(bbox, startangle);
    GrFillWedge(rr, startangle, anglen);
    GrStrokeWedge(rr, startangle, anglen);
}

int HFlip(int a)
{
    a= 360-a;
    if (a > 360)
	a-= 360;
    else if (a < 0)
	a+= 360;
    return a;
}

int VFlip(int a)
{
    a= 180+360-a;
    if (a > 360)
	a-= 360;
    else if (a < 0)
	a+= 360;
    return a;
}

int FlipArrows(int a)
{
    if (a == 0 || a == 3)
	return a;
    return a^0x3;
}

void OvalShape::Flip(int n)
{
    int s, e;
    if (anglen == 360)
	return;
    switch (n) {
    case 1:
	s= VFlip((startangle+anglen));
	e= VFlip(startangle);
	arrows= (GrLineCap) FlipArrows(arrows);
	break;
    case 2:
	s= HFlip((startangle+anglen));
	e= HFlip(startangle);
	arrows= (GrLineCap) FlipArrows(arrows);
	break;
    case 3:
	s= HFlip(VFlip(startangle));
	e= HFlip(VFlip((startangle+anglen)));
	break;
    default:
	return;
    }
    startangle= s;
    anglen= e-s;
}

void OvalShape::Outline(Point p1, Point p2)
{    
    if (anglen == 360) {
	GrStrokeOval(NormRect(p1, p2));
	return;
    }
    if (abs(p1.x-p2.x) < 2 || abs(p1.y-p2.y) < 2)
	return;
    int a, s, e;
    if (p2.x > p1.x) {
	if (p2.y < p1.y) {
	    s= VFlip((startangle+anglen));
	    e= VFlip(startangle);
	    a= FlipArrows(arrows);
	} else {
	    a= arrows;
	    s= startangle;
	    e= (startangle+anglen);
	}
    } else {
	if (p2.y > p1.y) {
	    s= HFlip((startangle+anglen));
	    e= HFlip(startangle);
	    a= FlipArrows(arrows);
	} else {
	    s= HFlip(VFlip(startangle));
	    e= HFlip(VFlip((startangle+anglen)));
	}
    }
    Shape::Outline(p1, p2);
    GrStrokeWedge(RealRect(NormRect(p1, p2), s), s, e-s);
}

Rectangle OvalShape::RealRect(Rectangle rr, int)
{
    return rr;
}

bool OvalShape::ContainsPoint(Point p)
{
    Rectangle rr= RealRect(bbox, startangle);
    Point center= rr.extent/2;
    
    if (anglen != 360) {
	int ang= rr.PointToAngle(p);
	if (startangle <= (startangle+anglen)) {
	    if (ang < startangle || ang > (startangle+anglen))
		return FALSE;
	} else {
	    if (ang <= startangle && ang >= (startangle+anglen))
		return FALSE;
	}
    }
    p-= rr.Center();
    if (Len(center+2, p) <= 1.0) {
	if (ink == 0)
	    return (bool) (Len(center-(pensize+2), p) >= 1.0);
	return TRUE;
    }
    return FALSE;
}

Point *OvalShape::MakeHandles(int *n)
{
    if (GetSplit()) {
	spts[0]= bbox.OvalAngleToPoint((startangle+anglen))-bbox.origin;
	spts[1]= bbox.OvalAngleToPoint(startangle)-bbox.origin;
	if (spts[0] == spts[1])
	    *n= 1;
	else
	    *n= 2;
	return spts;
    }
    return Shape::MakeHandles(n);
}

ShapeStretcher *OvalShape::NewStretcher(DrawView *dv, int handle)
{
    if (GetSplit())
	return new WedgeStretcher(dv, this, handle, (bool) (handle == 0));
    return Shape::NewStretcher(dv, handle);
}

ostream& OvalShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << startangle SP << anglen SP;
}
	
istream& OvalShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    return s >> startangle >> anglen;
}

//---- Stretcher Methods -------------------------------------------------------

WedgeStretcher::WedgeStretcher(DrawView *dv, Shape *p, int h, bool w)
						    : ShapeStretcher(dv, p, h)
{
    op= (OvalShape*)p;
    where= w;
    oldangle= op->GetAngle(where);
    angpt= op->bbox.OvalAngleToPoint(oldangle);
}

Command *WedgeStretcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    newangle= op->bbox.PointToAngle(angpt+delta);
    anglept= op->bbox.OvalAngleToPoint(newangle);
    view->ShowInfo(tp, "%s: %d", where ? "end" : "start", newangle);
    if (tp == eTrackRelease && newangle == oldangle)
	    return gNoChanges;
    return this;
}

void WedgeStretcher::TrackFeedback(Point, Point, bool)
{
    GrLine(op->bbox.Center(), anglept);
}

void WedgeStretcher::TrackConstrain(Point, Point, Point*)
{
}

void WedgeStretcher::DoIt()
{
    op->SetAngle(where, newangle);
}

void WedgeStretcher::UndoIt()
{
    op->SetAngle(where, oldangle);
}
