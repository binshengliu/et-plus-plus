//$MethodBrowser,MethodItem,Implementors,MethodReference,MethodFilterDialog$

#include "MethodB.h"

#include "OrdColl.h"
#include "RegularExp.h"
#include "ObjectTable.h"
#include "ClassManager.h"
#include "String.h"

#include "System.h"

#include "Menu.h"
#include "Scroller.h"
#include "Buttons.h"
#include "EditTextItem.h"
#include "ManyOfCluster.h"
#include "Alert_e.h"
#include "Expander.h"
#include "Window.h"

#include "EtPeCmdNo.h"
#include "EtPeDoc.h"
  
static MethodFilterDialog *filter= 0;

const int cIdFilter     =   cIdFirstUser + 1,
	  cIdVisibility =   cIdFirstUser + 2;   

//---- MethodItem --------------------------------------------------------------

class MethodItem: public TextItem {
public:
    Class *cl;
    MethodReference *mrp;

    MetaDef(MethodItem);    
    MethodItem(MethodReference *method, char *label);
    MethodItem(Class *cl);
    int Line()
	{ return mrp->Line(); }
    Class *GetClass()
	{ return mrp ? mrp->GetClass() : 0; }
    bool IsPublic()
	{ return mrp->IsPublic(); }
};

MetaImpl(MethodItem, (TP(mrp), 0));

MethodItem::MethodItem(MethodReference *m, char *label) 
			    : TextItem(cIdNone, label, gFixedFont, Point(2,1))
{
    mrp= m;
}

MethodItem::MethodItem(Class *cp)
	    : TextItem(cIdNone, 0, gFixedFont->WithFace(eFaceBold), Point(2,1))
{
    mrp= 0;
    cl= cp;
    SetFString(FALSE, "Class: %s", cp->Name());
}

//---- MethodBrowser ----------------------------------------------------------

MetaImpl0(MethodBrowser);

MethodBrowser::MethodBrowser(EvtHandler *b) 
				    : CollectionView(b, 0, eCVDontStuckToBorder)
{
    allMethods= filteredMethods= 0;
    ofClass= 0;
    menu= new Menu("Methods");
    SetMinExtent(Point(250, 0));
}

static ObjPtr AsMethodItem(ObjPtr, ObjPtr op, void*)
{
    MethodReference *mr= (MethodReference*)op;
    return new MethodItem(mr, (char*) mr->Str());
}

void MethodBrowser::ShowMethodsOf(Class* cp)
{
    Collection *oc= 0, *methodsOfClass;
    
    if (cp != 0) {
	oc= new OrdCollection;
	ofClass= cp;
	for (Class *cl= cp; cl != 0; cl= cl->Super()) {
	    oc->Add(new MethodItem(cl));
	    Collection *methods= gEtPeDoc->LoadMethodsOfClass(cl);
	    if (methods) {
		methodsOfClass= methods->Collect(AsMethodItem);
		oc->AddAll(methodsOfClass);
		SafeDelete(methodsOfClass);
	    }
	}
    }
    if (allMethods)
	allMethods->FreeAll();
    SafeDelete(allMethods);
    allMethods= oc;
    SetCollection(allMethods, FALSE);
    UpdateEvent();
}
 
void MethodBrowser::SelectMethod(MethodReference *mr)
{
    MethodItem *mi;
    Iter next(GetCollection());
    for (int i= 0; (mi= (MethodItem*)next()) != 0; i++) {
	if (mi->mrp && mr->IsEqual(mi->mrp)) {
	    Rectangle sr= Rectangle(0, i, 1, 1);
	    SetSelection(sr);
	    Rectangle r= ItemRect(sr);
	    RevealRect(r, Point(10, r.extent.y));
	    return;
	}
    }
}

class MethodItem *MethodBrowser::GetSelectedMethod()
{
    Collection *col= GetCollection();
    if (GetSelection().IsEmpty())
	return 0;
    int at= GetSelection().origin.y;
    MethodItem *mi= Guard(col->At(at), MethodItem);
    return mi;
}

