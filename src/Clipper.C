//$Clipper$
#include "Clipper.h"
#include "View.h"
#include "Error.h"
#include "Token.h"
#include "BlankWin.h"
#include "ObjList.h"
#include "String.h"
#include "WindowPort.h"

//---- Clipper -----------------------------------------------------------------

MetaImpl(Clipper, (TP(vop), T(minExtent), T(relOrigin), T(offset), 0)); 

Clipper::Clipper(VObject *vp, Point minsize, int id, GrPattern bg)
						    : VObject(minsize, id)
{
    bgcolor= bg;
    vop= vp;
    offset= relOrigin= gPoint0;
    SetMinExtent(minsize);
    ResetFlag(eVObjOpen);
}

void Clipper::Open(bool mode)
{
    if (IsOpen() == mode)
	return;
    VObject::Open(mode);
    if (vop) {
	if (mode) {
	    vop->AddToClipper(this);
	    ViewSizeChanged(gPoint0, vop->GetExtent());
	} else
	    vop->RemoveFromClipper(this);
	vop->Open(mode);
    }
}

void Clipper::Enable(bool b, bool redraw)
{
    VObject::Enable(b, redraw);
    if (vop)
	vop->Enable(b, redraw);
}

Rectangle Clipper::GetViewSize()
{
    if (vop)
	return vop->GetExtent();
    return GetExtent();
}

Rectangle Clipper::GetViewedRect()
{
    return Rectangle(relOrigin, contentRect.extent);
}

VObject *Clipper::GetViewedVObject()
{
    return vop;
}

Metric Clipper::GetMinSize()
{
    Point me(minExtent);
    if (vop && (minExtent.x == -1 || minExtent.y == -1)) {
	Point e= vop->GetMinSize().extent;
	if (minExtent.x == -1)
	    me.x= e.x;
	if (minExtent.y == -1)
	    me.y= e.y;
    }
    return me;
}

void Clipper::SetMinExtent(Point e)
{
    minExtent= e;
    if (e.x == -1)
	SetFlag(eVObjHFixed);
    if (e.y == -1)
	SetFlag(eVObjVFixed);
}

void Clipper::ShowsView(VObject *vp)
{
    RemoveView(vop);
    vop= vp;
    if (vop) {
	vop->AddToClipper(this);
	ViewSizeChanged(gPoint0, vop->GetExtent());
    }
}

void Clipper::RemoveView(VObject *vp)
{
    if (vop && vop == vp) {
	vop->RemoveFromClipper(this);
	vop= 0;
    }
}

void Clipper::ViewSizeChanged(Point oldsize, Point newsize)
{
    if (vop && newsize != oldsize) {
	Rectangle r[4], oldViewSize= oldsize, viewSize= newsize;
	int nr= Difference(r, oldViewSize, viewSize);
	for (int i= 0; i < nr; i++)
	    InvalidateViewRect(r[i]);
	Send(GetId(), cPartViewSize, &newsize);
    }
}

void Clipper::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    offset= contentRect.origin - relOrigin;
}

void Clipper::SetExtent(Point e)
{
    Rectangle viewSize= GetViewSize();
    
    VObject::SetExtent(e);
    if (viewSize.IsNotEmpty()) {
	Point oldrelOrigin= relOrigin;
	
	relOrigin= Min(relOrigin, viewSize.extent - e);
	if (viewSize.extent.x < e.x)
	    relOrigin.x= 0;
	if (viewSize.extent.y < e.y)
	    relOrigin.y= 0;
	if (relOrigin != oldrelOrigin)
	    Send(GetId(), cPartScrollPos, &relOrigin);
    }
}

void Clipper::SetFocus(Rectangle r, Point o)
{
    r.origin= ContainerPoint(r.origin);
    GetContainer()->SetFocus(Inter(contentRect, r), ContainerPoint(o));
}

