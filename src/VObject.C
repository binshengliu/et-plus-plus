//$VObject,VObjectMover,VObjectStretcher,CompositeVObject,VObjectCommand$

#include "VObject.h" 
#include "Menu.h"
#include "BlankWin.h"
#include "WindowSystem.h"
#include "String.h"
#include "OrdColl.h"

//---- VObject -----------------------------------------------------------------

AbstractMetaImpl(VObject, (TP(container), T(id), T(contentRect), 0));

VObject::VObject(EvtHandler *next, Rectangle r, int id)
{
    Init(id, r, (View*) next);
}

VObject::VObject(Rectangle r, int id)
{
    Init(id, r, 0);
}

VObject::VObject(int id)
{
    Init(id, gRect0, 0);
}

void VObject::Init(int i, Rectangle r, View *v)
{
    VObject::SetContainer(v);
    id= i;
    contentRect= r;
    SetFlag(eVObjDefault+eVObjOpen);
}

char *VObject::AsString()
{
    return "~";
}

int VObject::Compare(ObjPtr op)
{
    return StrCmp((byte*)AsString(), (byte*)op->AsString(), -1, sortmap);
}

bool VObject::IsEqual(ObjPtr op)
{
    return StrCmp((byte*)AsString(), (byte*)op->AsString(), -1, sortmap) == 0;
}

//---- state -------------------------------------------------------------------

void VObject::Open(bool mode)
{
    SetFlag(eVObjOpen, mode);
}

void VObject::Enable(bool b, bool redraw)
{
    SetFlag(eVObjEnabled, b);
    if (redraw)
	ForceRedraw();
}

//---- container ---------------------------------------------------------------

void VObject::SetContainer(VObject *v)
{
    container= v;
}

VObject *VObject::FindContainerOfClass(Class *cla)
{
    register VObject *vp;
    
    if (cla == Meta(BlankWin))
	vp= this;
    else
	vp= GetContainer();
    
    for (; vp; vp= vp->GetContainer()) {
	if (vp->IsA()->isKindOf(cla))
		return vp;
    }
    return 0;
}

Point VObject::ContainerPoint(Point p)
{
    return p;
}

Point VObject::GetPortPoint(Point p)
{
    p= ContainerPoint(p);
    if (GetContainer())
	return GetContainer()->GetPortPoint(p);
    return p;
}

BlankWin *VObject::GetWindow()
{
    return (BlankWin*) FindContainerOfClass(Meta(BlankWin));
}

View *VObject::GetView()
{
    return (View*) FindContainerOfClass(Meta(View));
}

Clipper *VObject::Getclipper()
{
    return (Clipper*) FindContainerOfClass(Meta(Clipper));
}

void VObject::AddToClipper(class Clipper *clipper)
{
    VObject::SetContainer(clipper);
    CalcExtent();
    SetOrigin(gPoint0);
}

void VObject::RemoveFromClipper(class Clipper *clipper)
{
    if (GetContainer() == clipper)
	SetContainer(0);
}

Rectangle VObject::GetViewedRect()
{
    return contentRect;
}

void VObject::SetFocus(Rectangle r, Point o)
{
    if (GetContainer() && r.Clip(contentRect))
	GetContainer()->SetFocus(r, o);
}

//---- tree walking ------------------------------------------------------------

VObject *VObject::Detect(BoolFun find, void *arg)
{
    if (find(this, this, arg))
	return this;
    return 0;
}

static bool Comp1(Object*, Object *op, void *v)
{
    return (bool) (((VObject*)(op))->GetId() == (int)v);
}

static bool Comp2(Object*, Object *op, void *v)
{
    VObject *vop= (VObject*) op;
    Point p= *((Point*)v);
    Point offset= vop->ContainerPoint(gPoint0);
    return (bool) vop->ContainsPoint(p-offset);
}

static bool Comp3(Object*, Object *op, void *v)
{
    return op->IsEqual((VObject*)v);
}

static bool Comp4(Object*, Object *op, void *v)
{
    return op == (VObject*)v;
}

VObject *VObject::FindItem(int id)
{
    if (id != cIdNone)
	return Detect(Comp1, (void*) id);
    return 0;
}

VObject *VObject::FindItem(Point p)
{
    return Detect(Comp2, &p);
}

