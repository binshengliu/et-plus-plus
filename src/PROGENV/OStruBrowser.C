//$ObjectStructureView, FindSelPtrs$
//$LabeledGraphReference, ObjectBrowser, NestedIter, FindSelReferences$

#include "ET++.h"

#include "CmdNo.h"
#include "Scroller.h"
#include "Panner.h"
#include "BackgroundItem.h"
#include "Cluster.h"
#include "Buttons.h"

#include "WindowSystem.h"
#include "System.h"

#include "ObjectTable.h"
#include "AccessMem.h"
#include "ByteArray.h"
#include "OrdColl.h"

#include "OStruBrowser.h"
#include "EtPeCmdNo.h"
#include "ClassItem.h"

//---- FindSelReferences ------------------------------------------------

class FindSelReferences: public AccessObjPtrs {
    Object *referencesTo;
    Collection *col;
    Collection *names;
public:
    FindSelReferences(Object *ref, Collection *cp, Collection *l= 0)
	{ col= cp; names= l; referencesTo= ref;}
    void FoundPointer(Object *op, char *n, int i);
};    

void FindSelReferences::FoundPointer(Object *op, char *n, int i)
{
    if (op && op == referencesTo) {
	char *cp;    
	col->Add(inObject);
	if (i != -1)
	    cp= form("%s[%d]", n, i);
	else
	    cp= n;
	if (names)
	    names->Add(new ByteArray(n));
    }
}

//---- FindSelPtrs -----------------------------------------------------------

class FindSelPtrs: public AccessObjPtrs {
    Collection *col;
    Collection *names;
public:
    FindSelPtrs(Collection *cp, Collection *n= 0)
	{ col= cp; names= n; }
    void FoundPointer(Object *op, char*, int= -1);
};    

void FindSelPtrs::FoundPointer(Object *op, char *name, int)
{ 
    if (op) 
	col->Add(op); 
    if (names)
	names->Add(new ByteArray(name));
}

//---- ObjectStructureView ------------------------------------------------------------

MetaImpl(ObjectStructureView, (TB(lines), 0));

static Ink *depPat, *evtPat, *refPat, *contPat, *pointPat;
static int width;

ObjectStructureView::ObjectStructureView() : GraphView((EvtHandler*)0)
{ 
    if (depPat == 0) { // init colors
	if (gColor) {
	    depPat= new RGBColor(255, 0, 255);
	    evtPat= new RGBColor(255, 0, 0);
	    contPat= new RGBColor(0, 255, 128);
	    refPat= new RGBColor(0, 0, 255);
	    pointPat= new RGBColor(0, 255, 255);
	    width= 3;
	} else {
	    depPat= ePatGrey50;
	    evtPat= ePatGrey75;
	    contPat= ePatGrey25;
	    refPat= ePatGrey60;
	    pointPat= ePatBlack;
	    width= 6;
	}
    }
    currentOp= 0;
    lines= varNames= TRUE; 
    menu= new Menu("Object Structure");
}

Object *ObjectStructureView::GetRoot()
{
    return rootOp;
}

VObject *ObjectStructureView::NodeAsVObject(Object *vop)
{
    char buf[20], *cp;
    buf[0]= '\0';
    vop->InspectorId(buf, sizeof buf);
    if (strlen(buf))
	cp= buf;
    else
	cp= form("0x%x", (int) vop);
    return
	new BackgroundItem(ePatWhite, 
	    new Cluster(cIdNone, eVObjHCenter, 0,
		new TextItem(vop->ClassName(), gFixedFont->WithFace(eFaceBold),
		    Point(2,1)
		),
		new ObjectItem(cObjItemId, cp, vop),
		0
	    )
	);
}

Iterator *ObjectStructureView::MakeSubPartsIter(Object *op)
{
    OrdCollection *col= new OrdCollection;
    op->Parts(col);
    return new NestedIter(col, TRUE);
}