void Clipper::DrawInner(Rectangle r, bool)
{
    Rectangle oldcliprect= port->GetCliprect();
    Point oldoffset= port->GetOrigin();
    GrSetClip(r+oldoffset, offset+oldoffset);
    
    r.origin-= offset;
    if (bgcolor != ePatNone)
	GrPaintRect(r, bgcolor);
    if (vop)
	vop->DrawAll(r, FALSE);
    
    GrSetClip(oldcliprect, oldoffset);
}

VObject *Clipper::Detect(BoolFun find, void *arg)
{
    VObject *v;

    if (vop && (v= vop->Detect(find, arg)))
	return v;
    if (VObject::Detect(find, arg))
	return (VObject*) this;
    return 0;
}

Command *Clipper::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    Command *cmd= 0;
    if (vop)
	cmd= vop->Input(lp-offset, t, this);
    else
	cmd= VObject::DispatchEvents(lp-offset, t, vf);
    if (cmd)
	PerformCommand(cmd);
    return gNoChanges;
}

Point Clipper::ContainerPoint(Point p)
{
    return p+offset;
}

void Clipper::SetBgInk(Ink *ip)
{
    bgcolor= ip;
}

Ink *Clipper::GetBgInk()
{
    return bgcolor;
}

//---- Scrolling ----------------------------------------------------------------

Point Clipper::AutoScroll(Point p)
{
    Point scroll, newp;
    
    scroll= newp= Rectangle(contentRect.extent).Constrain(p);
    scroll-= p;
    if (scroll != gPoint0) 
	Control(GetId(), cPartScrollRel, &scroll);
    return newp;
}

void Clipper::RevealRect(Rectangle r, Point minToSee)
{
    Rectangle vr= GetViewedRect();
    Point scroll= 0, rUL= r.NW(), rLR= r.SE(), vrUL= vr.NW(), vrLR= vr.SE();
    int v;
    
    for (v= 0; v <= 1; v++) {
	if (rUL[v] < vrUL[v] && rLR[v] > vrLR[v])
	    continue;
	if (rLR[v] - vrUL[v] < minToSee[v])
	    scroll[v]= vrUL[v] - rUL[v];
	if (vrLR[v] - rUL[v] < minToSee[v])
	    scroll[v]= vrLR[v] - rLR[v];
    }
    if (scroll != gPoint0)
	Control(GetId(), cPartScrollRel, &scroll);
}

void Clipper::RevealAlign(Rectangle r, VObjAlign al)
{
    Rectangle vr= GetViewedRect();
    Point scroll;

    switch (al & eVObjH) {
    case eVObjHCenter:
	scroll.x= (vr.Center().x - r.Center().x)/2;
	break;
    case eVObjHRight:
	scroll.x= vr.NE().x - r.NE().x;
	break;
    default:
	scroll.x= vr.NW().x - r.NW().x;
	break;
    }
    switch (al & eVObjV) {
    case eVObjVCenter:
	scroll.y= (vr.Center().y - r.Center().y)/2;
	break;
    case eVObjVBottom:
	scroll.y= vr.SW().y - r.SW().y;
	break;
    default:
	scroll.y= vr.NW().y - r.NW().y;
	break;
    }
    if (scroll != gPoint0)
	Control(GetId(), cPartScrollRel, &scroll);
}

void Clipper::Control(int id, int part, void *val)
{
    if (part == cPartScrollRel)
	Scroll(part, *(Point*)val);
    else
	VObject::Control(id, part, val);
}

void Clipper::SendDown(int id, int part, void *val)
{
    switch (part) {
    case cPartScrollPage:
    case cPartScrollAbs:
    case cPartScrollHAbs:
    case cPartScrollVAbs:
    case cPartScrollRel:
    case cPartScrollStep:
	Scroll(part, *((Point*)val));
	break;
    case cPartIncr:
	Scroll(cPartScrollStep, Point(0, -1));
	break;
    case cPartDecr:
	Scroll(cPartScrollStep, Point(0, 1));
	break;
    default:
	vop->SendDown(id, part, val);
	//VObject::SendDown(id, part, val);
	break;
    }
}

