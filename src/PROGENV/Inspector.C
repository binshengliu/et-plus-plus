//$Inspector,FindInspReferences,RefreshHandler$ 

#include "ET++.h"
 
#include "Inspector.h"

#include "ObjList.h"
#include "OrdColl.h"
#include "Dictionary.h"
#include "ObjectTable.h"
#include "ClassItem.h"
#include "ClassManager.h"
#include "AccessMem.h"

#include "WindowSystem.h"
#include "Buttons.h"

#include "ObjectView.h"
#include "Reference.h"
#include "InspItem.h"
#include "ClassList.h"
#include "EtPeDoc.h"
#include "EtPeCmdNo.h"
#include "EtProgEnv.h"

const int cMaxObjects= 400,
	  cRefBufSize= 60;

//---- timeout handler to refresh inspector views -----------------------------

class RefreshHandler : public SysEvtHandler {
    VObject *inspector;
public:
    RefreshHandler(VObject *w) : SysEvtHandler(0)
	{ inspector= w; }
    bool HasInterest()
	{ return inspector->IsOpen(); }
    void Notify(SysEventCodes, int) 
	{ inspector->UpdateEvent(); }
};

//---- storage for Refs ---------------------------------------------------------

class RefBuf {
    int ix;
    Ref buf[cRefBufSize];
public:
    RefBuf()
	{ ix= 0; }
    Ref *NewRef();
};

Ref *RefBuf::NewRef()
{
    if (ix == cRefBufSize)
	ix= 0;
    return &buf[ix++];
}

static RefBuf gRefBuf;

//---- update References to deleted objects

void InspectorFreeHook(void *, void *addr, size_t sz)
{
    void *end= (void*)((u_long)addr + sz);
    Inspector *ip;
    for (int i= 0; i < Inspector::allInspectors->Size(); i++) {
	ip= (Inspector*)Inspector::allInspectors->At(i);
	if (addr != ip)
	    ip->FreedMemory(addr, end);
    }
}
  
//---- FindInspReferences --------------------------------------------------------------

class FindInspReferences: public AccessObjPtrs {
protected:
    Object *referencesTo;
    Inspector *ip;
public:
    FindInspReferences(Inspector *insp) : AccessObjPtrs(0)
	{ ip= insp; }
    void ReferencesTo(Object *op)
	{ referencesTo= op; }
    void FoundPointer(Object *op, char*, int);
};    

void FindInspReferences::FoundPointer(Object *op, char *name, int at)
{
    if (op == referencesTo && !inObject->IsKindOf(Assoc) && !inObject->IsKindOf(ObjectItem))
	ip->FoundReference(inObject, name, at);
}

//---- Inspector ----------------------------------------------------------------

MetaImpl(Inspector, (TP(classes), TP(objects), 
		     TP(ClassItems), TP(ObjectItems), TP(references),
		     TP(ReferenceItems), TP(refTitle), TP(objTitle), 0));

Inspector::Inspector() : Expander(cIdNone, eVert, gPoint2)
{
    Menu *menu;
    
    findReferences= new FindInspReferences(this); 
    
    classes= new ClassListView(this, TRUE);
    menu= new Menu("Classes");
    menu->AppendItems ("update",                cUPDATELIST,
		       "hide empty classes",    cEMPTYCLASSES, 
		       0);
    classes->SetMenu(menu);
    
    objects= new CollectionView(this, 0, eCVDefault);
    objects->SetMinExtent(Point(400, 0));
    objTitle= new TextItem("", gFixedFont->WithFace(eFaceItalic), Point(2,1));
    ResetObjects();
    menu= new Menu("Objects");
    menu->AppendItems ("all instances", cSHOWALL, 0);
    objects->SetId(cObjects);   
    objects->SetMenu(menu);
    
    references= new CollectionView(this, 0, eCVDefault, 0);
    references->SetMinExtent(Point(400, 0));
    refTitle= new TextItem("", gFixedFont->WithFace(eFaceItalic), Point(2,1));
    ResetReferences();
    references->SetId(cReferences);   
    
    objView= new ObjectView();
    objView->SetNextHandler(this);
    
    path= new OrdCollection;
    if (allInspectors == 0) {
	allInspectors= new OrdCollection;
	StorageSetFreeHook(InspectorFreeHook, 0);
    }
    allInspectors->Add(this);
    position= -1;
    
    Layout();
    gSystem->AddTimeoutHandler(refresh= new RefreshHandler(this));
    UpdateButtons();
}

Inspector::~Inspector()
{
    allInspectors->RemovePtr(this);
    if (allInspectors->Size() == 0) {
	StorageSetFreeHook(0, 0);
	SafeDelete(allInspectors);
    }
    if (refresh) {
	refresh->Remove();
	refresh= 0;
    }
    SafeDelete(objView);
    SafeDelete(findReferences);
    SafeDelete(classes);
    SafeDelete(objects);
    SafeDelete(references);
    SafeDelete(refTitle);
    SafeDelete(objTitle);
    SafeDelete(path);
}