VObject *VObject::FindItem(VObject *g)
{
    return Detect(Comp3, g);
}

VObject *VObject::FindItemPtr(VObject *g)
{
    return Detect(Comp4, g);
}

//---- layout ------------------------------------------------------------------

Metric VObject::GetMinSize()
{
    return Metric(contentRect.extent);
}

void VObject::CalcExtent()
{
    SetExtent(GetMinSize().Extent());
}

int VObject::Base()
{ 
    return contentRect.extent.y;
}

void VObject::SetOrigin(Point origin)
{
    if (contentRect.origin != origin) {
	contentRect.origin= origin;
	if (TestFlag(eVObjLayoutCntl)) 
	    Control(GetId(), cPartOriginChanged, this);
	Object::Send(GetId(), cPartOriginChanged, &origin);
    }
}

void VObject::SetExtent(Point extent)
{
    if (contentRect.extent != extent) {
	contentRect.extent= extent;
	if (TestFlag(eVObjLayoutCntl)) 
	    Control(GetId(), cPartExtentChanged, this);
	Object::Send(GetId(), cPartExtentChanged, &extent);
    }
}

void VObject::SetContentRect(Rectangle r, bool redraw)
{
    if (!redraw && contentRect == r)
	return;
    if (redraw)
	ForceRedraw();
    if (contentRect.extent != r.extent)
	SetExtent(r.extent);
    SetOrigin(r.origin);
    if (redraw)
	ForceRedraw();
}

void VObject::Align(Point at, Metric m, VObjAlign ga)
{
    switch (ga & eVObjH) {
    case eVObjHLeft:
	break;
    case eVObjHCenter:
	at.x+= (m.Width() - Width())/2;
	break;
    case eVObjHRight:
	at.x+= m.Width() - Width();
	break;
    }
    switch (ga & eVObjV) {
    case eVObjVBase:
	at.y+= m.Base() - Base();
	break;
    case eVObjVCenter:
	at.y+= (m.Height() - Height())/2;
	break;
    case eVObjVBottom:
	at.y+= m.Height() - Height();
	break;
    }
    SetOrigin(at);
}

void VObject::Move(Point delta, bool redraw)
{
    if (delta == gPoint0)
	return;
    if (redraw)
	ForceRedraw();
    SetOrigin(GetOrigin()+delta);
    if (redraw) {
	ForceRedraw();
	Changed();
    }
}

//---- resizing ----------------------------------------------------------------

Command *VObject::GetMover()
{
    VObject *v= GetView();
    if (v)
	return new VObjectMover(this, v->contentRect);
    return new VObjectMover(this);
}

Command *VObject::GetStretcher()
{
    VObject *v= GetView();
    if (v)
	return new VObjectStretcher(this, v->contentRect);
    return new VObjectStretcher(this);
}

bool VObject::ContainsPoint(Point p)
{
    return contentRect.ContainsPoint(p);
}

//---- drawing -----------------------------------------------------------------

static VObject *aetsch= 0;

void VObject::DrawAll(Rectangle r, bool highlight)
{
    if (IsOpen() && r.Clip(contentRect))
	DrawInner(r, (bool) (highlight || aetsch == this));
}

void VObject::DrawInner(Rectangle r, bool highlight)
{
    if (!GrHasColor())
	Draw(r);
    if (highlight && ! gPrinting) {
	GrSetPattern(gHighlightColor);
	GrSetPenPattern(gHighlightColor);
	DrawHighlight(r);
	GrSetPattern(ePatBlack);
	GrSetPenNormal();
    }
    if (GrHasColor())
	Draw(r);
}

void VObject::Draw(Rectangle r)
{
}

void VObject::DrawHighlight(Rectangle r)
{
    GrFillRect(r);
}

void VObject::Highlight(HighlightState hst)
{
    if (hst)
	aetsch= this;
    ForceRedraw();
    UpdateEvent();
    aetsch= 0;
}

void VObject::Outline2(Point p1, Point p2)
{
    GrStrokeRect(NormRect(p1, p2));
}

void VObject::OutlineRect(Rectangle r)
{
    Outline2(r.NW(), r.SE());
}

void VObject::Outline(Point delta)
{
    OutlineRect(contentRect+delta);
}

