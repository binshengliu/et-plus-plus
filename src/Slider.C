//$Slider,PagerCommand,Thumb,ThumbMover$
#include "Slider.h"

//---- ThumbMover ------------------------------------------------------------

class ThumbMover: public Command {
    Point thumborigin;
    Slider *slider;
public:
    ThumbMover(Point, Slider*);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool)
	{ /* indirect feedback */ }
};

ThumbMover::ThumbMover(Point to, Slider *sl) : Command(cIdNone)
{
    slider= sl;
    thumborigin= to;
}

Command *ThumbMover::TrackMouse(TrackPhase atp, Point ap, Point, Point np)
{
    if (atp == eTrackRelease)
	return gNoChanges;
    if (atp == eTrackMove)
	slider->MoveThumb(thumborigin + (np - ap));
    return this;
}

//---- Thumb -------------------------------------------------------------------

MetaImpl0(Thumb);

Thumb::Thumb(Slider *sl, GrCursor c) : VObject(Rectangle(20), eSliderThumb)
{
    slider= sl;
    cursor= c;
}

void Thumb::Draw(Rectangle r)
{
    GrPaintRect(r, ePatGrey50);
    GrStrokeRect(contentRect);
}

Metric Thumb::GetMinSize()
{
    return Metric(20);
}

Command *Thumb::DoLeftButtonDownCommand(Point, Token, int)
{
    return new ThumbMover(GetOrigin(), slider);
}

GrCursor Thumb::GetCursor(Point)
{
    return cursor;
}

//---- Slider ------------------------------------------------------------------

MetaImpl(Slider, (T(minVal), T(maxVal), T(currVal), T(bubbleSize), 0));

Slider::Slider(int id, Direction dir)
    : CompositeVObject(id, new Thumb(this, (dir == eVert) ? eCrsUpDownArrow
						 : eCrsLeftRightArrow), 0)
{
    minVal= gPoint0;
    maxVal= 100;
    bubbleSize= GetThumb()->GetMinSize().extent;
}

void Slider::SetVal(Point v, bool redraw)
{
    currVal= v;
    Update(redraw);
}

void Slider::SetMax(Point v, bool redraw)
{
    maxVal= v;
    Update(redraw);
}

void Slider::SetMin(Point v, bool redraw)
{
    minVal= v;
    Update(redraw);
}

void Slider::SetThumbRange(Point s, bool redraw)
{
    bubbleSize= s;
    Update(redraw);
}

void Slider::SetOrigin(Point at)
{
    Point d= GetThumb()->GetOrigin() - GetOrigin();
    VObject::SetOrigin(at);
    GetThumb()->SetOrigin(at+d);
}

Metric Slider::GetMinSize()
{
//    return Max(GetExtent(), GetThumb()->GetMinSize().Extent());
    return GetThumb()->GetMinSize();
}

void Slider::Update(bool redraw)
{
    Rectangle b;
    
    b.extent= Range(GetThumb()->GetMinSize().extent, GetExtent(), Scale(bubbleSize, GetExtent(), maxVal));
    b.origin= GetOrigin() + Scale(currVal, GetExtent()-b.extent, maxVal-bubbleSize);
    if (GetThumb()->contentRect != b)
	GetThumb()->SetContentRect(b, redraw);
}

void Slider::MoveThumb(Point pos)
{
    Point val= Scale(pos-GetOrigin(), maxVal-bubbleSize,
					GetExtent()-GetThumb()->GetExtent());
    Control(GetId(), eSliderThumb, &val);
}

void Slider::Draw(Rectangle r)
{
    GrPaintRect(r, ePatGrey12);
    CompositeVObject::Draw(r);
    GrSetPenNormal();
    GrStrokeRect(contentRect);
}

int Slider::Where(Point lp)
{
    Rectangle r= GetThumb()->contentRect;
    
    if (lp.x >= r.NW().x && lp.x <= r.NE().x) {
	if (lp.y < r.origin.y)
	    return 1;
	return 2;
    }
    if (lp.y >= r.NW().y && lp.y <= r.SW().y) {
	if (lp.x < r.origin.x)
	    return 3;
	return 4;
    }
    return 0;
}

Command *Slider::DoLeftButtonDownCommand(Point lp, Token, int)
{
    switch (Where(lp)) {
    case 1:
    case 3:
	return new PagerCommand(this, eSliderPageUpLeft);
    case 2:
    case 4:
	return new PagerCommand(this, eSliderPageDownRight);
    }
    return gNoChanges;
}

GrCursor Slider::GetCursor(Point lp)
{
    switch (Where(lp)) {
    case 1:
	return eCrsUpArrow;
    case 2:
	return eCrsDownArrow;
    case 3:
	return eCrsLeftArrow;
    case 4:
	return eCrsRightArrow;
    }
    return VObject::GetCursor(lp);
}

//---- PagerCommand ------------------------------------------------------------

PagerCommand::PagerCommand(Slider* s, int p)   
		    : Command(0, (CommandFlags)(eCmdDefault | eCmdIdleEvents))
{
    slider= s;
    part= p;
}

void PagerCommand::TrackFeedback(Point, Point, bool)
{
}

Command *PagerCommand::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    switch (atp) {
    case eTrackPress:
    case eTrackIdle:
	if (slider->ContainsPoint(np))
	    slider->Control(slider->GetId(), part, (void*)0);
	break;
    case eTrackRelease:
	return gNoChanges;
    }
    return this;
}