void Inspector::Layout()
{   
    VObject *scr= new Scroller(classes, Point(-1, 100));
    scr->SetFlag(eVObjHFixed | eVObjVFixed);

    VObject *v= new Expander(cIdNone, eHor, gPoint2,
	    scr,
	    new Expander(cIdNone, eVert, gPoint2,
		new Clipper(objTitle, Point(0,-1)), 
		new Scroller(objects),
	    0),
	    new Expander(cIdNone, eVert, gPoint2, 
		new Clipper(refTitle, Point(0,-1)), 
		new Scroller(references),
	    0),
	0);

    VObject *v1= new Expander(cIdNone, eHor, gPoint8,
		       new Filler(gPoint0, eVObjVFixed),
		start= new ActionButton(cIdShiftStart, "||<"), 
		 left= new ActionButton(cIdShiftLeft,   "<<"), 
		       new ActionButton(cIdShiftAppl,   "Appl"), 
		right= new ActionButton(cIdShiftRight,  ">>"), 
		  end= new ActionButton(cIdShiftEnd,    ">||"), 
		       new Filler(gPoint0, eVObjVFixed),
		0
	    );
    v1->SetFlag(eVObjVFixed);

    Add(v);
    Add(v1);
    Add(new Expander(cIdNone, eHor, gPoint2, 
				new Scroller(objView, Point(550, 300)), 0));
}

void Inspector::Init(Object *op)
{
    Push(new Ref(*op));
}

void Inspector::MoveTo(int pos)
{
    pos= range(0, path->LastIndex(), pos);
    if (pos == position)
	return;
    position= pos;
    Ref *r= (Ref*)path->At(position);
    objView->SetInspected(r);
    ShowReference(r);
    UpdateButtons(); 
}

void Inspector::Push(Ref *r)
{
    if (position != path->LastIndex()) {
	while(path->Size() > position+1)
	    path->RemoveLast();
    }    
    int i= path->IndexOf(r);
    if (i < 0) {
	Ref *r1= gRefBuf.NewRef();
	*r1= *r;
	path->Add(r1);
    } else {
	while(path->Size() > i+1)
	    path->RemoveLast();
    }
    objView->SetInspected(r);
    ShowReference(r);
    position= path->LastIndex();
    UpdateButtons();
}

void Inspector::ShowReference(Ref *ref)
{
    Class *cl= 0;
    Object *op;
    
    if (op= ref->GetObject())
	cl= op->IsA();
    else {
	Reset();
	return;
    }
    ShowClass(cl); 
    ShowObject((Object*)ref->GetBase());   
}

void Inspector::ShowClass(Class *cl)
{   
    if (classes->SelectClass(cl)) 
	LoadObjectsOfClass(cl, TRUE);
}

void Inspector::ShowObject(Object *op)
{   
    ObjectItem *oi;
    Iter next(ObjectItems);
    for (int i= 0; oi= (ObjectItem*)next(); i++) {
	if (oi->GetObject() == (Object*)op) {
	    SelectAndRevealItem(objects, i); 
	    return;
	}        
    }
    objects->SetNoSelection();
}

void Inspector::SelectAndRevealItem(CollectionView *cv, int at)
{
    Rectangle r,sr= Rectangle(0, at, 1, 1);
    cv->SetSelection(sr);
    r= cv->ItemRect(sr);
    cv->RevealRect(r, Point(10, r.extent.y));
}

void Inspector::UpdateButtons()
{
    start->Enable(position != 0);
    left->Enable(position != 0);
    bool b= position == path->LastIndex();
    right->Enable(!b);
    end->Enable(!b);
}

void Inspector::LoadObjectsOfClass(Class *cl, bool members)
{    
    ObjectItems= new ObjList;
    Object *op;
    
    gObjectTable->Start(cl, members);
    for (int i= 0; op= (*gObjectTable)(); i++) {
	ObjectItems->Add(new ObjectItem(i, op));
	if (i == cMaxObjects) {
	    VObject *vop= new TextItem("...", gFixedFont->WithFace(eFaceItalic));
	    vop->Disable();
	    ObjectItems->Add(vop);
	    break;
	}
    }
    gObjectTable->End();
    if (i != 1)
	objTitle->SetFString(TRUE, "%d instances", i);
    else
	objTitle->SetFString(TRUE, "1 instance");
    objTitle->ForceRedraw();
    objects->SetCollection(ObjectItems, TRUE);
    objects->Update();    
}