//---- invalidation and focusing -----------------------------------------------

void VObject::InvalidateRect(Rectangle r)
{
    if (r.Clip(contentRect) && IsOpen() && GetContainer())
	GetContainer()->InvalidateViewRect(r);
}

void VObject::InvalidateViewRect(Rectangle r)
{
    r.origin= ContainerPoint(r.origin);
    InvalidateRect(r);
}

void VObject::ForceRedraw()
{ 
    InvalidateRect(contentRect); 
}

void VObject::UpdateEvent()
{
    if (IsOpen() && GetContainer())
	GetContainer()->UpdateEvent();
}

GrCursor VObject::GetCursor(Point lp)
{
    if (GetContainer())
	return GetContainer()->GetCursor(ContainerPoint(lp));
    return eCrsBoldArrow;
}

//---- input/output ------------------------------------------------------------

ostream& VObject::PrintOn(ostream &s)
{
    EvtHandler::PrintOn(s);
    return s << id SP << contentRect SP;
}

istream& VObject::ReadFrom(istream &s)
{
    EvtHandler::ReadFrom(s);
    return s >> id >> contentRect;
}

//---- input handling ----------------------------------------------------------

EvtHandler *VObject::GetNextHandler()
{
    return GetContainer();
}

void VObject::DoOnItem(int, VObject*, Point)
{
}

Command *VObject::Input(Point lp, Token t, Clipper *vf)
{
    if (ContainsPoint(lp) /* && IsOpen() */) {
	if (Enabled())
	    return DispatchEvents(lp, t, vf);
	return gNoChanges;
    }
    return 0;     
}

Command *VObject::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    extern int Clicks;
    register Command *currCmd= gNoChanges;
    
    if (t.IsMouseButton() || t.Code == eEvtEnter || t.Code == eEvtExit 
		    || t.Code == eEvtLocMove || t.Code == eEvtLocStill)
	GrSetCursor(GetCursor(lp));

    switch (t.Code) {
    case eEvtRightButton:
	if (!(t.Flags & eFlgButDown))
	    currCmd= DoRightButtonDownCommand(lp, t, Clicks, vf);
	break;
    case eEvtMiddleButton:
    case eEvtLeftButton:
	if (!(t.Flags & eFlgButDown)) {
	    if (t.Code == eEvtLeftButton) {
		if (t.Flags == (eFlgShiftKey|eFlgCntlKey|eFlgMetaKey))
		    Inspect();
		else
		    currCmd= DoLeftButtonDownCommand(lp, t, Clicks);
	    } else
		currCmd= DoMiddleButtonDownCommand(lp, t, Clicks);
	    if (currCmd && currCmd != gNoChanges && vf)
		currCmd= vf->TrackInContent(lp, t, currCmd);
	}
	break;
    default:
	if (t.IsKey())
	    currCmd= DoKeyCommand(t.Code, lp, t);
	else if (t.IsFunctionKey()) 
	    currCmd= DoFunctionKeyCommand(t.FunctionCode(), lp, t);
	else if (t.IsCursorKey())
	    currCmd= DoCursorKeyCommand(t.CursorDir(), lp, t);
	else if (t.Code == eEvtIdle)
	    currCmd= DoIdleCommand();
	else
	    currCmd= DoOtherEventCommand(lp, t);
	break;
    }
    return currCmd;
}

Command *VObject::DoKeyCommand(int ch, Point lp, Token t)
{
    if (GetContainer())
	return GetContainer()->DoKeyCommand(ch, ContainerPoint(lp), t);
    return gNoChanges;
}

Command *VObject::DoCursorKeyCommand(EvtCursorDir d, Point p, Token t)
{
    if (GetContainer())
	return GetContainer()->DoCursorKeyCommand(d, ContainerPoint(p), t);
    return gNoChanges;
}

Command *VObject::DoFunctionKeyCommand(int pfk, Point lp, Token t)
{
    int code= -1;
    
    switch (t.FunctionCode()) {
    case 3:
	code= cUNDO;
	break;
    case 5:
	code= cCOPY;
	break;
    case 4:
	code= cTOP;
	break;
    case 7:
	code= cPASTE;
	break;
    case 9:
	code= cCUT;
	break;
    case 8:
	code= cFIND;
	break;
    case 6:
	code= cOPEN;
	break;
    }
    if (code > 0)
	return DoMenuCommand(code);
    if (GetContainer())
	return GetContainer()->DoFunctionKeyCommand(pfk, ContainerPoint(lp), t);
    return gNoChanges;
}

