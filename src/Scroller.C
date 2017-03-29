//$Scroller,ScrollClipper$
#include "Scroller.h"
#include "ScrollBar.h"
#include "Clipper.h"
#include "View.h"

class ScrollClipper: public Clipper {
public:
    ScrollClipper(VObject *v, Point minsize, int id) : Clipper(v, minsize, id)
	{ }
    void Control(int id, int part, void *val)
	{ VObject::Control(id, part, val); }
};

//---- Scroller ----------------------------------------------------------------

MetaImpl(Scroller, (TP(vf), TE(controls), TE(visibleControls), TP(vop), T(ms), 0));

Scroller::Scroller(VObject *v, Point e, int id, ScrollDir w)
					: CompositeVObject(id, (Collection*) 0)
{
    controls= w;
    vop= v;
    ms= e;
    Init(v, e, id);
}

void Scroller::Init(VObject *v, Point e, int id)
{
    visibleControls= eScrollNone;
    for (int i= 0; i<4; i++)
	if (controls & BIT(i))
	    Add(sb[i]= new ScrollBar(cIdNone/*i+1111*/, (Direction) EVEN(i)));
    Add(vf= new ScrollClipper(v, e, id));
}

void Scroller::Open(bool mode)
{
    VObject::Open(mode);
    vf->Open(mode);
}

void Scroller::Control(int id, int part, void *val)
{
    switch (part) {
    case cPartScrollPos:
	BubbleUpdate(*((Point*)val));
	break;
	
    case cPartViewSize:
	ViewSizeChanged(*((Point*)val));
	break;
	
    case cPartScrollPage:
    case cPartScrollRel:
    case cPartScrollStep:
    case cPartScrollAbs:
    case cPartScrollHAbs:
    case cPartScrollVAbs:
	vf->Scroll(part, *((Point*)val));
	break;
	
    default:
	CompositeVObject::Control(id, part, val);
	break;
    }
}

void Scroller::BubbleUpdate(Point bs)
{
    for (int i= 0; i<4; i++)
	if (visibleControls & BIT(i))
	    sb[i]->BubbleUpdate(bs);
}

bool Scroller::NeedControls(Point e, Point vs)
{
    ScrollDir lastVisibleControls= visibleControls;
    
    visibleControls= (ScrollDir)(controls & ~eScrollHideScrolls);
    if (controls & eScrollHideScrolls) {
	if (vs.x < e.x)    // we don't need horizontal scrollbars
	    visibleControls= (ScrollDir)(visibleControls&~(eScrollUp | eScrollDown));
	if (vs.y < e.y)    // we don't need vertical scrollbars
	    visibleControls= (ScrollDir)(visibleControls&~(eScrollLeft | eScrollRight));
    }
    return visibleControls != lastVisibleControls;
}

void Scroller::ViewSizeChanged(Point newviewsize)
{
    Point e= GetExtent();

    if (NeedControls(e, newviewsize)) {
	SetExtent(e);
	SetOrigin(GetOrigin());
	ForceRedraw();
    } else {
	for (int i= 0; i<4; i++)
	    if (visibleControls & BIT(i))
		sb[i]->ViewSizeChanged(newviewsize);
    }
}

void Scroller::Draw(Rectangle r)
{
    GrSetPenNormal();
    GrStrokeRect(vf->contentRect.Expand(gPoint1));
    CompositeVObject::Draw(r);
}

Metric Scroller::GetMinSize()
{
    Point clipperSize, sbSizes, ee;

    clipperSize= vf->GetMinSize().extent;
    for (int i= 0; i < 4; i++) {
	if (controls & BIT(i))
	    ee= sb[i]->GetMinSize().extent;
	else
	    ee= gPoint1;
	if (EVEN(i)) {      // right & left scrollbar
	    sbSizes.x+= ee.x;
	    clipperSize.y= max(clipperSize.y, ee.y);
	} else {            // up & down scrollbar
	    sbSizes.y+= ee.y;
	    clipperSize.x= max(clipperSize.x, ee.x);
	}
    }
    return clipperSize + sbSizes;
}

void Scroller::SetBgInk(Ink *bg)
{
    vf->SetBgInk(bg);
}

Ink *Scroller::GetBgInk()
{
    return vf->GetBgInk();
}

void Scroller::SetExtent(Point e)
{
    Point ee, eee, viewExtent= vf->GetViewSize().extent;

    VObject::SetExtent(e);
    
    NeedControls(e, viewExtent);

    if (visibleControls & eScrollUp)
	e.y-= sb[3]->GetMinSize().extent.y;
    else
	ee.y--;
    
    if (visibleControls & eScrollDown)
	e.y-= sb[1]->GetMinSize().extent.y;
    else
	ee.y--;
    
    if (visibleControls & eScrollLeft)
	e.x-= sb[2]->GetMinSize().extent.x;
    else
	ee.x--;

    if (visibleControls & eScrollRight)
	e.x-= sb[0]->GetMinSize().extent.x;
    else
	ee.x--;

    ee+= e;

    for (int i= 0; i<4; i++) {
	if (controls & BIT(i)) {
	    if (visibleControls & BIT(i)) {
		eee= sb[i]->GetMinSize().extent;
		if (EVEN(i))
		    eee.y= e.y;
		else
		    eee.x= e.x;
		if (!sb[i]->IsOpen())
		    sb[i]->Open(TRUE);
		sb[i]->Init(eee, ee, viewExtent);
	    } else {
		if (sb[i]->IsOpen())
		    sb[i]->Open(FALSE);
	    }
	}
    }
    vf->SetExtent(ee);
}

void Scroller::SetOrigin(Point at)
{
    Point left, pp(1), middle= vf->GetExtent();
    
    VObject::SetOrigin(at);
   
    if (visibleControls & eScrollLeft)
	pp.x= left.x= sb[2]->Width();
    if (visibleControls & eScrollUp)
	pp.y= left.y= sb[3]->Height();
	
    vf->SetOrigin(at+pp);
    
    if (visibleControls & eScrollRight)
	sb[0]->SetOrigin(at+Point(pp.x+middle.x, left.y));
    if (visibleControls & eScrollDown)
	sb[1]->SetOrigin(at+Point(left.x, pp.y+middle.y));
    if (visibleControls & eScrollLeft)
	sb[2]->SetOrigin(at+Point(0, left.y));
    if (visibleControls & eScrollUp)
	sb[3]->SetOrigin(at+Point(left.x, 0));        
}

ostream& Scroller::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << vop SP << controls SP << ms SP;
}

istream& Scroller::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    FreeAll();
    s >> vop >> Enum(controls) >> ms;
    Init(vop, ms, GetId());
    SetExtent(GetExtent());
    if (vf) {   // CSE
	vf->SetFlag(eVObjOpen);
	vop->AddToClipper(vf);
    }
    return s;
}
