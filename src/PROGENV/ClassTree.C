//$ClassTreeView, NameTreeNodeSelector, ClassReference, HierarchyBrowser$ 
//$MemberReference, CollectClassMembers, CollectClients$
 
#include "ClassTree.h"

#include "VObject.h"
#include "BackgroundItem.h"
#include "PopupItem.h"
#include "OrdColl.h"
#include "ClassItem.h"
#include "ProgEnv.h"
#include "Menu.h"
#include "Alert_e.h"
#include "Scroller.h"
#include "Panner.h"

#include "CmdNo.h"

#include "Application.h"

#include "Set.h"
#include "ClassManager.h"
#include "AccessMem.h"
#include "EtPeCmdNo.h"
#include "String.h"

static RGBColor *c1;

//---- ClassTreeView ----------------------------------------------------------

MetaImpl(ClassTreeView, (TP(menu), 0));

ClassTreeView::ClassTreeView(EvtHandler *eh, Class *root) 
				     : TreeView(eh, eTLLeftRight, eTCDiagonal2)
{
    gClassManager->SetupSubclasses();
    classes= new OrdCollection(200);
    InstallTree(root);   
    menu= new Menu("Tree");
    references= new OrdCollection;
}

ClassTreeView::~ClassTreeView()
{
    SafeDelete(menu);
    SafeDelete(classes);
    references->FreeAll();
    SafeDelete(references);
}

VObject *ClassTreeView::NodeAsVObject(Object *op)
{
    Class *cl= Guard(op, Class);
    VObject *vop= new ClassItem(cIdClassItem, cl);
    classes->Add(vop);
    return new BackgroundItem(ePatWhite, vop);    
}

Iterator *ClassTreeView::MakeChildrenIter(Object *op)
{
    return Guard(op, Class)->SubclassIterator();
}

Command *ClassTreeView::GetNodeSelector(TreeNode *tn, int cl)
{
    VObject *vop= tn->At(0)->FindItem(cIdClassItem);
    if (vop && vop->IsKindOf(ClassItem) && vop->TestFlag(eClItemCollapsed)) 
	return new NameTreeNodeSelector(tn, cl);
    return TreeView::GetNodeSelector(tn, cl);
}

void ClassTreeView::ShowClass(Class *cl)
{
    ClassItem *ci= FindClassItem(cl);
    if (ci) {
	RevealRect(ci->ContentRect().Expand(100), Point(0,100));
	SetSelection((VObject*)ci->GetNextHandler());
	UpdateEvent();
    }
}

Class *ClassTreeView::GetRootClass()
{
    TreeNode *tn= GetTree();
    ClassItem *cl= Guard(GetTree()->FindItem(cIdClassItem), ClassItem);
    return cl->GetClass();
}

void ClassTreeView::ShowAllClasses()
{
    classes->ForEach(ClassItem,ResetFlag)(eClItemCollapsed);
    CalcLayout();
} 

void ClassTreeView::ShowOnlyAbstractClasses()
{
    Iter next(classes);
    ClassItem *cl;
    while (cl= (ClassItem*)next()) 
	if (!cl->GetClass()->IsAbstract())
	    cl->SetFlag(eClItemCollapsed);
    CalcLayout();
}

void ClassTreeView::ShowOnlyApplicationClasses()
{
    Set interest;
    ClassItem *cl;
    Class *cp;
    Iter next(classes);
    const char *wd= ""; // gApplication->IsA()->GetCompDir();
    int n= strlen(wd);
    while (cl= (ClassItem*)next())  {
	cp= cl->GetClass();
	if (cp && strncmp("" /* cp->GetCompDir() */, wd, n) == 0) {
	    while (cp) {
		interest.Add(cp);
		cp= cp->Super();
	    }
	}
    } 
    next.Reset(classes);
    while (cl= (ClassItem*)next()) 
	if (!interest.Contains(cl->GetClass()))
	    cl->SetFlag(eClItemCollapsed);
    CalcLayout();
}

Menu *ClassTreeView::GetMenu()
{
    return menu;
}

void ClassTreeView::DoCreateMenu(Menu *menu)
{
    menu->AppendItems(
		      "collapse",                       cDOCOLLAPSE,
		      "promote",                        cPROMOTE,
		      "-",
		      "members",                        cMEMBERS,
		      "clients",                        cCLIENTS,
		      "show source",                    cSOURCE,
		      "inspect some instance",          cINSPSOME,
		      0);                      
}

void ClassTreeView::DoSetupMenu(Menu *menu)
{
    VObject *sel= GetSelection();
    if (sel) {
	TreeNode *tn= FindTreeNode(sel);
	char *current;
	if (tn->Collapsed())
	    current= "expand";
	else
	    current= "collapse";        
	menu->ReplaceItem(cDOCOLLAPSE, current);
    
	menu->EnableItems(cMEMBERS, cCLIENTS, cSOURCE, cDOCOLLAPSE, 
							cINSPSOME, cPROMOTE, 0);
    }    
}