Command *VObject::DoMiddleButtonDownCommand(Point p, Token t, int clicks)
{
    if (GetContainer())
	return GetContainer()->DoMiddleButtonDownCommand(ContainerPoint(p), t, clicks);
    return gNoChanges;
}

Command *VObject::DoLeftButtonDownCommand(Point p, Token t, int clicks)
{
    if (GetContainer())
	return GetContainer()->DoLeftButtonDownCommand(ContainerPoint(p), t, clicks);
    return gNoChanges;
}

Command *VObject::DoOtherEventCommand(Point p, Token t)
{
    if (GetContainer())
	return GetContainer()->DoOtherEventCommand(ContainerPoint(p), t);
    return gNoChanges;
}

Command *VObject::DoRightButtonDownCommand(Point lp, Token, int, Clipper *vf)
{
    Menu *menu;

    if (vf && (menu= GetMenu())) {
	int cmdno;

	if (menu->IsNew())
	    DoCreateMenu(menu);                     // append menu items
	menu->DisableAll();
	DoSetupMenu(menu);                          // enable some menu items
	if ((cmdno= menu->Show(lp, vf)) >= 0) {     // show menu in window
	    vf->Focus();
	    return DoMenuCommand(cmdno);
	}
    }
    return gNoChanges;
}

Command *VObject::TrackInContent(Point, Token, Command *currCmd)
{
    return currCmd;
}

//---- misc --------------------------------------------------------------------

void VObject::Print()
{
    extern int ShowPrintDialog(VObject*);
    
    ShowPrintDialog(this);
}

//---- VObjectCommand ----------------------------------------------------------

VObjectCommand::VObjectCommand(VObject *g) : Command(cIdNone)
{
    vop= g;
    SetFlag(eCmdNoReplFeedback);
}

void VObjectCommand::Init(VObject *g, Rectangle cr, Point gr, GrCursor cd, int hy)
{
    vop= g;
    constrainRect= cr;
    grid= gr;
    newcursor= cd;
    firstmove= FALSE;
    hysterese= hy;
    oldRect= newRect= vop->ContentRect();  
}

Command *VObjectCommand::TrackMouse(TrackPhase atp, Point ap, Point, Point np)
{
    switch (atp) {
    case eTrackPress:
	oldcursor= GrGetCursor();
	break;

    case eTrackRelease:
	GrSetCursor(oldcursor);
	
	if (firstmove) {
	    if (vop->GetView() && !TestFlag(eCmdFullScreen))     
		vop->SetContentRect(oldRect, FALSE);

	    if (TestFlag(eCmdCanUndo))
		break;
	    DoIt();
	}
	return gNoChanges;

    case eTrackMove:
	delta= np - ap;
	if (abs(delta.x) > hysterese || abs(delta.y) > hysterese) {
	    firstmove= TRUE;
	    GrSetCursor(newcursor);
	}
	break;
    }
    return this;
}

void VObjectCommand::TrackConstrain(Point ap, Point, Point *np)
{
    Rectangle r= oldRect;

    r.origin+= *np-ap;
    *np+= r.AmountToTranslateWithin(constrainRect);
    if (grid >= 1)
	*np= (*np/grid) * grid;     // align to grid
}

void VObjectCommand::TrackFeedback(Point, Point, bool on)
{
    if (firstmove) {
	if (port->penink != ePatXor) {
	    if (on)
		vop->SetContentRect(newRect, TRUE);
	} else
	    vop->Outline2(newRect.NW(), newRect.SE());
    }
}

void VObjectCommand::DoIt()
{
    vop->SetContentRect(newRect, !TestFlag(eCmdFullScreen));
}

void VObjectCommand::UndoIt()
{
    vop->SetContentRect(oldRect, !TestFlag(eCmdFullScreen));
}

//---- VObjectMover ------------------------------------------------------------

VObjectMover::VObjectMover(VObject *g) : VObjectCommand(g)
{
    Init(g, gRect0, gPoint1, eCrsMoveHand, 2);
}

