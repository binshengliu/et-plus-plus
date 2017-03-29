//$Slider2,ThumbMover2,Thumb2$
#include "Slider2.h"

//---- ThumbMover2 ------------------------------------------------------------

class ThumbMover2: public Command {
    Rectangle thumbRect;
    Slider2 *slider;
public:
    ThumbMover2(Rectangle, Slider2*);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool)
	{ /* indirect feedback */ }
};

ThumbMover2::ThumbMover2(Rectangle tr, Slider2 *sl) : Command(555)
{
    slider= sl;
    thumbRect= tr;
}

Command *ThumbMover2::TrackMouse(TrackPhase atp, Point ap, Point, Point np)
{
    if (atp == eTrackRelease)
	return gNoChanges;
    if (atp == eTrackMove)
	slider->MoveThumb(thumbRect.origin + (np - ap));
    return this;
}

//---- Thumb2 -------------------------------------------------------------------

MetaImpl0(Thumb2);

Thumb2::Thumb2(Slider2 *sl, GrCursor c) : VObject(Rectangle(20), eSliderThumb)
{
    slider= sl;
    cursor= c;
}

void Thumb2::Draw(Rectangle r)
{
    GrPaintOval(r, ePatGrey50);
    GrStrokeOval(contentRect);
}

Metric Thumb2::GetMinSize()
{
    return Metric(25,15);
}

Command *Thumb2::DoLeftButtonDownCommand(Point, Token, int)
{
    return new ThumbMover2(contentRect, slider);
}

GrCursor Thumb2::GetCursor(Point)
{
    return cursor;
}

//---- Slider2 ------------------------------------------------------------------

MetaImpl(Slider2, (T(minVal), T(maxVal), T(currVal), 0));

Slider2::Slider2(int id, Point max, Point min, Direction dir)
				    : CompositeVObject(id, (Collection*)0)
{
    Add(new Thumb2(this, (dir==eVert) ? eCrsUpDownArrow : eCrsLeftRightArrow));
    minVal= min;
    maxVal= max;
    currVal= (min+max)/2;
}

void Slider2::SetVal(Point v, bool redraw)
{
    v= Min(Max(v, minVal), maxVal);
    if (currVal != v) {
	currVal= v;
	Update(redraw);
    }
}

void Slider2::SetMax(Point v, bool redraw)
{
    maxVal= v;
    Update(redraw);
}

void Slider2::SetMin(Point v, bool redraw)
{
    minVal= v;
    Update(redraw);
}

void Slider2::SetOrigin(Point at)
{
    Point d= GetThumb()->GetOrigin() - GetOrigin();
    VObject::SetOrigin(at);
    GetThumb()->SetOrigin(at+d);
}

Metric Slider2::GetMinSize()
{
    return Max(GetExtent(), GetThumb()->GetMinSize().Extent());
}

void Slider2::Update(bool redraw)
{
    Thumb2 *thumb= GetThumb();
    Rectangle b= thumb->contentRect;
    Point framesize= GetExtent() - b.extent;
    
    b.origin= GetOrigin() + Scale(currVal-minVal, framesize, maxVal.x-minVal);

    if (thumb->contentRect != b) {
	thumb->SetContentRect(b, redraw);
	if (redraw)
	    UpdateEvent();
    }
}

void Slider2::MoveThumb(Point pos)
{
    Point val= Scale(pos-GetOrigin(), maxVal-minVal, GetExtent() - GetThumb()->GetExtent()) + minVal;
    val= Range(minVal, maxVal, val);
    Control(GetId(), eSliderThumb, &val);
}

void Slider2::Draw(Rectangle r)
{
    GrSetPenSize(2);
    GrLine(contentRect.N(), contentRect.S());
    GrSetPenSize(1);
    CompositeVObject::Draw(r);
}

void Slider2::Control(int id, int part, void *val)
{   
    if (id == GetId() && part == eSliderThumb)
	SetVal(*((Point*)val), TRUE);
    CompositeVObject::Control(id, part, val);
}

