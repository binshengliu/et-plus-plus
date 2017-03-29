//$CollTableView,DictionaryView$

#include "CollTblView.h"
#include "ClassItem.h"
#include "ObjectTable.h"
#include "OrdColl.h"
#include "ObjArray.h"
#include "Collection.h"
#include "Alert_e.h"
#include "String.h"
#include "Dictionary.h"

//---- CollTableView -----------------------------------------------------------

CollTableView::CollTableView(int col)
	   : CollectionView(0, 0, (CollViewOptions)(eCVDefault+eCVGrid), 0, col)
{
}

ObjectItem *CollTableView::AsObjectItem(Object *op)
{
    char buf[200], label[1000];
    op->InspectorId(buf, sizeof buf);
    
    sprintf(label, "0x%x %-14.14s: %s", (int) op, form("<%s>",op->ClassName()), buf);
    return new ObjectItem(cIdNone, label, op);
}

void CollTableView::ShowCollection(Collection* col)
{
    if (col->Size() == 0)
	return;
    Iter next(col);
    OrdCollection *oc= new OrdCollection;
    Object *op; 
    
    while (op= next()) 
	oc->Add(AsObjectItem(op));
    SetCollection(oc);
    Update();
    UpdateEvent();
}

void CollTableView::DoSelect(Rectangle r, int)
{
    Object *op, *sel= GetCollection()->At(r.origin.y);
    op= Guard(sel, ObjectItem)->GetObject();
    if (ObjectTable::PtrIsValid(op))
	op->Inspect();
    else
	ShowAlert(eAlertNote, "instance 0x%x disappeard", op);
}

//---- DictionaryView -----------------------------------------------------------

DictionaryView::DictionaryView() : CollTableView(2)
{
}

void DictionaryView::ShowCollection(Collection *col)
{
    Dictionary *dict= Guard(col, Dictionary);
    if (dict->Size() == 0)
	return;
    Iter next(col);
    ObjArray *oc= new ObjArray(dict->Size()*2);
    Assoc *ap; 
    
    for (int i= 0; ap= (Assoc*)next(); i++) {
	oc->AtPutAndExpand(i*2,   AsObjectItem(ap->Key()));
	oc->AtPutAndExpand(i*2+1, AsObjectItem(ap->Value()));
    }    
    SetCollection(oc);
    Update();
    UpdateEvent();
}
