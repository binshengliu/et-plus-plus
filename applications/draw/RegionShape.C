//$RegionShape,RegionSketcher$
#include "Port.h"
#include "Storage.h"
#include "RegionShape.h"
#include "DrawView.h"

//---- RegionShape -------------------------------------------------------------

static short RegionImage[]= {
#   include  "images/RegionShape.im"
};

MetaImpl0(RegionShape);

RegionShape::RegionShape()
{
}

ShapeSketcher *RegionShape::NewSketcher(class DrawView *dv, SketchModes m)
{
    return new RegionSketcher(dv, this, m);
}

short *RegionShape::GetImage()
{
    return RegionImage;
}

GrCursor RegionShape::SketchCursor()
{
    return eCrsPen;
}

//---- RegionSketcher ----------------------------------------------------------

RegionSketcher::RegionSketcher(class DrawView *dv, Shape *sp, SketchModes)
					    : ShapeSketcher(dv, sp, eSMDefault)
{
    newshape= (Shape*) proto->Clone();
    newshape->SetContainer(view);
    view->GetShapes()->Insert(newshape);
}

void RegionSketcher::TrackFeedback(Point, Point np, bool on)
{
    if (on && lastp != np) {
	GrSetPenNormal();
	GrLine(lastp, np);
	lastp= np;
    }
}

Command *RegionSketcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    if (lastp != np)
	((PolyShape*)newshape)->AddPt(np);
    if (tp == eTrackPress)
	lastp= ap;
    else if (tp == eTrackRelease)
	view->Remove(newshape);
    return this;
}
