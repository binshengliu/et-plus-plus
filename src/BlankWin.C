//$BlankWin,BlankWindowStretcher,InspectCommand$
#include "BlankWin.h"
#include "Error.h"
#include "WindowPort.h"
#include "WindowSystem.h"
#include "Menu.h"
#include "ObjectTable.h"
#include "ClassManager.h"

#include "FixedStorage.h"
#include "Storage.h"

Token gToken;
BlankWin *gWindow;

//---- InspectCommand ----------------------------------------------------------

class InspectCommand : public Command {
    BlankWin *win;
    VObject *newvop, *lastvop;
public:
    InspectCommand(BlankWin *bw)
	{ win= bw; lastvop= 0; }
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase atp, Point, Point, Point np);
};

void InspectCommand::TrackFeedback(Point, Point, bool)
{
    if (newvop != lastvop) {
	if (lastvop)
	    GrInvertRect(lastvop->contentRect);
	if (newvop)
	    GrInvertRect(newvop->contentRect);
	lastvop= newvop;
    }
}

Command *InspectCommand::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    newvop= win->GetViewedVObject()->FindItem(np);
    if (atp == eTrackRelease && newvop) {
	VObject *v= newvop;
	newvop= 0;
	TrackFeedback(gPoint0, gPoint0, FALSE);
	v->Inspect();
	return gNoChanges;
    }
    return this;
}

//---- BlankWin ----------------------------------------------------------------

MetaImpl(BlankWin, (TP(nexthandler), 0));

BlankWin::BlankWin(Point extent, BWinFlags f) : Clipper(0, extent)
{
    Init(0, eCrsBoldArrow, f);
}

BlankWin::BlankWin(VObject *vp, Point extent, BWinFlags f, Ink *bg,
			    GrCursor curs) : Clipper(vp, extent, cIdNone, bg)
{
    Init(0, curs, f);
}

BlankWin::BlankWin(EvtHandler *eh, VObject *vp, Point extent, BWinFlags f)
							: Clipper(vp, extent)
{
    Init(eh, eCrsBoldArrow, f);
}

void BlankWin::Init(EvtHandler *eh, GrCursor curs, BWinFlags f)
{
    nexthandler= eh;
    ResetFlag(eVObjOpen);
    SetFlag(f);
    cursor= curs;
}

BlankWin::~BlankWin()
{
    if (vop) {
	// vop->FreeAll(); // Causes double delete,
	// when called by ~Dialog, tom@izf.tno.nl
	SafeDelete(vop);
    }
    if (portDesc) {
	portDesc->Remove();     // mark as free
	portDesc= 0;
    }
}

EvtHandler *BlankWin::GetNextHandler()
{
    return nexthandler;
}

void BlankWin::InvalidateRect(Rectangle r)
{
    if (GetExtent() != gPoint_1)    // resize event pending; ignore 
	MakePort()->InvalidateRect(r);
}

void BlankWin::SetExtent(Point e)
{
    if (portDesc && (e != GetExtent())) {
	contentRect.extent= gPoint_1;
	portDesc->SetExtent(e);
    }
}

void BlankWin::SetOrigin(Point at)
{
    VObject::SetOrigin(gPoint0);
    if (portDesc)
	portDesc->SetOrigin(GetRect().origin+at);
}

void BlankWin::SetFocus(Rectangle r, Point o)
{
    if (IsOpen() && portDesc) {
	GrSetPort(portDesc);
	GrSetClip(Inter(contentRect, r), o);
    }
}

