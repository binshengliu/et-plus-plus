//$DialogView,Dialog$
#include "Dialog.h"
#include "Buttons.h"
#include "OrdColl.h"
#include "Menu.h"
#include "Error.h"
#include "CmdNo.h"
#include "Window.h"

//---- DialogView -------------------------------------------------------------

AbstractMetaImpl(DialogView, (TP(dialogRoot), TP(kbdFocusItems),
	    TP(kbdFocus), TP(defaultButton), TB(modified), TP(menu), 0));

DialogView::DialogView(EvtHandler *eh) : View(eh, Point(3000))
{
    dialogRoot= 0;
    modified= TRUE;
    kbdFocus= 0;
    kbdFocusItems= 0;
    defaultButton= 0;
    menu= 0;
    SetFlag(eViewNoPrint);
}

DialogView::~DialogView()
{
    if (dialogRoot) {
	dialogRoot->FreeAll();
	SafeDelete(dialogRoot);
    }
    SafeDelete(kbdFocusItems);
}

void DialogView::Open(bool mode)
{
    View::Open(mode);
    if (mode)
	Update();
    if (dialogRoot)
	dialogRoot->Open(mode);
}

void DialogView::SetContainer(VObject *c)
{
    View::SetContainer(c);
    if (dialogRoot)
	dialogRoot->SetContainer(this);
}

VObject *DialogView::DoCreateDialog()
{
    return 0;
}

Metric DialogView::GetMinSize()
{
    Update();
    return dialogRoot->GetMinSize();
}

VObject *DialogView::SetDialog(VObject *dia, bool redraw)
{
    VObject *olddialog= 0;
    if (dia) {
	olddialog= dialogRoot;
	dialogRoot= dia;
	dialogRoot->SetContainer(this);
	dialogRoot->Enable();
	dialogRoot->SendDown(cIdEndKbdFocus, cPartFocusChanged, (void*)TRUE);
	if (kbdFocus)
	    kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, (void*)TRUE);
	CalcLayout(redraw);
    }
    return olddialog;
}

void DialogView::SetOrigin(Point at)
{
    View::SetOrigin(at);
    if (dialogRoot) 
	dialogRoot->SetOrigin(at);
}

void DialogView::SetExtent(Point e)
{
    View::SetExtent(e);
    if (dialogRoot) 
	dialogRoot->SetExtent(e);
}

void DialogView::CalcLayout(bool redraw)
{
    if (dialogRoot) {
	dialogRoot->CalcExtent();
	SetFlag(eVObjHFixed, dialogRoot->TestFlag(eVObjHFixed));
	SetFlag(eVObjVFixed, dialogRoot->TestFlag(eVObjVFixed));
	dialogRoot->SetOrigin(GetOrigin());
	View::SetExtent(dialogRoot->GetExtent());
	if (redraw)
	    ForceRedraw();
    }
}

void DialogView::Update()
{
    if (modified) {
	if (dialogRoot)
	    CalcLayout();
	else
	    SetDialog(DoCreateDialog(), FALSE);

	//if (kbdFocus) 
	//    kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, (void*)TRUE);
	modified= FALSE;
    }    
}

void DialogView::Control(int id, int part, void *val)
{
    switch (part) {
 
    case cPartWantKbdFocus:
	if (kbdFocus != (VObject*) val) {
	    if (kbdFocus)
		kbdFocus->SendDown(cIdEndKbdFocus, cPartFocusChanged, 0);
	    kbdFocus= (VObject*) val;
	    //kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, 0);
	}
	break;

    case cPartLayoutChanged:
	CalcLayout();
	break;

    case cPartFocusRemove: 
	if (((VObject*)val)->FindItemPtr(kbdFocus)) 
	    SetKbdFocus(0);
	break;
	    
    default:
	break;
    }
    View::Control(id, part, val);
}

Command *DialogView::DispatchEvents(Point lp, Token t, Clipper *vf)
{    
    if (t.IsKey()) {
	if (kbdFocusItems && t.Code == '\t') {
	    DoTab(t);
	    return gNoChanges;
	}
	if (defaultButton && t.Code == '\r') {
	    defaultButton->Flush();
	    return gNoChanges;
	}
    }
    if (kbdFocus && kbdFocus->Enabled() &&
			     (t.IsCursorKey() || t.IsKey() || t.Code == eEvtRightButton))
	return kbdFocus->DispatchEvents(lp, t, vf);

    if (dialogRoot)
	return dialogRoot->Input(lp, t, vf);
    return View::DispatchEvents(lp, t, vf);
}

void DialogView::Parts(Collection *col)
{
    View::Parts(col);
    col->Add(dialogRoot);
}

//---- Menus --------------------------------------------------------------------

Menu *DialogView::GetMenu()
{
    if (GetNextHandler())
	return View::GetMenu();
    if (menu == 0)
	menu= new Menu("dialog");
    return menu;
}

bool DialogView::HasSelection()
{
    bool hasSelection;
    if (kbdFocus) {
	kbdFocus->SendDown(cIdNone, cPartHasSelection, &hasSelection);
	return hasSelection;
    }
    return View::HasSelection();
}

Command *DialogView::DoMenuCommand(int cmd)
{
    if (kbdFocus) {
	VObject *oldactive= kbdFocus;
	kbdFocus= 0;
	Command *rcmd= oldactive->DoMenuCommand(cmd);
	kbdFocus= oldactive;
	return rcmd;
    }
    return View::DoMenuCommand(cmd);
}