Command *ClassTreeView::DoMenuCommand(int cmd)
{
    VObject *sel= GetSelection();
    ClassItem *cl= 0;
    
    if (sel)
	cl= Guard(selection->FindItem(cIdClassItem), ClassItem);
    
    switch (cmd) {
    case cMEMBERS:

	ShowMembers(cl);
	break;
	
    case cCLIENTS:
	ShowAllClients(cl);
	break;
	
    case cSOURCE:
	Control(cIdBrowser, -1, cl->GetClass());
	break;
	
    case cINSPSOME:
	InspectSomeInstance(cl);
	break;

    case cDOCOLLAPSE:
	Collapse(FindTreeNode(selection));
	break;
	
    case cPROMOTE:
	Control(cIdSpawnHier, -1, cl->GetClass());
	break;
	
    default:
	return TreeView::DoMenuCommand(cmd);
    }
    return gNoChanges;
}

void ClassTreeView::Draw(Rectangle r)
{
    if (references) {
	references->ForEach(ClassReference,Draw)(r);
    }
    TreeView::Draw(r);
}

void ClassTreeView::AddReference(ClassReference *cr)
{
    references->Add(cr); 
    InvalidateRect(cr->BBox());       
}

void ClassTreeView::RemoveAllReferences()
{
    references->FreeAll();
    references->Empty(0);
    ForceRedraw();
}

void ClassTreeView::InspectSomeInstance(ClassItem *cip)
{ 
    Class *clp= cip->GetClass();
    Object *op= clp->SomeMember();
    if (!op)
	op= clp->SomeInstance();
    if (op)
	op->Inspect();
    else
	ShowAlert(eAlertNote, "No instances found");
}

void ClassTreeView::OpenTreeNode(TreeNode *tn, bool)
{
    if (references->Size() == 0)
	return;
    Iter next(references);
    VObject *ci= tn->At(0)->FindItem(cIdClassItem);
    if (ci && ci->IsKindOf(ClassItem)) {
	ClassReference *cr;
	while (cr= (ClassReference*)next()) {
	    if (cr->Includes((ClassItem*)ci)) 
		InvalidateRect(cr->BBox()); 
	}      
    }
}

class CollectClassMembers : public AccessMembers {
    Collection *cp;
public:
    CollectClassMembers()
	{ cp= new OrdCollection; }
    ~CollectClassMembers()
	{ SafeDelete(cp); }
    void Member(char *, short, short, int, Class *cl)
	{ cp->Add(cl); }
    Collection *GetCollection()
	{ return cp; }
};

void ClassTreeView::ShowMembers(ClassItem *ci)
{
    CollectClassMembers clm;
    Class *cp= ci->GetClass(), *clp;
    Collection *colItems= new OrdCollection;
    
    cp->EnumerateMyMembers(&clm);
    Iter next(clm.GetCollection());
    while (clp= (Class *)next()) 
	colItems->Add(FindClassItem(clp));
    AddReference (new MemberReference(ci, colItems));        
}

class CollectClients : public AccessMembers {
    Collection *colp;
    Class *clp, *current;
public:
    CollectClients(Class *c)
	{ colp= new Set; clp= c; }
    ~CollectClients()
	{ SafeDelete(colp); }
    void SetCurrent(Class *cp)
	{ current= cp; }
    void Member(char *, short, short, int, Class *cl)
	{ if (clp == cl) colp->Add(current); }
    Collection *GetCollection()
	{ return colp; }
};

void ClassTreeView::ShowAllClients(ClassItem *ci)
{
    Class *cp;
    CollectClients clm(ci->GetClass());
    Iter next(gClassManager->Iterator());
    while (cp= (Class*)next()) {
	clm.SetCurrent(cp);
	cp->EnumerateMyMembers(&clm);
    }
    Collection *colItems= new OrdCollection;
    Iter next2(clm.GetCollection());
    while (cp= (Class *)next2()) 
	colItems->Add(FindClassItem(cp));
    AddReference (new ClassReference(ci, colItems, ePatBlack));        
}

ClassItem *ClassTreeView::FindClassItem(Class *cp)
{
    ClassItem dummy(0,cp);
    return (ClassItem*)classes->Find(&dummy);
}

TreeNode *ClassTreeView::FindTreeNode(VObject *vop)
{
    return (TreeNode*)vop->FindNextHandlerOfClass(Meta(TreeNode));
}

//---- NameTreeNodeSelector -------------------------------------------------------------

NameTreeNodeSelector::NameTreeNodeSelector(TreeNode *tn, int cl)
{
    item= tn;
    lastinside= FALSE;
    clicks= cl;
    fp= gFixedFont->WithFace(eFaceBold);
    VObject *vop= tn->At(0)->FindItem(cIdClassItem);
    if (vop && vop->IsKindOf(ClassItem)) 
	name= ((ClassItem*)vop)->ClassName();
    p= vop->ContentRect().NE();
}

void NameTreeNodeSelector::TrackFeedback(Point, Point, bool)
{
    if (item && (lastinside != inside)) {
	GrShowString(fp, ePatXor, p, (byte*) name);
	item->At(0)->Outline(0);
	lastinside= inside;
    }
}

