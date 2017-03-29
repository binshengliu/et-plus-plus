//$ObjectView,ShowMembers$

#include "ObjectView.h"
#include "EtPeDoc.h"
#include "EtPeCmdNo.h"

#include "Application.h"
#include "Menu.h"

#include "ObjArray.h"
#include "ObjList.h"
#include "String.h"
#include "AccessMem.h"
#include "System.h"

#include "ProgEnv.h"
#include "InspItem.h"
#include "Reference.h"

const int cMaxItems = 100;

//---- ShowMembers -------------------------------------------------------------

class ShowMembers: public AccessMembers {
    int supercnt;
    ObjectView *dv;
    Object *op;
public:
    void Reset(Object *op, ObjectView *v);
    void ClassName(char *name);
    void Member(char *name, short offset, short offsetOrLen, int type, Class *cl)
	{ dv->Add(new InspectorItem(name, new Ref(op, type, offset, offsetOrLen, cl))); }
};

void ShowMembers::Reset(Object *o, ObjectView *v)
{
    supercnt= 0;
    dv= v;
    op= o;
}

void ShowMembers::ClassName(char *name)
{
    if (supercnt == 0)
	dv->Add(new ClassItem0(op, name));
    else
	dv->Add(new SuperItem0(name));
    supercnt++;
}

//---- ObjectView ---------------------------------------------------------------

MetaImpl0(ObjectView);

ObjectView::ObjectView() : CollectionView(0, new ObjList, eCVDefault)
{
    menu= new Menu("inspector");
    inspected= 0;
    if (accessor == 0) {
	accessorcnt= 0;
	accessor= new ShowMembers;
    }
    accessorcnt++;
}

ObjectView::~ObjectView()
{
    if (--accessorcnt <= 0)
	SafeDelete(accessor);
    SafeDelete(menu);
    if (inspected && inspected->IsObject()) 
	inspected->GetObject()->RemoveObserver(this);
}

void ObjectView::SetInspected(Ref *newinsp)
{
    Ref *oldref= inspected;
    if (inspected && inspected->IsObject()) 
	inspected->GetObject()->RemoveObserver(this);

    inspected= newinsp;
    if (inspected) {
	Build();
	if (inspected->IsObject())
	    inspected->GetObject()->AddObserver(this);
    }
    UpdateViews();      // always update values
}
  
void ObjectView::DoObserve(int, int part, void*, ObjPtr op)
{
    if (op && inspected && (op == inspected->GetBase())) {
	if (part != cPartSenderDied) 
	    UpdateViews();
    }
}

static void SecureDied(void *p1, void *, void*, void*)
{
    Collection *cp= (Collection*)p1;
    cp->ForEach(InspectorItem0,Died)();
}

void ObjectView::InspectedDied()
{
    SetInspected(0);
    // ??? what if an InspectorItem died ???
    if (gSystem->Call(SecureDied, (void*) GetCollection(), 0, 0, 0)) 
	return;
    ForceRedraw();
}

void ObjectView::DoSelect(Rectangle r, int)
{
    InspectorItem0 *di= At(r.origin.y);
    if (di && inspected) 
	di->Deref(shifted);
}

Command *ObjectView::DispatchEvents(Point p, Token t, Clipper *cl)
{
    if (inspected)
	return CollectionView::DispatchEvents(p, t, cl);
    return gNoChanges;
}

Command *ObjectView::DoLeftButtonDownCommand(Point p, Token t, int cl)
{
    shifted= t.Flags == eFlgShiftKey;
    return CollectionView::DoLeftButtonDownCommand(p, t, cl);  
}

void ObjectView::Add(InspectorItem0 *ii)
{
    GetCollection()->Add(ii);
}

void ObjectView::Build()
{
    GetCollection()->FreeAll();
    if (inspected == 0)
	return;
    if (inspected->IsObject()) 
	BuildInstanceView();    
    else if (inspected->GetType() & (T_VEC | T_ARR))
	BuildArrayView();
    else 
	Add(new InspectorItem("simple", new Ref(*inspected)));
}

void ObjectView::BuildInstanceView()
{
    if (inspected->GetObject() == gApplication)
	gApplication->UpdateGlobals();
    accessor->Reset(inspected->GetObject(), this);
    inspected->GetObject()->IsA()->EnumerateMembers(accessor);
}

void ObjectView::BuildArrayView()
{
    int size, i, off, l, tt= inspected->GetType();
    size= sizeof(void*); // ????
    if ((inspected->GetType() & T_VEC) && (inspected->GetLen() == -1)) {
	cerr << "TVAR0" NL;
	void **lp= (void**) inspected->GetObject();
	for (i= 0; lp[i]; i++)
	    ;
	inspected->SetLen(i);
    }
    l= inspected->GetLen();

    tt&= ~(T_VEC+T_ARR);
    if (inspected->GetClass() && tt == 0)
	size= inspected->GetClass()->Size();
    for (i= 0, off= 0; i < min(l, cMaxItems); i++, off+= size)
	Add(new InspectorItem(form("[%d]", i), 
		new Ref(inspected->GetBase(), tt, off, 0, inspected->GetClass())));
}

static void SecureUpdate(void *p1, void *, void*, void*)
{
    Collection *cp= (Collection*)p1;
    cp->ForEach(InspectorItem0,Update)();
}

void ObjectView::UpdateViews()
{
    if (gSystem->Call(SecureUpdate, (void*) GetCollection(), 0, 0, 0)) 
	return;
    SetSelection(gRect0);
    Modified();
}

void ObjectView::DoCreateMenu(Menu *menu)
{
    menu->AppendItems("edit definition",    cEDITDECL,
		      "edit implementation",cEDITIMPL,
		      "references",         cOBJREFERENCES,
		      "abstract view",      cABSTRVIEW,
		      "object structure",   cOBJBROWSER,
		      "spawn",              cSPAWN,
		      "print ...",          cPRINT,
		      0);
}

void ObjectView::DoSetupMenu(Menu *menu)
{
    if (inspected) {
	char *viewname= gProgEnv->HasAbstractView(inspected->GetClass(), 
						  inspected->GetBase(),
						  inspected->GetObject());
	if (viewname) {
	    menu->ReplaceItem(cABSTRVIEW, viewname);
	    menu->EnableItem(cABSTRVIEW);
	}
	if (inspected->IsObject()) 
	    menu->EnableItems(cOBJREFERENCES, cOBJBROWSER, cEDITDECL, cEDITIMPL, 0);
    }
    menu->EnableItems(cPRINT, cSPAWN, 0);
}

Command *ObjectView::DoMenuCommand(int cmd)
{
    if (cmd == cABSTRVIEW)
	gProgEnv->ShowAbstractView(inspected->GetClass(), inspected->GetBase(), 
				   inspected->GetObject(), this);
    else
	return View::DoMenuCommand(cmd);
    return gNoChanges;
}

bool ObjectView::PrintOnWhenObserved(Object *)
{
    return FALSE;
}