VObjectMover::VObjectMover(VObject *g, Rectangle cr) : VObjectCommand(g)
{
    Init(g, cr, gPoint1, eCrsMoveHand, 2);
}

VObjectMover::VObjectMover(VObject *g, Rectangle cr, Point gr, GrCursor cd,
						    int hy) : VObjectCommand(g)
{
    Init(g, cr, gr, cd, hy);
}

void VObjectMover::Init(VObject *g, Rectangle cr, Point gr, GrCursor cd, int hy)
{
    SetName("move");
    VObjectCommand::Init(g, cr, gr, cd, hy);
}

Command *VObjectMover::TrackMouse(TrackPhase atp, Point ap, Point pp, Point np)
{
    Command *cmd= VObjectCommand::TrackMouse(atp, ap, pp, np);
    switch (atp) {
    case eTrackMove:
	newRect= oldRect;
	newRect.origin+= delta;
	return this;
    case eTrackRelease:
	if (newRect == oldRect)
	    cmd= gNoChanges;
	break;
    }
    return cmd;
}

//---- VObjectStretcher --------------------------------------------------------

VObjectStretcher::VObjectStretcher(VObject *g) : VObjectCommand(g)
{
    Init(g, gRect0, gPoint1, eCrsMoveStretch, 2, g->GetMinSize());
}

VObjectStretcher::VObjectStretcher(VObject *g, Rectangle cr) : VObjectCommand(g)
{
    Init(g, cr, gPoint1, eCrsMoveStretch, 2, g->GetMinSize());
}

VObjectStretcher::VObjectStretcher(VObject *g, Rectangle cr, Point ms) : VObjectCommand(g)
{
    Init(g, cr, ms, eCrsMoveStretch, 2, g->GetMinSize());
}

VObjectStretcher::VObjectStretcher(VObject *g, Rectangle cr, Point ms, Point gr,
				    GrCursor cd, int hy) : VObjectCommand(g)
{
    Init(g, cr,  gr, cd, hy, ms);
}

void VObjectStretcher::Init(VObject *g, Rectangle cr, Point gr,
						GrCursor cd, int hy, Point ms)
{
    SetName("resize");
    VObjectCommand::Init(g, cr, gr, cd, hy);
    minSize= ms;
}

void VObjectStretcher::TrackConstrain(Point ap, Point, Point *np)
{  
    switch (corner) {
    case 0: case 4:
	np->x= ap.x;
	break;
    case 2: case 6:
	np->y= ap.y;
	break;
    }
}

Command *VObjectStretcher::TrackMouse(TrackPhase atp, Point ap, Point pp, Point np)
{
    Point p1, p2;

    switch (atp) {
    case eTrackPress:
	VObjectCommand::TrackMouse(atp, ap, pp, np);
	corner= oldRect.PointToCorner(ap);
	break;

    case eTrackMove:
	VObjectCommand::TrackMouse(atp, ap, pp, np);
	p1= oldRect.NW();
	p2= oldRect.SE();

	switch (corner) {
	case 0: case 6: case 7:
	    p1+= delta;
	    break;
	case 1:
	    p2.x+= delta.x;
	    p1.y+= delta.y;
	    break;
	case 5:
	    p1.x+= delta.x;
	    p2.y+= delta.y;
	    break;
	default: // case 2: case 3: case 4:
	    p2+= delta;
	    break;
	}
	newRect= NormRect(p1, p2);
	newRect.extent= Max(newRect.extent, minSize);
	break;

    case eTrackRelease:
	return VObjectCommand::TrackMouse(atp, ap, pp, np);
    }
    return this;
}

//---- CompositeVObject --------------------------------------------------------------

AbstractMetaImpl(CompositeVObject, (TB(modified), TP(list), 0));

CompositeVObject::CompositeVObject(int id, Collection *cp) : VObject(id)
{
    list= cp;
    if (list == 0)
	list= new OrdCollection;
    list->ForEach(VObject,SetContainer)(this);
    modified= TRUE;
}

CompositeVObject::CompositeVObject(int va_(id), ...) : VObject(va_(id))
{
    va_list ap;
    va_start(ap,va_(id));
    list= new OrdCollection;
    SetItems(ap);
    modified= TRUE;
    va_end(ap);
}

