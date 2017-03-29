//$ClassListView$
 
#include "ClassList.h"
#include "ClassItem.h"
#include "Menu.h"
#include "EtPeCmdNo.h"

#include "OrdColl.h"
#include "ObjList.h"
#include "ClassManager.h"
#include "ObjectTable.h"
#include "String.h"


//---- ClassListView ---------------------------------------------------------------

MetaImpl(ClassListView, (TP(classItems), T(hideEmptyClasses), T(showInstCount), 0));

ClassListView::ClassListView(EvtHandler *b, bool count) 
				    : CollectionView(b, 0, eCVDontStuckToBorder)
{
    hideEmptyClasses= FALSE;
    showInstCount= count;
    menu= new Menu("Classes");
    LoadClasses();   
}

ClassListView::~ClassListView()
{
}
 
void ClassListView::LoadClasses()
{
    Iter next(gClassManager->Iterator());
    Class *clp;
    VObject *vop;
    
    if (showInstCount) 
	ObjectTable::UpdateInstCount();

    classItems= new OrdCollection(200); 

    while (clp= (Class*) next()) {
	if (!clp->TestFlag(eClassObject))
	    continue;
	if (!(hideEmptyClasses && clp->GetInstanceCount() == 0)) {
	    if (showInstCount)
		vop= new ClassItem(cIdNone, clp, 
			  form("%s (%d)", clp->Name(), clp->GetInstanceCount()));
	    else 
		vop= new ClassItem(cIdNone, clp);
	    classItems->Add(vop);
	}
    }
    classItems->Sort();
    SetCollection(classItems);
    Update();    
}

void ClassListView::DoSelect(Rectangle r, int)
{
    if (r.IsEmpty())
	return;
    ClassItem *ci= (ClassItem*) classItems->At(r.origin.y);
    if (ci == 0)
	return; 
    Control(cIdCLChangedClass, 0, ci->GetClass());    
}

void ClassListView::DoObserve(int, int, void*, Object *op)
{
    if (op == gClassManager) {
	classItems->Empty();
	LoadClasses();
	SelectClass(0);
    }
}

void ClassListView::ToggleHideEmpty()
{
    hideEmptyClasses= !hideEmptyClasses;
    LoadClasses();
}

bool ClassListView::SelectClass(Class *cl)
{
    if (cl == 0) {
	SetNoSelection();
	return FALSE;
    }
    Rectangle r, sr;
    ClassItem *ci= (ClassItem*) classItems->At(GetSelection().origin.y);
    if (ci && ci->GetClass() == cl)
	return FALSE;
    ClassItem *clp= 0, searchItem(0, cl);

    int pos= classItems->BinarySearch(&searchItem);
    if (pos != -1)
	clp= (ClassItem*) classItems->At(pos);
    if (clp) {
	sr= Rectangle(0, pos, 1, 1);
	SetSelection(sr);
	RevealAlign(ItemRect(sr));
    } 
    return TRUE; 
}

Class *ClassListView::SelectedClass()
{
    Rectangle r, sr;
    ClassItem *ci= (ClassItem*) classItems->At(GetSelection().origin.y);
    if (ci)
	return ci->GetClass();
    return 0;
}
