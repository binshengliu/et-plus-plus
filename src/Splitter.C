//$Splitter,SplitScroller,SplitBar$
#include "Splitter.h"
#include "Scroller.h"

const int cSplitGap= 3;

//---- SplitBar ----------------------------------------------------------------

class SplitBar: public VObject {
    Splitter *splitframe;
    bool vertical;
public:
    MetaDef(SplitBar);
    SplitBar(Splitter *sf, bool v, int w, int h) : VObject(Point(w, h), cIdNone)
	{ splitframe= sf; vertical= v; }
    void DrawInner(Rectangle r, bool feedback)
	{ if (!feedback) GrPaintRect(r, ePatBlack); }
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void SetOrigin(Point at);
};

MetaImpl(SplitBar, (TP(splitframe), TB(vertical), 0));

Command *SplitBar::DoLeftButtonDownCommand(Point, Token, int clicks)
{
    if (clicks >= 2) {
	splitframe->SetSplit(vertical, Point(cSplitGap));
	return gNoChanges;
    }
    Rectangle constrainRect(splitframe->contentRect);
    if (vertical) {
	constrainRect.origin.x+= constrainRect.extent.x-Width();
	constrainRect.extent.x= Width();
    } else {
	constrainRect.origin.y+= constrainRect.extent.y-Height();
	constrainRect.extent.y= Height();
    }
    Command *cmd= new VObjectMover(this, constrainRect);
    cmd->ResetFlag(eCmdCanUndo);
    return cmd;
}

void SplitBar::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    splitframe->SetSplit(vertical, at);
}

//---- SplitScroller -----------------------------------------------------------

class SplitScroller: public Scroller {
public:
    MetaDef(SplitScroller);
    SplitScroller(VObject *v, Point ms, int id, ScrollDir sd)
						    : Scroller(v, ms, id, sd)
	{ }
    void Control(int, int part, void *vp);
};

MetaImpl0(SplitScroller);

void SplitScroller::Control(int id, int part, void *vp)
{
    if (part == cPartViewSize)
	Scroller::Control(id, part, vp);
    else
	VObject::Control(GetId(), part, vp);
}

//---- Splitter ----------------------------------------------------------------

MetaImpl(Splitter, (T(split), T(ms), TP(vop), 0));

Splitter::Splitter(VObject *v, Point p, int id)
					: CompositeVObject(id, (Collection*)0)
{
    vop= v;
    ms= p;
    Init(v, ms);
    ResetFlag(eVObjOpen);
}

void Splitter::Init(VObject *v, Point p)
{
    split= Point(-cSplitGap);
    for (int i= 0; i < 4; i++)
	Add(new SplitScroller(v, p, i, ScrollDir(i)));
    Add(new SplitBar(this, FALSE, cSplitGap, cScrollBarSize));
    Add(new SplitBar(this, TRUE, cScrollBarSize, cSplitGap));
}

void Splitter::Open(bool mode)
{
    VObject::Open(mode);
    At(3)->Open(mode);  // right/bottom Scroller is always open
}

void Splitter::SendDown(int id , int part, void *val)
{
    At(3)->SendDown(id, part, val); 
}

void Splitter::Control(int id, int part, void *val)
{
    if (id >= 0 && id < 4) {
	if (part != cPartScrollPos) {
	    VObject *v0= At(0), *v1= At(1), *v2= At(2), *v3= At(3);
	    Point px, py;
	    px= py= *(Point*)val;
	    if (id < 2)
		py.y= px.x= 0;
	    else
		py.x= px.y= 0;
	    if (id == 1 || id == 2) {
		if (v0->IsOpen())
		    ((Scroller*)v0)->Scroller::Control(id, part, &px);
		if (v3->IsOpen())
		    ((Scroller*)v3)->Scroller::Control(id, part, &py);
	    }
	    if (id == 0 || id == 3) {
		if (v1->IsOpen())
		    ((Scroller*)v1)->Scroller::Control(id, part, &px);
		if (v2->IsOpen())
		    ((Scroller*)v2)->Scroller::Control(id, part, &py);
	    }
	}
	VObject *v= At(id);
	((Scroller*)v)->Scroller::Control(id, part, val);
    } else
	VObject::Control(id, part, val);
}

Metric Splitter::GetMinSize()
{
    return At(3)->GetMinSize();
}

void Splitter::TestOpen(int i, int w, int h, Point &minsize)
{
    Point e(w,h);
    register VObject *scr= At(i);
    if (e >= minsize) {
	if (! scr->IsOpen())
	    scr->Open();
	scr->SetExtent(e);
    } else if (scr->IsOpen())
	scr->Close();
}

void Splitter::SetExtent(Point e)
{
    Point minsize= GetMinSize().Extent();
    VObject::SetExtent(e);
    
    TestOpen(0, split.x, split.y, minsize);
    TestOpen(1, e.x-split.x-cSplitGap, split.y, minsize);
    TestOpen(2, split.x, e.y-split.y-cSplitGap, minsize);
    At(3)->SetExtent(Point(e.x-split.x-cSplitGap, e.y-split.y-cSplitGap));
}

void Splitter::SetOrigin(Point at)
{
    Point p, e= GetExtent();
    
    VObject::SetOrigin(at);
    if (At(0)->IsOpen())
	At(0)->SetOrigin(at);
    if (At(1)->IsOpen())
	At(1)->SetOrigin(at+Point(split.x+cSplitGap, 0));
    if (At(2)->IsOpen())
	At(2)->SetOrigin(at+Point(0, split.y+cSplitGap));
    if (At(3)->IsOpen())
	At(3)->SetOrigin(at+Point(split.x+cSplitGap, split.y+cSplitGap));
    p= Point(split.x<0 ? e.x-cScrollBarSize : split.x, e.y-cScrollBarSize);
    At(4)->SetOrigin(at+p);
    p= Point(e.x-cScrollBarSize, split.y<0 ? e.y-cScrollBarSize : split.y);
    At(5)->SetOrigin(at+p);
}

void Splitter::SetSplit(bool vertical, Point at)
{
    Point minSize= GetMinSize().Extent(), e= GetExtent();
    at-= GetOrigin();
    
    if (vertical) {
	if (at.y < minSize.y || at.y > e.y - minSize.y)
	    at.y= -cSplitGap;
    } else {
	if (at.x < minSize.x || at.x > e.x - minSize.x)
	    at.x= -cSplitGap;
    }
    
    if (split[vertical] != at[vertical]) {
	split[vertical]= at[vertical];
	ForceRedraw();
	SetExtent(contentRect.extent);
	SetOrigin(contentRect.origin);
    }
}

ostream& Splitter::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << vop SP << ms SP;
}

istream& Splitter::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    FreeAll();
    s >> vop >> ms;
    Init(vop, ms);
    SetExtent(GetExtent());
    return s;
}