Command *ObjectStructureView::NodeSelected(VObject *t, int cl)
{
    Command *cmd= GraphView::NodeSelected(t, cl);
    
    VObject *vop= t->FindItem(cObjItemId);
    if (vop) {
	Object *op= Guard(vop, ObjectItem)->GetObject();
	if (ObjectTable::PtrIsValid(op)) {
	    if (cl > 1)
		op->Inspect();
	    else
		currentOp= op;
	} else
	    ShowAlert(eAlertNote, "instance 0x%x disappeard", op);
    }
    return cmd;
}

void ObjectStructureView::LoadETGraph(Object *root)
{
    RemoveAllPaths();
    RemoveAllReferences();
    EmptyGraph();
    rootOp= root;
    BuildGraphBFS(rootOp);
    GraphNode *gn= AssociatedGraphNode(rootOp);
    SetGraph(gn);
    Rectangle r= gn->Image()->ContentRect();
    RevealRect(r.Expand(Point(0,200)), r.extent); 
}

bool ObjectStructureView::CheckObject(Object *op)
{
    if (!ObjectTable::PtrIsValid(op)) {
	ShowAlert(eAlertNote, "instance 0x%x disappeard", op);
	return FALSE;
    }
    return TRUE;
}

void ObjectStructureView::EvtHandlers(Object *op)
{
    if (!CheckObject(op))
	return;
    if (!op->IsKindOf(EvtHandler))
	return;
    EvtHandler *ep= (EvtHandler*)op;
    OrdCollection *oc= new OrdCollection;
    oc->Add(ep);
    while (ep= ep->GetNextHandler())
	oc->Add(ep);
    if (oc->Size()) {
	GraphPath *gp= new GraphPath(this, oc, evtPat, width, TRUE);
	AddPath(gp);
    } else {
	ShowAlert(eAlertNote, "No next eventhandler");
	SafeDelete(oc);
    }
}

void ObjectStructureView::Containers(Object *op)
{
    if (!CheckObject(op))
	return;
    if (!op->IsKindOf(VObject))
	return;
    VObject *vp= (VObject*)op;
    OrdCollection *oc= new OrdCollection;
    oc->Add(vp);
    while (vp= vp->GetContainer())
	oc->Add(vp);
    if (oc->Size()) {
	GraphPath *gp= new GraphPath(this, oc, contPat, width, TRUE);
	AddPath(gp);
    } else {
	ShowAlert(eAlertNote, "No container");
	SafeDelete(oc);
    }
}

void ObjectStructureView::Dependents(Object *op)
{
    if (!CheckObject(op))
	return;
    Iter next(op->GetObserverIter());
    OrdCollection *oc= new OrdCollection;
    Object *dep;
    while (dep= next())
	oc->Add(dep);
    GraphReference *gp= new GraphReference(this, op, oc, depPat, width, TRUE);
    AddReference(gp);
}

void ObjectStructureView::References(Object *op)
{
    if (!CheckObject(op))
	return;
    GrShowWaitCursor cw;
    OrdCollection *refs= new OrdCollection, *names= 0;
    if (varNames)
	names= new OrdCollection;
    FindSelReferences collector(op, refs, names);
    Object *cop;
    gObjectTable->Start();    
    while (cop= (*gObjectTable)()) {
	if (cop->IsA()) {
	    collector.ForObject(cop);
	    cop->IsA()->EnumerateMembers(&collector); 
	}
    } 
    gObjectTable->End();
    if (refs->Size()) {
	GraphReference *gp;
	if (varNames)
	    gp= new LabeledGraphReference(this, op, refs, names, refPat, 2, TRUE);
	else
	    gp= new GraphReference(this, op, refs, refPat, 2, TRUE);
	AddReference(gp);
    }
    else {
	ShowAlert(eAlertNote, "No references");
	SafeDelete(refs);
	SafeDelete(names);
    }
}

