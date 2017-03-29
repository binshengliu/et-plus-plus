//$PolyShape,PolySketcher,SplineStretcher$
#include "PolyShape.h"
#include "DrawView.h"

static short PolyImage[]= {
#   include  "images/PolyShape.im"
};

//---- PolyShape ---------------------------------------------------------------

MetaImpl(PolyShape, (T(npts), TV(pts, npts), 0));

PolyShape::PolyShape()
{
    SetFlag(eShapeSplit);
    type= ePolyDefault;
    size= npts= 0;
    pts= 0;
    ink= 0;
}
	
PolyShape::~PolyShape()
{
    SafeDelete(pts);
}

short *PolyShape::GetImage()
{
    return PolyImage;
}

int PolyShape::AddPt(Point p)
{
    if (npts >= size) {
	size= max(10, npts*2);
	pts= (Point*) Realloc(pts, size * sizeof(Point));
    }
    p-= bbox.origin;
    pts[npts++]= p;
    CalcBBox();
    Invalidate();
    return npts;
}

Point PolyShape::GetPt(int i)
{
    if (pts[0] == pts[npts-1]) {
	if (i < 0)
	    i= npts-1+i;
	i %= npts-1;
    } else
	i= range(0, npts-1, i);
    return pts[i] + bbox.origin;
}

void PolyShape::SetSpan(Rectangle r)
{
    Shape::SetSpan(r);
}

void PolyShape::CalcBBox()
{
    Point oldorigin= bbox.origin;
    bbox= BoundingBox(npts, pts);
    bbox.origin+= oldorigin;
}

void PolyShape::MovePoint(int handle, Point delta, bool redraw)
{
    if (redraw)
	Invalidate();
    if (pts[0] == pts[npts-1] && handle == 0)
	pts[npts-1]+= delta;
    pts[handle]+= delta;
    if (redraw) {
	CalcBBox();
	Invalidate();
	Changed();
    }
}

bool PolyShape::CanSplit()
{
    return ! GetSplit();
}

Point *PolyShape::MakeHandles(int *n)
{
    if (GetSplit()) {
	if (npts > 1 && pts[0] == pts[npts-1])
	    *n= npts-1;
	else
	    *n= npts;
	return pts;
    }
    return Shape::MakeHandles(n);
}

void PolyShape::Outline(Point p1, Point p2)
{
    GrStrokePolygon(NormRect(p1, p2).origin, pts, npts, type);
}

void PolyShape::StrokePartial(int s, int e)
{
    Point p[10];
    for (int i= 0; i < e-s+1; i++)
	p[i]= GetPt(s+i);
    GrStrokePolygon(gPoint0, p, e-s+1, type);
}

void PolyShape::Draw(Rectangle)
{
    GrFillPolygon(bbox.origin, pts, npts, type);
    GrStrokePolygon(bbox.origin, pts, npts, type);
}

ShapeSketcher *PolyShape::NewSketcher(DrawView *dv, SketchModes m)
{
    return new PolySketcher(dv, this, m);
}

ShapeStretcher *PolyShape::NewStretcher(DrawView *dv, int handle)
{
    if (GetSplit())
	return new SplineStretcher(dv, this, handle);
    return ((LineShape*)this)->LineShape::NewStretcher(dv, handle);
}

