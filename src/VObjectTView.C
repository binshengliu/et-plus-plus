//$VObjectTextView,StretchWrapper, StretchWrapperStretcher$

#include "VObjectTView.h"
#include "VObjectText.h"
#include "Clipper.h"

//----- VObjectTextView ---------------------------------------------------

MetaImpl(VObjectTextView, (TP(kbdFocus), 0));

VObjectTextView::VObjectTextView(EvtHandler *eh, Rectangle r, VObjectText *t, eTextJust m, eSpacing sp, 
		       bool doWrap, TextViewFlags fl, Point b, int id)
				    : TextView(eh,r,t,m,sp,doWrap,fl,b,id) 
{
    kbdFocus= 0;
    t->SetView(this);
}

static VObject *overaVObject= 0;

Command *VObjectTextView::DispatchEvents(Point lp, Token t, Clipper *vf) 
{
    VObject *vop= overaVObject= ((VObjectText*)GetText())->ContainsPoint(lp);
    if (t.Code != eEvtLeftButton && t.Code != eEvtMiddleButton) {
	if (kbdFocus && kbdFocus->Enabled()) 
	    return kbdFocus->DispatchEvents(lp, t, vf);
	return TextView::DispatchEvents(lp, t, vf);
    }
    
    if (kbdFocus == 0) {
	if (vop) {
	    if (vop->WantsKbdFocus()) {
		SetNoSelection();
		kbdFocus= vop;
		kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, 0);
	    }
	    return vop->DispatchEvents(lp, t, vf);
	}
	return TextView::DispatchEvents(lp, t, vf);
    } else {
	if (vop) {
	    if (vop->WantsKbdFocus()) {
		if (vop != kbdFocus) {
		    kbdFocus->SendDown(cIdEndKbdFocus, cPartFocusChanged, 0);
		    kbdFocus= vop;
		    kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, 0);
		}
	    }
	    return vop->DispatchEvents(lp, t, vf);
	}
	kbdFocus->SendDown(cIdEndKbdFocus, cPartFocusChanged, 0);
	kbdFocus= 0;
	return TextView::DispatchEvents(lp, t, vf);
    }
}

GrCursor VObjectTextView::GetCursor(Point)
{
    if (overaVObject)
	return eCrsBoldArrow;
    return eCrsIBeam;
}

void VObjectTextView::Control(int id, int part, void *val)
{
    if (part == cPartExtentChanged) 
	((VObjectText*)GetText())->VObjectChangedSize((VObject*)val);
    else
	TextView::Control(id, part, val);
}

void VObjectTextView::SetOrigin(Point p)
{
     if (p != GetOrigin()) {
	TextView::SetOrigin(p);
	InvalidateVObjects(0, NumberOfLines()-1);
     }
}

void VObjectTextView::InvalidateRange(int from, int to)
{
    TextView::InvalidateRange(from,to);
    InvalidateVObjects(from, to);
}

void VObjectTextView::InvalidateRange(int from, Point fp, int to, Point tp)
{
    TextView::InvalidateRange(from, fp, to, tp);
    InvalidateVObjects(from, to);
}

void VObjectTextView::InvalidateVObjects(int from, int to)
{
    //---- invalidates the contained VObjects by setting the origin
    //     to -cMaxInt. VObjectText::Draw will position them on demand
    //     e.g. when they are drawn
    VObjectText *t= (VObjectText*)GetText();    
    if (!t->VObjectCount())
	return;
    int start= StartLine(max(from,0)),
	end= EndLine(to);
    Iterator *next= t->VObjectIterator();
    VObjectMark *m;
    while (m= (VObjectMark*)(*next)())
	if (m->Pos() >= start && m->Pos() <= end) 
	    m->Invalidate();
    SafeDelete(next);    
}

istream& VObjectTextView::ReadFrom(istream &s)
{
    TextView::ReadFrom(s);
    ((VObjectText*)GetText())->SetView(this);
    return s;   
}

Text *VObjectTextView::SetText(Text *newText)
{
    Text *ot= TextView::SetText(newText);
    kbdFocus= 0;
    if (newText->IsKindOf(VObjectText)) 
	((VObjectText*)newText)->SetView(this);
    else
	Error("SetText", "only accepts VObjectTexts");
    return ot;
}

//---- StretchWrapperStretcher ------------------------------------------------

class StretchWrapperStretcher: public Command {
    Point newExtent, oldExtent;
    class VObject *vob;
    Rectangle maxRect;
public:
    StretchWrapperStretcher(VObject *v);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point ap, Point pp, bool);
    void TrackConstrain(Point, Point, Point *);
    void DoIt();
    void UndoIt();
};

