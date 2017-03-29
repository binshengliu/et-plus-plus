//$View$

#include "View.h"
#include "Error.h"
#include "Document.h"
#include "Menu.h"
#include "CmdNo.h"
#include "ClipBoard.h"
#include "PrintDialog.h"
#include "Alert_e.h"
#include "Clipper.h"
#include "OrdColl.h"
#include "String.h"

//---- View --------------------------------------------------------------------

AbstractMetaImpl(View, (TP(clippers), TP(nexthandler), TP(focus), 0));

View::View(EvtHandler *eh, Rectangle itsExtent, int id) : VObject(0, itsExtent, id)
{
    clippers= 0;
    ResetFlag(eVObjOpen|eVObjHFixed|eVObjVFixed);
    nexthandler= eh;
}

View::~View()
{
    if (clippers) {
	clippers->ForEach(Clipper,RemoveView)(this);
	SafeDelete(clippers);
    }
}
 
Document *View::GetDocument()
{
    return (Document*) FindNextHandlerOfClass(Meta(Document));
}

void View::CheckOpen()
{
    ResetFlag(eVObjOpen);
    if (GetView() && GetView()->IsOpen()) {
	SetFlag(eVObjOpen);
	return;
    }
    if (clippers) {
	Iter next(clippers);
	VObject *fp;
    
	while (fp= (VObject*) next()) {
	    if (fp->IsOpen()) {
		SetFlag(eVObjOpen);
		return;
	    }
	}
    }
}

void View::SetExtent(Point newExtent)
{
    Point oldExtent= contentRect.extent;
    VObject::SetExtent(newExtent);
    if (clippers) {
	clippers->ForEach(Clipper,ViewSizeChanged)(oldExtent, newExtent);
    }
}

void View::AddToClipper(class Clipper *clipper)
{
    if (clippers == 0) {
	clippers= new OrdCollection;
	SetContainer(clipper);
    }
    clippers->RemovePtr(clipper);   // no duplicates
    clippers->Add(clipper);
    CheckOpen();
    if (IsOpen())
	Update();  // hack
}

void View::RemoveFromClipper(Clipper* clipper)
{
    if (clippers) {
	clippers->RemovePtr(clipper);
	if (clippers->Size() <= 0) {
	    SetContainer(0);
	    SafeDelete(clippers);
	}
    }
    CheckOpen();
}

//---- drawing

void View::DrawAll(Rectangle r, bool)
{
    if (!gPrinting)
	Update();
    Draw(r);
    if (!TestFlag(eViewNoPrint)) {
	if (gPrintManager == 0)
	    gPrintManager= new PrintDialog;
	gPrintManager->ShowPageGrid(r, this);
    }
}

void View::InvalidateRect(Rectangle r)
{
    if (clippers) {
	clippers->ForEach(VObject,InvalidateViewRect)(r);
    } else if (GetContainer())
	GetContainer()->InvalidateRect(r);
}

void View::ShowInAllClippers(VoidObjMemberFunc of, Object *op, void *v1, void *v2, void *v3, void *v4)
{
    if (clippers) { 
	clippers->ForEach(Clipper,DrawInFocus)(of, op, v1, v2, v3, v4);
    } else if (GetView()) {
	GetView()->ShowInAllClippers(of, op, v1, v2, v3, v4);
    }
}

void View::Update()
{
}

Rectangle View::GetViewedRect()
{
    if (focus)
	return focus->GetViewedRect();
    if (GetView()) {
	return GetView()->GetViewedRect();
    }
    return gRect0;
}

//---- scrolling

void View::ConstrainScroll(Point*)
{
}

void View::RevealRect(Rectangle revealRect, Point minToSee)
{
    if (focus)
	focus->RevealRect(revealRect, minToSee);
    else if (clippers) {
	clippers->ForEach(Clipper,RevealRect)(revealRect, minToSee);
    } else if (GetView()) {
	GetView()->RevealRect(revealRect, minToSee);
    }
}

