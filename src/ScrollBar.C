//$ScrollBarButton,ScrollBar$
#include "ScrollBar.h"
#include "Slider.h"
#include "Buttons.h"
#include "ObjectTable.h"

const int eScrollBarUpLeft      = cPartLast + 0,
	  eScrollBarDownRight   = cPartLast + 1,
	  eScrollBarSlider      = cPartLast + 2;

static u_short UpArrowBits[]= {
#   include "images/UpArrow.image"
};
static u_short InvUpArrowBits[]= {
#   include "images/UpArrowInv.image"
};
static u_short DownArrowBits[]= {
#   include "images/DownArrow.image"
};
static u_short InvDownArrowBits[]= {
#   include "images/DownArrowInv.image"
};
static u_short LeftArrowBits[]= {
#   include "images/LeftArrow.image"
};
static u_short InvLeftArrowBits[]= {
#   include "images/LeftArrowInv.image"
};
static u_short RightArrowBits[]= {
#   include "images/RightArrow.image"
};
static u_short InvRightArrowBits[]= {
#   include "images/RightArrowInv.image"
};

static Bitmap *ArrowUp, *InvArrowUp, *ArrowDown, *InvArrowDown,
	      *ArrowLeft, *InvArrowLeft, *ArrowRight, *InvArrowRight;

//---- ScrollBarButton ---------------------------------------------------------

class ScrollBarButton: public ImageButton {
public:
    ScrollBarButton(int id, Bitmap *b1, Bitmap *b2) : ImageButton(id, b1, b2, TRUE)
	{ SetFlag(eVObjHFixed | eVObjVFixed); }
    void DrawInner(Rectangle r, bool highlight);
    Metric GetMinSize()
	{ return ImageButton::GetMinSize().extent + gPoint4; }
};

void ScrollBarButton::DrawInner(Rectangle r, bool highlight)
{
    contentRect= contentRect.Inset(2);
    ImageButton::DrawInner(r, highlight);
    contentRect= contentRect.Expand(2);
    GrStrokeRect(contentRect);
}

//---- ScrollBar ---------------------------------------------------------------

MetaImpl(ScrollBar, (TP(slider), 0));

ScrollBar::ScrollBar(int id, Direction v) : Expander(id, v)
{
    if (v && ArrowUp == 0)
	ObjectTable::AddRoots(
	    ArrowUp= new Bitmap(16, UpArrowBits),
	    InvArrowUp= new Bitmap(16, InvUpArrowBits),
	    ArrowDown= new Bitmap(16, DownArrowBits),
	    InvArrowDown= new Bitmap(16, InvDownArrowBits),
	    0
	);
    if (!v && ArrowLeft == 0)
	ObjectTable::AddRoots(
	    ArrowLeft= new Bitmap(16, LeftArrowBits),
	    InvArrowLeft= new Bitmap(16, InvLeftArrowBits),
	    ArrowRight= new Bitmap(16, RightArrowBits),
	    InvArrowRight= new Bitmap(16, InvRightArrowBits),
	    0
	);
    Add(new ScrollBarButton(eScrollBarUpLeft, v ? ArrowUp : ArrowLeft,
					      v ? InvArrowUp : InvArrowLeft));
    Add(slider= new Slider(eScrollBarSlider, v));
    Add(new ScrollBarButton(eScrollBarDownRight, v ? ArrowDown : ArrowRight,
						 v ? InvArrowDown : InvArrowRight));
}

void ScrollBar::Draw(Rectangle r)
{
    // GrSetPenNormal();
    // GrStrokeRect(contentRect);
    Expander::Draw(r);
}

void ScrollBar::ViewSizeChanged(Point vsz)
{
    slider->SetMax(vsz, TRUE);
}

void ScrollBar::BubbleUpdate(Point relOrig)
{
    slider->SetVal(relOrig, TRUE);
}

void ScrollBar::SetThumbRange(Point sz)
{
    slider->SetThumbRange(sz, TRUE);
}

void ScrollBar::Init(Point e, Point sz, Point vsz)
{
    SetExtent(e);
    slider->SetThumbRange(sz);
    slider->SetMax(vsz, TRUE);
}

void ScrollBar::Control(int id, int part, void *val)
{
    Point scroll;
    
    switch (id) {
    case eScrollBarUpLeft:
	part= cPartScrollStep;
	scroll[dir]= -1;
	break;
    case eScrollBarDownRight:
	part= cPartScrollStep;
	scroll[dir]= 1;
	break;
    case eScrollBarSlider:
	switch (part) {
	case eSliderPageUpLeft:
	    part= cPartScrollPage;
	    scroll[dir]= -1;
	    break;
	case eSliderPageDownRight:
	    part= cPartScrollPage;
	    scroll[dir]= 1;
	    break;
	case eSliderThumb:
	    if (dir) {
		part= cPartScrollVAbs;
		scroll.y= ((Point*) val)->y;
	    } else {
		part= cPartScrollHAbs;
		scroll.x= ((Point*) val)->x;
	    }
	    break;
	}
	break;
    default:
	VObject::Control(id, part, val);
	return;
    }
    VObject::Control(GetId(), part, &scroll);
}