StretchWrapperStretcher::StretchWrapperStretcher(VObject *v) : Command(1212, "change extent")
{
    vob= v;
    View *vp= vob->GetView();
    if (vp->IsKindOf(StaticTextView)) {
	StaticTextView *stv= (StaticTextView*)vp;
	maxRect= Rectangle(stv->GetInnerOrigin(), stv->GetInnerExtent());
    } else
	maxRect= vob->GetView()->ContentRect();
}

Command *StretchWrapperStretcher::TrackMouse(TrackPhase tp, Point, Point, Point np)
{
    if (tp == eTrackRelease) {
	newExtent= np - vob->GetOrigin();
	oldExtent= vob->GetExtent();
	if (oldExtent == newExtent)
	    return gNoChanges;
	return this;
    }
    return this;
}

void StretchWrapperStretcher::TrackFeedback(Point, Point pp, bool)
{
    GrStrokeRect(NormRect(vob->GetOrigin(), pp));
}

void StretchWrapperStretcher::TrackConstrain(Point, Point, Point *np)
{    
    *np= Max(vob->GetOrigin()+ vob->GetMinSize().Extent(), *np);
    Rectangle r1, r= NormRect(vob->GetOrigin(), *np);
    r1= r.Intersect(maxRect);
    *np= r1.SE();
}

void StretchWrapperStretcher::DoIt()
{
    vob->SetExtent(newExtent);
}

void StretchWrapperStretcher::UndoIt()
{
    vob->SetExtent(oldExtent);
}

//---- StretchWrapper ---------------------------------------------------------

MetaImpl(StretchWrapper, (T(interiorOffset), T(extentDiff), T(border), TP(interior)));

StretchWrapper::StretchWrapper(VObject *in, Point b, int id) 
						    :CompositeVObject(id, in, 0)
{
    SetFlag(eVObjKbdFocus);
    kbdFocus= FALSE;
    interior= in;
    if (interior->IsKindOf(View))
	((View*)interior)->SetNextHandler(this);
    border= Max(Point(4,4),b);
}

Metric StretchWrapper::GetMinSize()
{
    interiorOffset= border;
    extentDiff= 2*interiorOffset;
    Metric m= interior->GetMinSize();
    return m.Expand(border);
}

void StretchWrapper::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    interior->SetOrigin(at+interiorOffset);
}

void StretchWrapper::SetExtent(Point e)
{
    VObject::SetExtent(e);
    e-= extentDiff;
    interior->SetExtent(e);
}

Command *StretchWrapper::DoLeftButtonDownCommand(Point p, Token, int)
{
    Rectangle r(ContentRect().SE()-gPoint8, gPoint8);
   
    if (r.ContainsPoint(p)) 
	return new StretchWrapperStretcher(this);
    return gNoChanges;
}

Command *StretchWrapper::DoKeyCommand(int, Point, Token)
{
    return gNoChanges;
}

void StretchWrapper::Control(int id, int part, void *val)
{
    if (part == cPartExtentChanged && val == interior) {
	VObject *vop= (VObject*)val;
	if (interior->GetExtent() != GetExtent()+extentDiff) // change from inside
	    SetExtent(interior->GetExtent()+extentDiff);
	SetOrigin(GetOrigin());
	VObject::Control(id, part, this);         // change from outside
	return;
    }
    VObject::Control(id, part, val);
}

void StretchWrapper::SendDown(int id, int part, void *val)
{
    switch (part) {
    case cPartEnableLayoutCntl:
	interior->SetFlag(eVObjLayoutCntl);
	break;
    case cPartFocusChanged:
	kbdFocus= !kbdFocus;
	ForceRedraw();
	CompositeVObject::SendDown(id, part, val);
	break;
    default:
	CompositeVObject::SendDown(id, part, val);
    }    
}

void StretchWrapper::Draw(Rectangle r)
{
    CompositeVObject::Draw(r);
    if (kbdFocus) {
	GrSetPenNormal();
	GrSetPenPattern(ePatGrey50);
	GrSetPenSize(2);
	GrStrokeRect(ContentRect());
	GrPaintRect(Rectangle(ContentRect().SE()-gPoint4, gPoint4), ePatBlack);
    }
}

GrCursor StretchWrapper::GetCursor(Point p)
{
    Rectangle r(ContentRect().SE()-gPoint8, gPoint8);
    
    if (r.ContainsPoint(p)) 
	return eCrsMoveStretch;
    else
	return VObject::GetCursor(p);
}

int StretchWrapper::Base()
{
    return interior->Base()+border.y;
}

ostream& StretchWrapper::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << border SP << interiorOffset SP << extentDiff SP << interior SP;
}

istream& StretchWrapper::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    s >> border >> interiorOffset >> extentDiff >> interior;
    if (interior->IsKindOf(View))
	((View*)interior)->SetNextHandler(this);
    return s;
}
