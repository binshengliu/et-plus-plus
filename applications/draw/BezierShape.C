//$BezierShape,BezierSketcher,BezierStretcher$
#include "Port.h"
#include "Storage.h"
#include "BezierShape.h"
#include "DrawView.h"

//---- BezierSketcher ----------------------------------------------------------

class BezierSketcher: public ShapeSketcher {
    bool firstpress;
    HighlightState feedback;
    Point pts[4];
public:
    BezierSketcher(DrawView*, Shape*, SketchModes);
    void TrackFeedback(Point, Point pp, bool turniton);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    BezierShape *Bezier()
	{ return ((BezierShape*) newshape); }
};

BezierSketcher::BezierSketcher(DrawView *dv, Shape *sp, SketchModes m)
						: ShapeSketcher(dv, sp, m) 
{
    firstpress= FALSE;
    SetFlag(eCmdMoveEvents);
    newshape= (Shape*) proto->Clone();
    view->Insert(newshape);
}

void BezierSketcher::TrackFeedback(Point, Point, bool)
{
    if (feedback) {
	GrLine(pts[2], pts[3]);
	if (Bezier()->GetPtCnt() > 2) {
	    Point pp[4];
	    pp[0]= (pts[0]+pts[1])/2;
	    pp[1]= pts[1];
	    pp[2]= pts[2];
	    pp[3]= (pts[2]+pts[3])/2;
	    GrStrokePolygon(gPoint0, pp, 4, ePolyBezier);
	}
    }
}
 
Command *BezierSketcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    if (tp != eTrackRelease)
	DrawCommand::TrackMouse(tp, ap, pp, np);
    switch (tp) {
    case eTrackPress:
	if (! firstpress)
	    pts[0]= pts[1]= ap;
	pts[2]= pts[3]= ap;
	feedback= On;
	firstpress= TRUE;
	break;
    case eTrackMove:
	pts[2]= ap-delta;
	pts[3]= ap+delta;
	break;
    case eTrackRelease:
	feedback= Off;
	int n= Bezier()->GetPtCnt();
	if (n > 0 && Abs(Bezier()->GetPt(0) - ap) < HandleSize) {
	    Bezier()->AddPt(Bezier()->GetPt(0));
	    Bezier()->AddPt(Bezier()->GetPt(1));
	    Bezier()->AddPt(Bezier()->GetPt(2));
	    view->Remove(newshape);
	    ResetFlag(eCmdMoveEvents);
	} else if (n > 0 && Abs(Bezier()->GetPt(n-1) - ap) < HandleSize) {
	    view->Remove(newshape);
	    ResetFlag(eCmdMoveEvents);
	} else {
	    Bezier()->AddPt(ap-delta);
	    Bezier()->AddPt(ap);
	    Bezier()->AddPt(ap+delta);
	    view->UpdateEvent();
	}
	pts[0]= pts[2];
	pts[1]= pts[3];
	break;
    }
    return this;
}

//---- Stretcher Methods -------------------------------------------------------

class BezierStretcher: public ShapeStretcher {
    BezierShape *lsp;
public:
    BezierStretcher(DrawView*, Shape*, int);
    void DoIt();
    void UndoIt();
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool);
    void TrackConstrain(Point, Point, Point*)
	{ }
};

BezierStretcher::BezierStretcher(DrawView *dv, Shape *sp, int h)
						    : ShapeStretcher(dv, sp, h) 
{
    lsp= (BezierShape*) sp;
}
    
void BezierStretcher::TrackFeedback(Point, Point, bool)
{
    int hc= handle + 1 - (handle % 3);
    lsp->MoveAround(handle, delta, FALSE);
    GrLine(lsp->GetPt(hc-1), lsp->GetPt(hc+1));
    lsp->StrokePartial(max(1, hc-3), min(hc+3, lsp->GetPtCnt()-2));
    lsp->MoveAround(handle, -delta, FALSE);
}

Command *BezierStretcher::TrackMouse(TrackPhase tp, Point ap, Point, Point np)
{
    delta= ap-np;
    // DrawCommand::TrackMouse(tp, ap, pp, np);
    if (tp == eTrackRelease && delta == gPoint0)
	return gNoChanges;
    return this;
}

void BezierStretcher::DoIt()
{
    lsp->MoveAround(handle, delta);
}

void BezierStretcher::UndoIt()
{
    lsp->MoveAround(handle, -delta);
}

//---- Box Shape Methods -------------------------------------------------------

MetaImpl0(BezierShape);

static short BezierImage[]= {
#   include  "images/BezierShape.im"
};

BezierShape::BezierShape()
{
    type= ePolyBezier;
}

short *BezierShape::GetImage()
{
    return BezierImage;
}

ShapeSketcher *BezierShape::NewSketcher(DrawView *dv, SketchModes m)
{
    return new BezierSketcher(dv, this, m);
}

void BezierShape::Outline(Point p1, Point p2)
{
    GrStrokePolygon(NormRect(p1, p2).origin, &pts[1], npts-2, type);
}

void BezierShape::Draw(Rectangle)
{
    GrFillPolygon(bbox.origin, &pts[1], npts-2, type);
    GrStrokePolygon(bbox.origin, &pts[1], npts-2, type);
}

void BezierShape::MoveAround(int n, Point d, bool redraw)
{
    if (redraw)
	Invalidate();
    switch (n % 3) {
    case 0:
	pts[n]-= d;
	pts[n+2]+= d;
	break;
    case 1:
	pts[n-1]-= d;
	pts[n]-= d;
	pts[n+1]-= d;
	break;
    case 2:
	pts[n-2]+= d;
	pts[n]-= d;
	break;
    }
    if (redraw) {
	CalcBBox();
	Invalidate();
	Changed();
    }
}

void BezierShape::Highlight(HighlightState hst)
{
    PolyShape::Highlight(hst);
    if (GetSplit())
	for (int i= 1; i < npts-1; i+= 3) {
	    GrPaintLine(ePatBlack, 1, eDefaultCap, GetPt(i-1), GetPt(i));
	    GrPaintLine(ePatBlack, 1, eDefaultCap, GetPt(i), GetPt(i+1));
	}
}

ShapeStretcher *BezierShape::NewStretcher(DrawView *dv, int handle)
{
    //if (GetSplit())
	return new BezierStretcher(dv, this, handle);
    //return this->LineShape::NewStretcher(dv, handle);
}