void DialogView::AddItemWithKbdFocus(VObject* t)
{
    if (kbdFocusItems == 0)
	kbdFocusItems= new OrdCollection();
    kbdFocusItems->Add(t);
    if (kbdFocus == 0)  // the first appended textitem becomes the active one
	kbdFocus= t;             
}

void DialogView::SetKbdFocus(VObject *vop)
{
    Control(cIdNone, cPartWantKbdFocus, vop);
    if (kbdFocus) 
	kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, 0);
}

void DialogView::SetDefaultButton (Button *b)
{
    defaultButton= b;
}


void DialogView::FindNeighbours(VObject *&left, VObject *&right, VObject *&first)
{
    VObject *vop, *last;
    Iter next(kbdFocusItems);
    first= last= left= right= 0;
    bool found= FALSE;
    
    while (vop= (VObject*)next()) {
	if (vop->Enabled() && vop->IsOpen()) {
	    if (first == 0)
		first= vop;
	    if (found && right == 0)
		right= vop;
	    if (vop == kbdFocus)
		found= TRUE; 
	    if (!found)
		left= vop;
	    last= vop;
	}
    }
    if (left == 0)
	left= last;
    if (right == 0)
	right= first;
}

void DialogView::DoTab(Token t)
{
    VObject *first, *next, *prev, *nextKbdFocus= 0;

    FindNeighbours(prev, next, first); 
    
    if (!kbdFocus || !kbdFocusItems->FindPtr(kbdFocus)) // take the first enabled item
	nextKbdFocus= first;
    else if (t.Flags & eFlgShiftKey) 
	nextKbdFocus= prev;
    else 
	nextKbdFocus= next;
    
    if (kbdFocus)   
	kbdFocus->SendDown(cIdEndKbdFocus, cPartFocusChanged, 0);
    kbdFocus= nextKbdFocus;
    if (kbdFocus) 
	kbdFocus->SendDown(cIdStartKbdFocus, cPartFocusChanged, 0);
}

VObject* DialogView::RemoveItemWithKbdFocus (VObject* t)
{
    if (kbdFocus == t)
	kbdFocus= 0;
    return (VObject*) kbdFocusItems->Remove(t);
}

void DialogView::EnableItem (int id, bool b)
{
    VObject *gop;
    if (gop= FindItem(id))
	gop->Enable(b, TRUE);
}

VObject *DialogView::FindItem(int id)
{
    return dialogRoot->FindItem(id);
}

void DialogView::Draw(Rectangle r)
{
    if (dialogRoot)
	dialogRoot->DrawAll(r, FALSE);
}

//---- Dialog -------------------------------------------------------------------

AbstractMetaImpl(Dialog, (TP(dw), T(actionId), 0));

Dialog::Dialog(char *title, int f, EvtHandler *eh) : DialogView(eh)
{
    if (f == 0)
	f= eBWinDefault;
    if (f & eBWinBlock)
	f|= eBWinFixed;
    if (title)
	dw= new Window(this, gPoint_1, (WindowFlags)f, this, title);
    else
	dw= new BlankWin(this, this, gPoint_1, (BWinFlags)f);
}

Dialog::~Dialog()
{   
    Object *op= dw;
    dw= 0;
    SafeDelete(op);
}

void Dialog::SetTitle(char *title)
{
    if (GetWindow()->IsKindOf(Window) )
	((Window*)GetWindow())->SetTitle(title, FALSE);  // FALSE..don't redraw
}

void Dialog::DoSetDefaults()
{
}

void Dialog::DoSave()
{
}

void Dialog::DoRestore() 
{
}

void Dialog::DoStore() 
{
}

void Dialog::DoSetup() 
{
}

Point Dialog::GetInitialPos()
{
    if (GetDefaultButton())
	return GetDefaultButton()->contentRect.Center();
    return GetExtent().Half();
}

int Dialog::ShowAt(VObject *fp, Point p)
{
    bool b= IsModified() && !GetRoot();
    Update();
    if (b)
	DoSetDefaults();
    DoSave();
    DoSetup();
    dw->OpenAt(p - GetInitialPos(), fp);
    return actionId;
}

int Dialog::Show()
{
    return ShowAt(gWindow, gToken.Pos);
}

int Dialog::ShowOnWindow(VObject *fp)
{
    return ShowAt(fp, fp->GetExtent().Half());
}

void Dialog::Control(int id, int part, void *vp)
{
    bool veto;

    switch (id) {
    case cIdDefault:
	DoSetDefaults();
	DoSetup();
	break;
    case cIdCloseBox:
	dw->Close();
	break;
    default:
	if (part == cPartAction) {
	    DialogView::Control(id, part, vp);
	    if (id != cIdCancel) {
		veto= FALSE;
		GetRoot()->SendDown(0, cPartValidate, &veto);
		if (veto) 
		    return;
	    }
	    actionId= id;
	    dw->Close();
	    if (id == cIdCancel)
		DoRestore();
	    else if (id == cIdOk)
		DoStore();
	    return;
	}
	break;
    }
    DialogView::Control(id, part, vp);
}

void Dialog::Close()
{
    dw->Close();
}

void Dialog::InspectorId(char *buf, int sz)
{
    if (dw)
	dw->InspectorId(buf, sz);
    else
	DialogView::InspectorId(buf, sz); 
}