void ObjectStructureView::Pointers(Object *op)
{
    if (!CheckObject(op))
	return;
    GrShowWaitCursor cw;
    OrdCollection *pointers= new OrdCollection, *names= 0;
    if (varNames)
	names= new OrdCollection;
    if (op->IsA()) {
	FindSelPtrs collector(pointers, names);
	collector.ForObject(op);
	op->IsA()->EnumerateMembers(&collector); 
    }
    if (pointers->Size()) {
	GraphReference *gp;
	if (varNames)
	    gp= new LabeledGraphReference(this, op, pointers, names, pointPat, 2, TRUE);
	else
	    gp= new GraphReference(this, op, pointers, pointPat, 2, TRUE);
	AddReference(gp);
    } else {
	ShowAlert(eAlertNote, "No pointers");
	SafeDelete(pointers);
	SafeDelete(names);
    }
}

void ObjectStructureView::ShowObject(Object *op)
{
    VObject *vop= AssociatedVObject(op);
    if (vop) {
	currentOp= op;
	SetSelection(vop);
	RevealRect(vop->ContentRect().Expand(100), Point(0,100));
    }
}

void ObjectStructureView::ToggleShowLines()
{
    lines= !lines;
    if (lines) 
	SetConnType(eTCDiagonal);
    else 
	SetConnType(eTCNone);
}

Command *ObjectStructureView::DoMenuCommand(int cmd)
{
    switch (cmd) {
    case cEVTHANDLER:
	EvtHandlers(currentOp);
	break;
	
    case cCONTAINER:
	Containers(currentOp);
	break;

    case cDEPENDENCIES:
	Dependents(currentOp);
	break;

    case cREFERENCES:
	References(currentOp);
	break;

    case cPOINTERS:
	Pointers(currentOp);
	break;

    case cPROMOTE:
	Control(cIdSpawnOBrowser, -1, currentOp);
	break;

    case cDOINSPECT:
	if (!ObjectTable::PtrIsValid(currentOp)) 
	    ShowAlert(eAlertNote, "instance 0x%x disappeard", currentOp);
	else
	    currentOp->Inspect();
	break;

    case cSHOWLABELS:
	varNames= !varNames;
	break;
	
    default:
	break;
    }
    return View::DoMenuCommand(cmd);
}

Menu *ObjectStructureView::GetMenu()
{
    return menu;
}

void ObjectStructureView::DoCreateMenu(Menu *)
{
    menu->AppendItems(
		      "undo",               cUNDO,
		      "-",
		      "inspect",            cDOINSPECT,
		      "-",
		      "event handlers",     cEVTHANDLER,
		      "containers",         cCONTAINER,
		      "dependencies",       cDEPENDENCIES,
		      "-",
		      "referenced from",    cREFERENCES,
		      "points to",          cPOINTERS,
		      "-",
		      "show names",         cSHOWLABELS,
		      "promote",            cPROMOTE, 
		      0);
}

void ObjectStructureView::DoSetupMenu(Menu *menu)
{
    GraphView::DoSetupMenu(menu);
    menu->ToggleItem(cSHOWLINES, lines, "%s lines", "hide", "show");
    menu->ToggleItem(cSHOWLABELS, varNames, "%s names", "hide", "show");
    
    menu->EnableItems(cUPDATE, cCLEARPATH, cSHOWLINES, cSHOWLABELS, 0);
    if (currentOp) {
	Iterator *it= currentOp->GetObserverIter();
	Iter n(it);
	menu->EnableItems(cREFERENCES, cDOINSPECT, cPOINTERS, cPROMOTE, 0);        
	if (currentOp->IsKindOf(EvtHandler))
	    menu->EnableItem(cEVTHANDLER);
	if (it)
	    menu->EnableItem(cDEPENDENCIES);
	if (currentOp->IsKindOf(VObject))
	    menu->EnableItem(cCONTAINER);
    }
}

//---- LabeledGraphReference ------------------------------------------------------------

LabeledGraphReference::LabeledGraphReference(
	    GraphView *gv, Object *op, Collection *n, Collection *l, 
		    Ink *i, int w, bool f) : GraphReference(gv, op, n, i, w, f)
{
    labels= l;
}

 
LabeledGraphReference::~LabeledGraphReference()
{
    SafeDelete(labels);
}