Inside_2D_Polygon(Point p, int numpoints, Point *data)
{
    int intersect= 0, i, j;
    Point p1, p2;
    int x3, y4;

    for (i= 0; i < numpoints; i++) {
	p1= data[i];
	p2= data[(i+1) % numpoints];
	if (p.y != p2.y) {
	    if (p2.y > p1.y)
		Swap(p1, p2);
	    if (p.y > p2.y && p.y < p1.y) {
		x3= p1.x;
		if (p1.y != p2.y)
		    x3-= (p1.x-p2.x) * (p1.y-p.y) / (p1.y-p2.y);
		if (x3 < p.x)
		    intersect++;
	    }
	} else {
	    if (p2.x < p.x) {
		if (p1.y == p2.y) {
		    if (((p1.x > p.x) && (p2.x < p.x))
					|| ((p1.x < p.x) && (p2.x > p.x)))
			intersect++;
		} else {
		    for (j= i; j < numpoints; j++) {
			y4= data[(j+1) % numpoints].y;
			if (y4 != p.y) {
			    if (((p1.y < p.y) && (y4 > p.y))
						|| ((p1.y > p.y) && (p.y > y4)))
				intersect++;
			    break;
			}
		    }
		}
	    }
	}
    }
    return intersect % 2;
}

bool PolyShape::ContainsPoint(Point p)
{
    p-= bbox.origin;
    if (penink != 0) {
	for (int i= 1; i < npts; i++)
	    if (((LineShape*)this)->LineShape::PointAtLine(p, pts[i-1], pts[i]))
		return TRUE;
    }
    if (ink != 0)
	return Inside_2D_Polygon(p, npts, pts);
    return FALSE;
}

ostream& PolyShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    s << npts SP << type SP;
    for (int i= 0; i < npts; i++)
	s << pts[i] SP;
    return s;
}

istream& PolyShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    s >> npts >> Enum(type);
    size= npts;
    SafeDelete(pts);
    pts= new Point[npts];
    for (int i= 0; i<npts; i++)
	s >> pts[i];
    return s;
}

//---- Stretcher Methods -------------------------------------------------------

SplineStretcher::SplineStretcher(DrawView *dv, Shape *sp, int h)
						    : ShapeStretcher(dv, sp, h) 
{
    lsp= (PolyShape*) sp;
}
    
void SplineStretcher::TrackFeedback(Point, Point, bool)
{
    lsp->MovePoint(handle, delta, FALSE);
    lsp->StrokePartial(handle-1, handle+1);
    lsp->MovePoint(handle, -delta, FALSE);
}

void SplineStretcher::TrackConstrain(Point, Point, Point*)
{
}

Command *SplineStretcher::TrackMouse(TrackPhase tp, Point ap, Point, Point np)
{
    delta= np-ap;
    // DrawCommand::TrackMouse(tp, ap, pp, np);
    if (tp == eTrackRelease && delta == gPoint0)
	return gNoChanges;
    return this;
}

void SplineStretcher::DoIt()
{
    lsp->MovePoint(handle, delta);
}

void SplineStretcher::UndoIt()
{
    lsp->MovePoint(handle, -delta);
}

//---- Sketcher Methods --------------------------------------------------------

PolySketcher::PolySketcher(DrawView *dv, Shape *sp, SketchModes m)
						    : ShapeSketcher(dv, sp, m) 
{
    SetFlag(eCmdMoveEvents);
    newshape= (Shape*) proto->Clone();
    view->Insert(newshape);
}

void PolySketcher::TrackFeedback(Point ap, Point np, bool)
{
    GrLine(ap, np);
}
 
Command *PolySketcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    if (tp != eTrackRelease)
	DrawCommand::TrackMouse(tp, ap, pp, np);
    view->ShowInfo(tp, "x: %4d y: %4d", np.x, np.y);
    if (tp == eTrackRelease) {
	int n= Poly()->GetPtCnt();
	if (n > 2 && Abs(ap - Poly()->GetPt(n-1)) < gPoint3) {
	    view->Remove(newshape);
	    ResetFlag(eCmdMoveEvents);
	} else if (n > 2 && Abs(ap - Poly()->GetPt(0)) < gPoint3) {
	    Poly()->AddPt(Poly()->GetPt(0));
	    view->UpdateEvent();
	    view->Remove(newshape);
	    ResetFlag(eCmdMoveEvents);
	} else {
	    Poly()->AddPt(ap);
	    view->UpdateEvent();
	}
    }
    return this;
}