void MethodBrowser::DoSelect(Rectangle, int)
{
    MethodItem *mi= GetSelectedMethod();
    if (mi == 0)
	return;
    if (mi->mrp != 0) 
	Control(cIdChangedMethod, 0, mi->mrp); 
    else {   
	Control(cIdCLChangedClass, 0, mi->cl);
	SetNoSelection();
    }
}

void MethodBrowser::Filter()
{
    if (filter == 0)
	ObjectTable::AddRoot(filter= new MethodFilterDialog());
    if (filter->ShowOnWindow(GetWindow()) == cIdYes) {
	Collection *fp= filter->DoIt(allMethods);
	if (fp == 0)
	    return;
	SetCollection(fp, FALSE);
	UpdateEvent();
	SafeDelete(filteredMethods);
	filteredMethods= fp;    
    }
}

void MethodBrowser::RemoveFilter()
{
    if (filteredMethods != 0) {
	SetCollection(allMethods, FALSE);
	SafeDelete(filteredMethods);
	UpdateEvent();
    }
}

void MethodBrowser::DoCreateMenu(Menu *menu)
{
    menu->AppendItems("implementors",       cIMPLEMENTORS,
		      "overrides",          cOVERRIDES,
		      "inherited",          cINHERITED,
		      "references",         cMETHREFERENCES,
		      "-",
		      "filter...",          cFILTER,
		      "remove filter",      cREMFILTER,
		      0);
}

void MethodBrowser::DoSetupMenu(Menu *menu)
{
    if (GetSelectedMethod() != 0) {
	menu->EnableItem(cIMPLEMENTORS);
	menu->EnableItem(cOVERRIDES);
	menu->EnableItem(cINHERITED);
    }
    menu->EnableItems(cFILTER, cREMFILTER, 0);
}

Command *MethodBrowser::DoMenuCommand(int cmd)
{
    MethodItem *mi= GetSelectedMethod();
    switch (cmd) {
    case cIMPLEMENTORS:
	Control(cIdImplementors, 0, mi->mrp);
	break;
	
    case cOVERRIDES:
	Control(cIdOverrides, 0, mi->mrp);
	break;
	
    case cINHERITED:
	Control(cIdInherited, 0, mi->mrp);
	break;
	
    case cFILTER:
	Filter();
	break;    

    case cREMFILTER:
	RemoveFilter();
	break;    
    }
    return CollectionView::DoMenuCommand(cmd);
}

//---- Implementors -------------------------------------------------------

Implementors::Implementors(EvtHandler *b) : CollectionView(b, 0)
{
}
 
void Implementors::ShowImplementorsOf(MethodReference *mrp, bool subClassesOnly)
{
    Collection *oc= new OrdCollection;
    Class *cl, *acl= mrp->GetClass();
    char *method= (char*)mrp->Str();
    
    Iter next(gClassManager->Iterator());
    while (cl= (Class*)next()) {
	if (subClassesOnly && !(cl->isKindOf(acl) && cl != acl))
	    continue;
	Collection *methods= gEtPeDoc->LoadMethodsOfClass(cl);
	if (methods) {
	    Iter next1(methods);
	    MethodReference *mr;
	    while (mr= (MethodReference*)next1()) {
		if (gEtPeDoc->SameMethods((char*)mr->Str(), method)) 
		    oc->Add(new MethodItem(mr, form("%s::%s", cl->Name(), method)));
		else if (gEtPeDoc->IsDestructor(method) && gEtPeDoc->IsDestructor((char*)mr->Str()))
		    oc->Add(new MethodItem(mr, form("%s::~%s", cl->Name(), cl->Name())));
	    }
	}
    }
    SetCollection(oc);
    UpdateEvent();
}