void LabeledGraphReference::DrawConnection(int n, VObject *from, VObject *to)
{
    GraphReference::DrawConnection(n, from, to);

    Point p1= from->contentRect.Center();
    Point p2= to->contentRect.Center();
    Rectangle r1= NormRect(p1, p2);
    ByteArray *b= (ByteArray*)labels->At(n);
    Font *fp= gFixedFont->WithFace(eFaceItalic);
    char *name= (char*) b->Str();
    Rectangle r2= fp->BBox((byte*)name);
    Point p= r1.Center() - r2.Center();
    GrEraseRect(Rectangle(p, r2.extent));
    GrShowString(fp, ePatBlack, p + Point(0, fp->Ascender()), (byte*)name);
}

//---- ObjectBrowser ----------------------------------------------------

MetaImpl(ObjectBrowser, (TP(osv), 0));

ObjectBrowser::ObjectBrowser() : Expander(cIdNone, eVert, Point(5))
{
    osv= new ObjectStructureView();
    osv->SetNextHandler(this);
    Scroller *scl= new Scroller(osv);
    Panner  *sl= new Panner(scl->GetClipper());
    VObject *actions=
	new Cluster (cIdNone, eVObjVBottom, 10, 
	    new Filler(Point(40), eVObjVFixed),
	    new ActionButton (cUPDATE,      "update"), 
	    new ActionButton (cCLEARPATH,   "clear"), 
	    new ActionButton (cSHOWLINES,   "lines on/off"),
	    new ActionButton (cPRINT,       "print..."),
	    new ActionButton (cSPAWN,       "spawn"),
	    sl,
	    0
	);
    actions->SetFlag(eVObjVFixed);
    Add(scl);
    Add(actions);
}

ObjectBrowser::~ObjectBrowser()
{
    SafeDelete(osv);
}

void ObjectBrowser::ShowObject(Object *op)
{
    osv->LoadETGraph(op);
}
    
void ObjectBrowser::Control(int id, int part, void *vp)
{
    switch (id) {
    case cUPDATE:
	osv->LoadETGraph(osv->GetRoot()); 
	break;
	
    case cCLEARPATH:
	osv->RemoveAllPaths();
	osv->RemoveAllReferences();
	break;
    case cSHOWLINES:
	osv->ToggleShowLines();
	break;

    case cSPAWN:
	Expander::Control(cIdSpawnOBrowser, -1, osv->GetRoot()); 
	break;
	
    case cPRINT:
	osv->DoMenuCommand(cPRINT);
	
    default:
	Expander::Control(id, part, vp);
    }    
}

//---- NestedIter ----------------------------------------------------------

NestedIter::NestedIter(Object *op1, Object *va_(op2), ...)
{
    va_list ap;    
    va_start(ap,va_(op1));
    cont= new OrdCollection(20);
    Object *op;
    
    cont->Add(op1);
    cont->Add(va_(op2));
    while (op= va_arg(ap, Object*))
	cont->Add(op);
    it= cont->MakeIterator();
    subIter= 0;
    free= TRUE;
}
 
NestedIter::NestedIter(Collection *col, bool f)
{
    cont= col;
    it= cont->MakeIterator();
    subIter= 0;
    free= f;
}

NestedIter::~NestedIter()
{
    if (free)
	SafeDelete(cont);
    SafeDelete(it);
    SafeDelete(subIter);
}

void NestedIter::Reset(Collection *)
{
    Error("NestedIter::Reset", "May not call Reset");
}

Collection *NestedIter::Coll()
{
    Error("NestedIter::Coll", "May not call Coll");
    return 0;
}

ObjPtr NestedIter::operator()()
{
    Object *op= 0;
    
    while (op == 0) {
	if (subIter) {
	    op= subIter->operator()();
	    if (op == 0) {
		SafeDelete(subIter);
		continue;
	    }
	} 
	else {
	    op= it->operator()();
	    if (op && op->IsKindOf(Collection)) {
		subIter= ((Collection*)op)->MakeIterator();
		op= 0;
		continue;
	    }
	    else
		break;
	}
    }
    return op;
}