void BlankWin::UpdateEvent()
{
    Rectangle r, *rp;
    register int i, n;
    
    if (!IsOpen() || portDesc == 0 || portDesc->inval <= 0)
	return;
    GrSetPort(portDesc);
    rp= portDesc->invalRects;
    n= portDesc->inval;

    if (gBatch) {
	for (i= n-1; i >= 0; i--) {
	    GrSetClip(Inter(contentRect, rp[i]), gPoint0);
	    GrSetPenNormal();
	    GrSetPattern(ePatBlack);
	    GrGiveHint(eHintBatch, sizeof(Rectangle), &rp[i]);
	    GrGiveHint(eHintTextBatch);
	    DrawAll(rp[i], FALSE);
	    GrGiveHint(eHintTextUnbatch);
	    GrGiveHint(eHintUnbatch);
	}
    } else {
	GrGiveHint(eHintLock, sizeof(Rectangle), &portDesc->invalBounds);
	for (i= n-1; i >= 0; i--) {
	    GrSetClip(Inter(contentRect, rp[i]), gPoint0);
	    GrSetPenNormal();
	    GrSetPattern(ePatBlack);
	    GrGiveHint(eHintTextBatch);
	    DrawAll(rp[i], FALSE);
	    GrGiveHint(eHintTextUnbatch);
	}
	GrGiveHint(eHintUnlock);
    }
    portDesc->inval= 0;
}

void BlankWin::Update()
{
    SetContainer(0);
    vop->CalcExtent();
    vop->SetOrigin(gPoint0);
}

void BlankWin::input(Token *t)
{
    if (! TestFlag(eBWinOverlay))
	gWindow= this;
    if (portDesc == 0)
	return;
    GrSetPort(portDesc);
    
    if (t->Code == eEvtDamage) {
	if (t->Flags == eFlgDamage1) {
	    if (! IsOpen())
		Clipper::Open(TRUE);
	    if (t->ext != GetExtent()) {
		VObject::SetExtent(t->ext);
		if (vop)
		    vop->SetContentRect(Rectangle(gPoint0, t->ext), TRUE);
	    }
	}
    } else {
	gToken= *t;
	
	if ((t->Code == eEvtLeftButton) && !(t->Flags & eFlgButDown)
				&& t->Flags == (eFlgShiftKey|eFlgMetaKey)) {
	    TrackInContent(gToken.Pos, gToken, new InspectCommand(this));
	    return;
	}
	if (t->IsKey() && t->Flags == (eFlgShiftKey|eFlgCntlKey|eFlgMetaKey)) {
	    switch (t->MapToAscii()) {
	    case 'q':           // emergency exit
		Close();
		break;
	    case 't':
		Abort();
		break;
	    case 'p':
		Print();
		break;
	    case 'v':
		ObjectTable::VisitObjects();
		break;
	    case 's':
		gClassManager->InstanceStatistics();
		break;
	    case 'i':
		gClassManager->InstanceStatistics(TRUE);
		break;
	    case 'e':
		Error("input", "forced invocation of ErrorHandler");
		break;
	    case 'w':
		SetIgnoreLevel(0);
		break;
	    case 'm':
		MemPools::PrintStatistics();
		break;
	    }
	    return;
	}
	PerformCommand(DispatchEvents(gToken.Pos, gToken, this));
    }
    UpdateEvent();
}

Token BlankWin::ReadEvent(int timeout, bool overread)
{
    MakePort()->GetEvent(&gToken, timeout, overread);
    return gToken;
}

GrCursor BlankWin::GetCursor(Point)
{
    return cursor;
}

class WindowPort *BlankWin::MakePort()
{
    if (portDesc == 0) {
	portDesc= gWindowSystem->MakeWindow((InpHandlerFun) &BlankWin::input, this,
			TestFlag(eBWinOverlay), TestFlag(eBWinBlock), FALSE);
	portDesc->cursor= cursor;
    }
    return portDesc;
}

void BlankWin::Open(bool mode)
{
    if (mode)
	OpenAt(GetRect().origin);
    else {
	Clipper::Open(FALSE);
	if (portDesc)
	    portDesc->Hide();
    }
}

void BlankWin::OpenAt(Point p, VObject *fp)
{
    WindowPort *fatherport= 0;
    
    MakePort();
    
    if (IsOpen()) {
	MakePort()->Top();
	return;
    }
    if (fp) {
	fatherport= (WindowPort*) (fp->GetWindow()->portDesc);    
	p= fp->GetPortPoint(p);
    }

    GrSetPort(portDesc);

    if (vop)
	vop->SetContainer(this);
    Rectangle r(p, Max(GetMinSize().Extent(), GetExtent()));
    contentRect.extent= gPoint_1;
    portDesc->Show(fatherport, r);

    GrSetPort(fatherport);
}