CompositeVObject::CompositeVObject(int id, va_list ap) : VObject(id)
{
    list= new OrdCollection;
    SetItems(ap);
    modified= TRUE;
}

CompositeVObject::~CompositeVObject()
{
    SafeDelete(list);
}

void CompositeVObject::FreeAll()
{
    if (list) {
	list->FreeAll();
	SafeDelete(list);
    }    
}

Collection *CompositeVObject::GetList()
{
    return list;
}

int CompositeVObject::Size()
{
    if (list) 
	return list->Size();
    return 0;
}

void CompositeVObject::Add(VObject *vop)
{
    if (vop) {
	if (list == 0)
	    list= new OrdCollection;
	list->Add(vop);
	vop->SetContainer(this);
	SetModified();
    }
}

VObject *CompositeVObject::Remove(VObject* vop)
{
    if (list) {
	SetModified();
	return (VObject*) list->Remove(vop);
    }
    return 0;
}

VObject *CompositeVObject::SetAt(int at, VObject *vop)
{
    VObject *old= 0;
    
    if (vop) {
	if (list->Size() > 0)
	    old= (VObject*) list->RemovePtr(At(at));
	((OrdCollection*)list)->AddAt(at, vop);
	vop->SetContainer(this);
	SetExtent(GetExtent());
	SetOrigin(GetOrigin());
    }
    return old;
}

Iterator *CompositeVObject::MakeIterator()
{
    if (list)
	return list->MakeIterator();
    return 0;
}

void CompositeVObject::Open(bool mode)
{
    VObject::Open(mode);
    if (list) {
	list->ForEach(CompositeVObject,Open)(mode);
    }
}

void CompositeVObject::DoObserve(int, int, void*, Object *op)
{
    if (op == list)
	SetModified();
}

void CompositeVObject::SendDown(int id, int part, void *val)
{
    list->ForEach(VObject,SendDown)(id, part, val);
}

void CompositeVObject::SetItems(va_list ap)
{
    list->AddVector(ap);
    {
    list->ForEach(VObject,SetContainer)(this);
    }
}

void CompositeVObject::SetContainer(VObject *v)
{
    VObject::SetContainer(v);
    list->ForEach(VObject,SetContainer)(this);
}

void CompositeVObject::Enable(bool b, bool redraw)
{
    VObject::Enable(b);
    list->ForEach(VObject,Enable)(b, redraw);
}

void CompositeVObject::Draw(Rectangle r)
{
    list->ForEach(VObject,DrawAll)(r, FALSE);
}

void CompositeVObject::Outline2(Point p1, Point p2)
{
    list->ForEach(VObject,Outline2)(p1, p2);
}

VObject *CompositeVObject::Detect(BoolFun find, void *arg)
{
    VObject *g1, *g2;
    Iter next(list);

    while (g1= (VObject*) next())
	if (g2= g1->Detect(find, arg))
	    return g2;
    if (VObject::Detect(find, arg))
	return (VObject*) this;
    return 0;
}

Command *CompositeVObject::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    Command *cmd;
    RevIter next((SeqCollection*)list);
    VObject *dip;

    while (dip= (VObject*) next()) 
	if (cmd= dip->Input(lp, t, vf))
	    return cmd;
    return VObject::DispatchEvents(lp, t, vf);
}

void CompositeVObject::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    list->ForEach(VObject,SetOrigin)(at);
}

void CompositeVObject::SetExtent(Point e)
{
    VObject::SetExtent(e);
    list->ForEach(VObject,CalcExtent)();
}

Metric CompositeVObject::GetMinSize()
{
    Metric m;
    Iter next(list);
    VObject *dip;

    while (dip= (VObject*) next())
	m.Merge(dip->GetMinSize());
    return m;
}

int CompositeVObject::Base()
{
    return GetMinSize().base;
}

ostream& CompositeVObject::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << list SP;
}

istream& CompositeVObject::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    s >> list;
    SetModified();
    return s;
}

void CompositeVObject::Parts(Collection* col)
{
    VObject::Parts(col);
    col->Add(list);
}

void CompositeVObject::InspectorId(char *buf, int bufSize)
{
    VObject::InspectorId(buf, bufSize);
}

