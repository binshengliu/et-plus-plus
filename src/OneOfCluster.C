//$OneOfCluster$

#include "OneOfCluster.h"
#include "Buttons.h"
#include "OrdColl.h"

//---- OneOfCluster ------------------------------------------------------------

MetaImpl0(OneOfCluster);

OneOfCluster::OneOfCluster(int id, VObjAlign a, Point g, Collection *cp)
					    : Cluster(id, a, g, cp)
{
    Init(0);
}

OneOfCluster::OneOfCluster(int id, VObjAlign a, Point g, VObject *va_(vop), ...)
					    : Cluster(id, a, g, (Collection*)0)
{
    va_list ap;
    va_start(ap,va_(vop));
    Add(va_(vop));
    SetItems(ap);
    Init(0);
    va_end(ap);
}

OneOfCluster::OneOfCluster(int id, VObjAlign a, Point g, char *va_(cp), ...)
					    : Cluster(id, a, g, (Collection*)0)
{
    char *t;
    va_list ap;
    va_start(ap,va_(cp));
    list= new OrdCollection;
    for (int i= 0; ;i++) {
	if (i == 0)
	    t= va_(cp);
	else
	    if ((t= va_arg(ap, char*)) == 0)
		break;
	list->Add(new LabeledButton(id+i, t));
    }
    Init(0);
    va_end(ap);
}

void OneOfCluster::Init(int n)
{
    if (n >= 0 && n < Size())
	SetItemOn(At(n)->GetId()); 
    SetFlag(eVObjVFixed);
}

void OneOfCluster::SetItemOn(int id)
{
    if (Size() <= 0)
	return;
    Iter next(list);
    VObject *dip;

    // ForceRedraw();
    while (dip= (VObject*) next()) {
	int did= dip->GetId();
	dip->SendDown(did, cPartSetState, (void*) (did == id));
    }
}
    
int OneOfCluster::GetCurrentItem()
{
    if (Size() <= 0)
	return cIdNone;
    Iter next(list);
    VObject *dip;
    
    bool isSet= FALSE;
    while (dip= (VObject*) next()) {
	dip->SendDown(cIdNone, cPartStateIsOn, &isSet);
	if (isSet)
	    return dip->GetId();
    }
    return cIdNone;
}

void OneOfCluster::Control(int id, int part, void *v)
{
    if (part == cPartToggle) {
	SetItemOn(id);
	Cluster::Control(GetId(), id, v);
    }
}