void Clipper::Scroll(int mode, Point scroll, bool redraw)
{
    Point leftCorner, newOrigin, delta;
    Rectangle r;
    
    switch(mode) {
    case cPartScrollStep:
	delta= 32 * -scroll;
	break;
    case cPartScrollPage:
	delta= contentRect.extent * -scroll;
	break;
    case cPartScrollAbs:
	delta= relOrigin - scroll;
	break;
    case cPartScrollHAbs:
	delta.x= relOrigin.x - scroll.x;
	break;
    case cPartScrollVAbs:
	delta.y= relOrigin.y - scroll.y;
	break;
    case cPartScrollRel:
	delta= scroll;
	break;
    }
    
    leftCorner= Max(gPoint0, GetViewSize().extent - contentRect.extent);
    newOrigin= relOrigin-delta;
	
    if (vop && vop->IsKindOf(View))
	((View*)vop)->ConstrainScroll(&newOrigin);
    
    delta= relOrigin - Min(Max(gPoint0, newOrigin), leftCorner);
    
    if (delta == gPoint0)
	return;
	
    if (! IsOpen())
	redraw= FALSE;
	
    if (redraw) {
	UpdateEvent();
	Focus();
	r= GetViewedRect();
    }
    
    relOrigin-= delta;
    offset+= delta;
    
    if (redraw) {
	bool oldbatch= gBatch;
	
	((WindowPort*)port)->ScrollRect(r, delta);
	if (Width() > 200 && Height() > 200
		    && (abs(delta.x) > Width()/3 || abs(delta.y) > Height()/3))
	gBatch= FALSE;
	UpdateEvent();
	gBatch= oldbatch;
    }
    Send(GetId(), cPartScrollPos, &relOrigin);
    if (redraw)
	UpdateEvent();
}

// returns TRUE on exit loop
bool Clipper::TrackOnce(Command** tracker, TrackPhase atp, Point ap, Point pp, Point lp)
{
    Command *newTracker;

    if (*tracker == 0) {
	*tracker= gNoChanges;
	return TRUE;
    }

    newTracker= (*tracker)->TrackMouse(atp, ap, pp, lp);
    if (newTracker != *tracker) {
	if (*tracker && *tracker != gNoChanges)
	    delete *tracker;
	*tracker= newTracker;
	return (newTracker == gNoChanges);
    }
    return FALSE;
}

// called from View
void Clipper::DrawInFocus(VoidObjMemberFunc of, Object *op, void *v1, void *v2, void *v3, void *v4)
{
    if (IsOpen()) {
	Focus();
	(op->*of)(v1, v2, v3, v4);
    }
}

void Clipper::Feedback(Object *tracker, void *ap, void *pp, void *turniton)
{
    GrPattern pat= ePatBlack;
    if (((Command*)tracker)->TestFlag(eCmdFullScreen) ||
			! ((Command*)tracker)->TestFlag(eCmdNoReplFeedback))
	pat= ePatXor;
    GrSetPattern(pat);
    GrSetPenPattern(pat);
    GrSetTextPattern(pat);
    GrSetPenSize(1);
    ((Command*)tracker)->TrackFeedback(*((Point*)ap), *((Point*)pp), *((bool*)turniton));
    GrGiveHint(eHintTextUnbatch);
    UpdateEvent();
}

void Clipper::FeedbackOnce(Command *tracker, Point ap, Point pp, bool turniton)
{            
    if (!tracker->TestFlag(eCmdNoReplFeedback) && vop && vop->IsKindOf(View))
	((View*)vop)->ShowInAllClippers((VoidObjMemberFunc)&Clipper::Feedback, this,
		(void*) tracker, (void*) &ap, (void*) &pp, (void*) &turniton);
    else
	Feedback(tracker, (void*) &ap, (void*) &pp, (void*) &turniton);
}
 
