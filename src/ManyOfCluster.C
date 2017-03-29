//$ManyOfCluster$
 
#include "ManyOfCluster.h"
#include "Buttons.h"
#include "OrdColl.h"

//---- ManyOfCluster -----------------------------------------------------------

MetaImpl0(ManyOfCluster);

ManyOfCluster::ManyOfCluster(int id, VObjAlign a, Point g, Collection *cp)
						: Cluster(id, a, g, cp)
{
    SetFlag(eVObjVFixed);
}

ManyOfCluster::ManyOfCluster(int id, VObjAlign a, Point g, VObject *va_(vop), ...)
						: Cluster(id, a, g, (Collection*)0)
{
    va_list ap;
    va_start(ap,va_(vop));
    Add(va_(vop));
    SetItems(ap);
    va_end(ap);
    SetFlag(eVObjVFixed);
}

ManyOfCluster::ManyOfCluster(int id, VObjAlign a, Point g, char *va_(cp), ...)
					       : Cluster(id, a, g, (Collection*)0)
{
    char *t;
    va_list ap;
    va_start(ap,va_(cp));
    list= new OrdCollection;
    for (int i= 0; ; i++) {
	if (i == 0)
	    t= va_(cp);
	else
	    if ((t= va_arg(ap, char*)) == 0)
		break; 
	    
	list->Add(new LabeledButton(id+i, t, FALSE));
    }
    va_end(ap);
    SetFlag(eVObjVFixed);
}

void ManyOfCluster::Control(int id, int p, void *v)
{
    if (p == cPartToggle)
	Cluster::Control(GetId(), id, v);
}

void ManyOfCluster::SetItemOn(int id, bool state)
{
    if (Size() <= 0)
	return;
    Iter next(list);
    VObject *dip;

    while (dip= (VObject*) next()) {
	int did= dip->GetId();
	if (did == id)
	    dip->SendDown(did, cPartSetState, (void*) state);
    }
}
    
void ManyOfCluster::SetAllItemsOn(bool state)
{
    if (Size() <= 0)
	return;
    VObject *dip;
    Iter next(list);
    while (dip= (VObject*) next()) 
	    dip->SendDown(cIdNone, cPartSetState, (void*) state);
}
    
bool ManyOfCluster::IsItemOn(int id)
{
    bool isSet= FALSE;
    if (Size() <= 0)
	return isSet;
    Iter next(list);
    VObject *dip;

    while (dip= (VObject*) next()) {
	int did= dip->GetId();
	if (did == id)
	    dip->SendDown(cIdNone, cPartStateIsOn, &isSet);
    }
    return isSet;
}