void View::RevealAlign(Rectangle revealRect, VObjAlign al)
{
    if (focus)
	focus->RevealAlign(revealRect, al);
    else if (clippers) {
	clippers->ForEach(Clipper,RevealAlign)(revealRect, al);
    } else if (GetView()) {
	GetView()->RevealAlign(revealRect, al);
    }
}

void View::Scroll(int mode, Point scroll, bool redraw)
{
    if (focus)
	focus->Scroll(mode, scroll, redraw);
    else if (clippers) {
	clippers->ForEach(Clipper,Scroll)(mode, scroll, redraw);
    } else if (GetView()) {
	GetView()->Scroll(mode, scroll, redraw);
    }
}

//---- event handling

EvtHandler *View::GetNextHandler()
{
    return nexthandler;
}

void View::SetNextHandler(EvtHandler *eh)
{
    nexthandler= eh;
}

Command *View::Input(Point lp, Token t, Clipper *vf)
{
    focus= vf;
    Command *cmd= DispatchEvents(lp, t, vf);
    focus= 0;
    return cmd;
}

Command *View::DoCursorKeyCommand(EvtCursorDir d, Point p, Token t)
{
    if (focus)
	focus->Scroll(cPartScrollStep, t.CursorPoint());
    return VObject::DoCursorKeyCommand(d, p, t);
}

Command *View::DoFunctionKeyCommand(int code, Point p, Token t)
{
    if (focus) {
	switch (t.FunctionCode()) {
	case 28:    // end
	    focus->Scroll(cPartScrollAbs, Point(0, 30000));
	    break;
	case 22:    // home
	    focus->Scroll(cPartScrollAbs, gPoint0);
	    break;
	case 24:    // page up
	    focus->Scroll(cPartScrollPage, Point(0, -1));
	    break;
	case 30:    // page down
	    focus->Scroll(cPartScrollPage, Point(0, 1));
	    break;
	}
    }
    return VObject::DoFunctionKeyCommand(code, p, t);
}

//---- clipboard

bool View::CanPaste(char*)
{
    return FALSE;
}

void View::SelectionToClipboard(char*, ostream&)
{
}

Command *View::PasteData(char*, istream&)
{
    return gNoChanges;
}

bool View::HasSelection()
{
    return FALSE;
}

//---- menus

Command *View::DoMenuCommand(int cmd)
{
    Command *cm;
    
    switch (cmd) {

    case cCOPY:
    case cCUT:
	gClipBoard->SelectionToClipboard(this);
	return gNoChanges;

    case cPASTE:
	cm= gClipBoard->PasteClipboard(this);
	if (cm == gNoChanges)
	    ShowAlert(eAlertCaution, "Can't paste clipboard (%s)",
						gClipBoard->GetType());
	return cm;

    case cPRINT:
	Print();
	return gNoChanges;

    default:
	break;
    }
    return EvtHandler::DoMenuCommand(cmd);
}

void View::DoSetupMenu(Menu *menu)
{
    EvtHandler::DoSetupMenu(menu);
    menu->EnableItem(cPRINT);

    if (gClipBoard->CanPaste(this))
	menu->EnableItem(cPASTE);
    if (HasSelection())
	menu->EnableItems(cCUT, cCOPY, 0);        
}

void View::DoCreateMenu(Menu *m)
{
    VObject::DoCreateMenu(m);
    Menu *fileMenu= m->FindMenuItem(cFILEMENU);
    if (fileMenu) 
	fileMenu->InsertItemsBefore(cSHOWAPPLWIN,
		"print ...",        cPRINT,
		"-",
		0);
		
    if (!m->FindItem(cCUT)) 
	m->AppendItems(
	    "cut",         cCUT,
	    "copy",        cCOPY,
	    "paste",       cPASTE,
	0);
}

//---- inspector

void View::InspectorId(char *buf, int sz)
{
    VObject *container= 0, *vop= this;
    while (vop= vop->GetContainer())
	container= vop;
    if (container) {
	container->InspectorId(buf, sz-6);
	strn0cpy(buf, form("in %s", buf), sz);
    } else
	VObject::InspectorId(buf, sz);
}