Command *NameTreeNodeSelector::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    VObject *image= item->At(0);
    inside= image->ContainsPoint(np);
    if (atp == eTrackRelease) {
	if (item && lastinside) {
	    GrShowString(fp, ePatXor, p, (byte*) name);
	    image->Outline(0);
	    item->GetTreeView()->SetSelection(image);
	    return item->GetTreeView()->NodeSelected(item->At(0), clicks);
	}                
    }
    return this; 
}

//---- ClassReference --------------------------------------------------------------------------

ClassReference::ClassReference(ClassItem *c, Collection *refs, GrPattern p, int w)
{
    cl= c;
    nodes= refs;
    width= w;
    pat= p;
}

ClassReference::~ClassReference()
{
    SafeDelete(nodes);
}

bool ClassReference::Includes(ClassItem *ci)
{
    return nodes->ContainsPtr(ci) || (ci == cl);
}

void ClassReference::Draw(Rectangle)
{
    ClassItem *clp;
    Iter next(nodes);

    if (!cl->IsOpen())
	return;
    GrSetPenNormal();
    GrStrokeRect(cl->contentRect.Expand(1)); 
    while (clp= (ClassItem*)next()) {
	if (clp->IsOpen()) {
	    GrSetPenNormal();
	    GrStrokeRect(clp->ContentRect().Expand(1));
	    DrawConnection(cl, clp);
	}
    }
}

void ClassReference::DrawConnection(ClassItem *from, ClassItem *to)
{
    GrMoveto(from->contentRect.Center());
    GrSetPenSize(1);
    SetWidthAndPattern(from, to);
	
    if (from == to) 
	CircularReference(from);
    else {
	GrMoveto(from->contentRect.Center());
	GrLineto(to->contentRect.Center());
    }
}

static RGBColor *c2;

void ClassReference::SetWidthAndPattern(ClassItem*, ClassItem*)
{
    if (c2 == 0)
	c2= new RGBColor(0, 256, 0);
    GrSetPenSize(width);
    GrSetPenPattern(c2);
}

void ClassReference::CircularReference(ClassItem *ci)
{
    int d= ci->ContentRect().Height();
    Rectangle rr(ci->ContentRect().NE()-Point(d/2), Point(d)); 
    GrStrokeOval(rr);
}

Rectangle ClassReference::BBox()
{
    Rectangle r;
    VObject *clp;
    Iter next(nodes);
  
    r= cl->ContentRect().Expand(20);
    while ((clp= (VObject *)next())) 
	    r.Merge(clp->ContentRect().Expand(4));
    return r;
}

//---- MemberReference ---------------------------------------------------------

MemberReference::MemberReference(ClassItem *cl, Collection *n) : ClassReference(cl, n)
{
}

void MemberReference::SetWidthAndPattern(ClassItem*, ClassItem*)
{
    if (c1 == 0)
	c1= new RGBColor(256, 0, 0);
    GrSetPenPattern(c1);
}

//---- HierarchyBrowser --------------------------------------------------------

MetaImpl(HierarchyBrowser, (TP(ct), 0));

HierarchyBrowser::HierarchyBrowser(Class *root)
					    : Expander(cIdNone, eVert, Point(5))
{
    Scroller *scl;
    Panner *sl;
    
    ct= new ClassTreeView(this, root);
    scl= new Scroller(ct);
    sl= new Panner(scl->GetClipper());

    Menu *m= new Menu("", FALSE, 0, 1, FALSE);
    m->AppendItems(
	    "all classes",               cALLCLASSES,
	    "abstract classes",          cONLYABSTRACT,
	    0); 
    VObject *filter= new PopupItem(cIdClFilter, cALLCLASSES, "show:", m);
	
    VObject *actions=
	new Cluster (cIdNone, eVObjVBottom, 10, 
	    new Filler(Point(40), eVObjVFixed),
	    filter,
	    new ActionButton         (cCLEARREFS,       "clear"),
	    new ActionButton         (cPRINT,           "print..."),
	    new ActionButton         (cSPAWN,           "spawn"),
	    sl,
	    0
	);
    actions->SetFlag(eVObjVFixed);
    Add(scl);
    Add(actions);
}

HierarchyBrowser::~HierarchyBrowser()
{
    SafeDelete(ct);
}

void HierarchyBrowser::ShowClass(Class *cp)
{
    ct->ShowClass(cp);
}

void HierarchyBrowser::Control(int id, int part, void *vp)
{
    switch (id) {
    case cIdClFilter:
	if (part == cONLYABSTRACT)
	    ct->ShowOnlyAbstractClasses();
	else
	    ct->ShowAllClasses();
	break; 
	
    case cCLEARREFS:
	ct->RemoveAllReferences();
	break;
	
    case cPRINT:
	ct->DoMenuCommand(cPRINT);

    case cSPAWN:
	Expander::Control(cIdSpawnHier, -1, ct->GetRootClass());
	break;  
		 
    default:
	Expander::Control(id, part, vp);
    }
}