Command *Clipper::TrackInContent(Point lp, Token token, Command* tracker)
{
    static int level= 0;
    static bool haveSeenUp;
    
    bool autoscroll= FALSE, havereset, isidle= FALSE, fullscreen, done;
    Point newlp, ap, pp, dp, delta, oldrelorigin;
    BlankWin *bwin= GetWindow();
      
    dp= token.Pos - lp;
    level++;
    if (level == 1)
	haveSeenUp= FALSE;
    Focus();
    
    bwin->Grab(TRUE, fullscreen= tracker->TestFlag(eCmdFullScreen));
    havereset= FALSE;        

restart2:
    ap= pp= lp;
    tracker->TrackConstrain(ap, pp, &lp);
    ap= pp= lp;

    if (TrackOnce(&tracker, eTrackPress, ap, pp, lp))
	goto out;
    FeedbackOnce(tracker, ap, pp, TRUE);

restart:
    while (IsOpen()) {
	if (level > 0 && haveSeenUp && !tracker->TestFlag(eCmdMoveEvents))
	    break;
	    
	done= FALSE;
	while (!done) {
	    if (isidle || autoscroll)
		bwin->ReadEvent(0, TRUE);
	    else
		bwin->ReadEvent(200, FALSE);
	    
	    switch (gToken.Code) {
	    
	    case eEvtNone:
		if (autoscroll) {
		    gToken.Code= eEvtLocMove;   // simulate a move event
		    done= TRUE;
		}
		if (tracker->TestFlag(eCmdIdleEvents))
		    done= isidle= TRUE;
		break;
		
	    case eEvtRightButton:
	    case eEvtMiddleButton:
	    case eEvtLeftButton:
		if (gToken.Flags & eFlgButDown)
		    haveSeenUp= TRUE;
		else
		    goto restart2;
		done= TRUE;                     // return button event immediately
		
	    case eEvtLocMove:
		done= TRUE;
		
	    case eEvtIdle:                      // ignore idle events
	    default:
		break;
	    }
	}

	token= gToken;
	lp= token.Pos - dp;
	
	if (haveSeenUp)
	    break;
	      
	FeedbackOnce(tracker, ap, pp, FALSE);   // clear previous feedback
	
	tracker->TrackConstrain(ap, pp, &lp);
	if (! fullscreen) {
	    oldrelorigin= relOrigin;
	    newlp= AutoScroll(lp-relOrigin);
	    delta= relOrigin - oldrelorigin;
	    if (delta != gPoint0) {
		autoscroll= TRUE;
		lp= newlp + relOrigin;
		dp-= delta;
		// Focus();
	    } else
		autoscroll= FALSE;
	}
	oldrelorigin= relOrigin;
	if (TrackOnce(&tracker,
		token.Code == eEvtLocMove ? eTrackMove : eTrackIdle, ap, pp, lp))
	    goto out;
	delta= relOrigin - oldrelorigin;
	dp-= delta;
	
	pp= lp;
	FeedbackOnce(tracker, ap, pp, TRUE);    // new feedback
    }
    FeedbackOnce(tracker, ap, pp, FALSE);   // clear last feedback
    tracker->TrackConstrain(ap, pp, &lp);
	
    if (! tracker->TestFlag(eCmdMoveEvents)) {
	bwin->Grab(FALSE, fullscreen);
	havereset= TRUE;
	level--;
    }
		
    TrackOnce(&tracker, IsOpen() ? eTrackRelease : eTrackExit, ap, pp, lp);

    if (tracker->TestFlag(eCmdMoveEvents)) {
	haveSeenUp= FALSE;
	goto restart;
    }
out:
    Focus();
    if (! havereset) {
	bwin->Grab(FALSE, fullscreen);
	level--;
    }
    return tracker;
}

ostream& Clipper::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << minExtent SP << vop SP << bgcolor SP;
}

istream& Clipper::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    s >> minExtent >> vop >> bgcolor;
    ShowsView(vop);
    SetContentRect(contentRect, TRUE);
    return s;
}

void Clipper::InspectorId(char *buf, int sz)
{
    if (vop) 
	vop->InspectorId(buf, sz);
    else
	VObject::InspectorId(buf, sz);   
}

void Clipper::Parts(Collection* col)
{
    VObject::Parts(col);
    if (vop)
	col->Add(vop);
}