void Inspector::Control(int id, int p, void *data)
{
    ObjectItem *oi;
    Rectangle r;

    switch (id) {
    
    case cIdShiftAppl:
	Push(new Ref(*gApplication));
	break;
    
    case cIdShiftLeft:
	MoveTo(position-1);
	break;
	
    case cIdShiftRight:
	MoveTo(position+1);
	break;

    case cIdShiftStart:
	MoveTo(0);
	break;
	
    case cIdShiftEnd:
	MoveTo(path->Size()-1);
	break;

    case cObjects:
	r= objects->GetSelection();
	if (r.IsEmpty())
	    return;
	oi= (ObjectItem*) ObjectItems->At(r.origin.y);
	
	if (oi == 0 || !oi->IsKindOf(ObjectItem))
	    return;
	DoInspect(oi->GetObject());
	references->SetNoSelection();
	break;
    
    case cIdCLChangedClass:
	LoadObjectsOfClass((Class*)data, TRUE);
	references->SetNoSelection();
	break;
    
    case cIdLoadRef: 
	Push((Ref*)data);
	break;
	
    case cIdLoadRefNew:
	Control(cIdSpawnInsp, -1, (Ref*)data);
	break;
	
    case cReferences:
	r= references->GetSelection();
	if (r.IsEmpty())
	    return;
	oi= (ObjectItem*) ReferenceItems->At(r.origin.y);
	
	if (oi == 0 || !oi->IsKindOf(ObjectItem))
	    return;
	DoInspect(oi->GetObject());
    }    
    EvtHandler::Control(id, p, data);    
}

void Inspector::DoInspect(Object *op)
{
    if (!ObjectTable::PtrIsValid(op))
	ShowAlert(eAlertNote, "instance 0x%x disappeard", op);
    else if (strcmp(op->ClassName(), "InspectorItem") == 0) // hack
	ShowAlert(eAlertNote, "would crash the inspector");    
    else
	Push(new Ref(*op));
}

void Inspector::ResetReferences()
{
    ReferenceItems= new OrdCollection(10);
    refTitle->SetString("References", TRUE);
    references->SetCollection(ReferenceItems, TRUE);
}

void Inspector::ResetObjects()
{
    objTitle->SetString("Number of instances", TRUE);
    ObjectItems= new ObjList(); 
    objects->SetCollection(ObjectItems, TRUE);
}

void Inspector::Reset()
{
    objects->SetNoSelection();
    classes->SetNoSelection();
}

void Inspector::References(Object *op)
{
    ReferenceItems= new OrdCollection(10);
    VObject *vop= new TextItem("__________", gFixedFont->WithFace(eFaceItalic));
    vop->Disable();
    refTitle->SetString(form("References to 0x%x (%s)", (int)op, op->ClassName()), TRUE);
    findReferences->ReferencesTo(op);
    
    gObjectTable->Start();    
    while (currentOp= (*gObjectTable)()) {
	if (currentOp->IsA()) {
	    findReferences->ForObject(currentOp);
	    currentOp->IsA()->EnumerateMembers(findReferences); 
	}
    } 
    gObjectTable->End();
    ReferenceItems->Add(vop);
    references->SetCollection(ReferenceItems, TRUE);
}

void Inspector::FoundReference(Object *op, char *name, int index)
{
    char *cp;
    if (index == -1)
	cp= form("%s.%s", op->ClassName(), name);
    else
	cp= form("%s.%s[%d]", op->ClassName(), name, index);     
    ReferenceItems->Add(new ObjectItem(cp, op));
}

Command *Inspector::DoMenuCommand(int cmd)
{   
    Object *insp= objView->GetInspected()->GetObject();
    
    switch (cmd) {
   
    case cSHOWALL:
	ShowAllInstances();
	break;
    
    case cUPDATELIST:
	classes->LoadClasses();
	ResetObjects();
	break;
    
    case cEMPTYCLASSES:
	classes->ToggleHideEmpty();
	ResetObjects();
	break;

    case cOBJREFERENCES:
	References(insp);
	break;
	
    case cSPAWN:
	Control(cIdSpawnInsp, -1, objView->GetInspected());
	break;

    case cEDITDECL:
    case cEDITIMPL:
	insp->EditSource(cmd == cEDITDECL);
	break;
	
    case cOBJBROWSER:
	Control(cIdOBrowser, 0, insp);
	break;
    default:
	return Expander::DoMenuCommand(cmd); 
    }
    return gNoChanges; 
}

void Inspector::ShowAllInstances()
{
    Class *cl= classes->SelectedClass();
    if (cl != 0)
	LoadObjectsOfClass(cl, FALSE);
}
   
void Inspector::DoSetupMenu(Menu *menu)
{
    EvtHandler::DoSetupMenu(menu);
    if (classes->SelectedClass())
	menu->EnableItem(cSHOWALL);

    menu->ToggleItem(cEMPTYCLASSES, classes->HideEmptyClasses(), 
			   "show all classes", "hide classes with no instances");
    menu->EnableItems(cUPDATELIST, cEMPTYCLASSES, 0);
}

void Inspector::FreedMemory(void *addr, void *end)
{
    Ref *r= objView->GetInspected();
    u_int base;
    if (r) {
	base= (u_int)r->Addr();
	if (base >= (u_int)addr && base < (u_int)end) 
	    objView->InspectedDied();
    }
    if (path->Size() == 0)
	return;
    for (int i= 0; i < path->Size(); i++) {
	r= (Ref*)path->At(i);
	base= (u_int)r->Addr();
	if (base >= (u_int)addr && base < (u_int)end) 
	    RemoveDeletedRef(i, r);
    }
}
	    
void Inspector::RemoveDeletedRef(int i, Ref *r)
{
    if (i <= position)
	position= max(0, position -1);
    path->RemovePtr(r);
    UpdateButtons();    
}