Command *BlankWin::DoLeftButtonDownCommand(Point, Token t, int)
{
    MakePort()->Top();
    if (t.Flags & eFlgCntlKey)
	return GetStretcher();
    return GetMover();
}

Rectangle BlankWin::ScreenRect()
{
    return gScreenRect - GetRect().origin;
}

Rectangle BlankWin::GetRect()
{
    return MakePort()->GetRect();
}

Command *BlankWin::GetMover()
{
    Command *wm= new VObjectMover(this, ScreenRect(), gPoint1, eCrsMoveHand, 4);
    wm->SetFlag(eCmdFullScreen);
    wm->ResetFlag(eCmdCanUndo);
    return wm;
}

Command *BlankWin::GetStretcher()
{
    if (TestFlag(eBWinFixed))
	return gNoChanges;
    return new BlankWindowStretcher(this, ScreenRect());
}

void BlankWin::PushBackEvent(Token t)
{
    MakePort()->PushEvent(t);
}

void BlankWin::Top()
{
    MakePort()->Top();
}

void BlankWin::Bottom()
{
    MakePort()->Bottom();
}

void BlankWin::Bell(long d)
{
    MakePort()->Bell(d);
}

void BlankWin::SetMousePos(Point p)
{
    MakePort()->SetMousePos(p);
}

void BlankWin::Grab(bool g, bool fs)
{
    MakePort()->Grab(g, fs);
}

class Menu *BlankWin::GetMenu()
{
    return 0;
}

void BlankWin::DoCreateMenu(Menu *mp)
{
    mp->AppendItems(
	"collapse", cCOLLAPSE,
	"top",      cTOP,
	"bottom",   cBOTTOM,
	"redisplay    ",cREDISPLAY,
	0
    );
}

void BlankWin::DoSetupMenu(Menu *mp)
{
    if (TestFlag(eBWinBlock)) {
	mp->EnableItem(cREDISPLAY);
	return;
    }
    mp->EnableItems(cTOP, cREDISPLAY, cBOTTOM, cCOLLAPSE, 0);
    Clipper::DoSetupMenu(mp);
}

Command *BlankWin::DoMenuCommand(int cmd)
{
    switch (cmd) {
    case cTOP:
	MakePort()->Top();
	break;
	
    case cBOTTOM:
	MakePort()->Bottom();
	break;
	
    case cREDISPLAY:
	ForceRedraw();
	break;
	
    default:
	return Clipper::DoMenuCommand(cmd);
    }
    return gNoChanges;
}

//---- BlankWindowStretcher ---------------------------------------------------------

BlankWindowStretcher::BlankWindowStretcher(BlankWin *w, Rectangle r)
						    : VObjectStretcher(w, r)
{ 
    SetFlag(eCmdFullScreen);
    ResetFlag(eCmdCanUndo);
    ddd= 0;
}

void BlankWindowStretcher::TrackConstrain(Point ap, Point pp, Point *np)
{
    *np-= ddd;
    VObjectStretcher::TrackConstrain(ap, pp, np);
}

Command *BlankWindowStretcher::TrackMouse(TrackPhase atp, Point ap, Point pp, Point np)
{
    if (atp == eTrackPress) {
	VObjectStretcher::TrackMouse(atp, ap, pp, np);
	corner= Rectangle(oldRect.extent).PointToCorner(ap);
	Point ppp= Rectangle(oldRect.extent).CornerToPoint(corner);
	((BlankWin*)vop)->SetMousePos(ppp);
	// caution: because of SetMouse all following coordinates
	//          are translated by ddd
	ddd= ppp - ap;
	return this;
    }
    return VObjectStretcher::TrackMouse(atp, ap, pp, np);
}