void Implementors::ShowInherited(MethodReference *mrp)
{
    Collection *oc= new OrdCollection;
    Class *cl, *ofClass= mrp->GetClass();
    char *methodName= (char*)mrp->Str();
    cl= ofClass->Super();

    for (; cl != 0; cl= cl->Super()) {
	Collection *methods= cl->GetMethods();
	if (methods == 0) {
	    methods= gEtPeDoc->LoadMethodsOfClass(cl);
	    cl->SetMethods(methods);
	}
	Iter next1(methods);
	MethodReference *mr;
	while (mr= (MethodReference*)next1()) {
	    if (gEtPeDoc->SameMethods((char*)mr->Str(), methodName)) 
		oc->Add(new MethodItem(mr, form("%s::%s", cl->Name(), methodName)));
	    else if (gEtPeDoc->IsDestructor(methodName) && gEtPeDoc->IsDestructor((char*)mr->Str()))
		oc->Add(new MethodItem(mr, form("%s::~%s", cl->Name(), cl->Name())));
	}
    }
    SetCollection(oc);
    UpdateEvent();
}

void Implementors::DoSelect(Rectangle, int)
{
    Collection *col= GetCollection();
    int at= GetSelection().origin.y;
    MethodItem *mi= Guard(col->At(at), MethodItem);
    Control(cIdChangedMethod, 0, mi->mrp);
    SetNoSelection();    
}

//---- MethodFilterDialog ----------------------------------------------------

MethodFilterDialog::MethodFilterDialog() : Dialog(0, eBWinBlock)
{
    rex= new RegularExp;
    hidePublic= hidePrivate= hideProtected= FALSE;    
}

VObject *MethodFilterDialog::DoCreateDialog()
{
    VObject *filter=
	new BorderItem ("Filter (regular expression)", 
	    pattern= new EditTextItem(cIdFilter, "", 250),
	    0
	);   
    VObject *ok= 
	new Expander(cIdNone, eVert, gPoint2,
	    new ActionButton(cIdYes, "Ok", TRUE),
	    new ActionButton(cIdCancel, "Cancel"),
	    0
	    ); 
		       
    VObject *visibility=
	new BorderItem ("Hide",
	    new ManyOfCluster(cIdVisibility, eVObjVBase, Point(4,4),
		"public", "protected", "private", 0)
	);

    return
	new BorderItem( 
	    new BorderItem(
		new Cluster(cIdNone, eVObjVCenter, 10, 
		    new Expander(cIdNone, eVert, gPoint2,
			filter,
			visibility,
			0
		    ),
		    ok,
		    0
		),
		20, 3
	    ),
	    Point(2,2),
	    1
	);
}

void MethodFilterDialog::Control(int id, int p, void *v)
{
    switch (id) {
    
    case cIdVisibility:
	switch(p) {
	case cIdVisibility:
	    hidePublic= !hidePublic;
	    break;
	case cIdVisibility+1:
	    hideProtected= !hideProtected;
	    break;
	case cIdVisibility+2:
	    hidePrivate= !hidePrivate;
	    break;
	}
	break;
	 
    default:
	Dialog::Control(id, p, v);
    } 
}

Collection *MethodFilterDialog::DoIt(Collection *allMethods)
{
    MethodItem *mi;
    bool matchAll= FALSE;
    
    char *pat= pattern->GetText()->AsString();
    if (strlen(pat) == 0) 
	matchAll= TRUE;
    else {
	rex->Reset(pat, TRUE);
	if (rex->GetExprState()) {
	    ShowAlert(eAlertNote, rex->GetExprState());
	    return 0;
	}
    }
    SafeDelete(pat);
    Collection *filteredMethods= new OrdCollection;
    Iter next(allMethods);
    while (mi= (MethodItem *)next()) {
	int n, match= 0;
	if (mi->GetClass() == 0) {
	    filteredMethods->Add(mi);
	    continue;
	}
	if (!matchAll)
	    match= rex->SearchForward(mi->AsString(), &n);
	if (matchAll || match != -1) {
	    if (mi->IsPublic() && !hidePublic)
		filteredMethods->Add(mi);
	    if (!mi->IsPublic() && !hidePrivate)
		filteredMethods->Add(mi);
	}
    }
    return filteredMethods;
}
