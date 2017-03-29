#ifndef ManyOfCluster_First
#ifdef __GNUG__
#pragma once
#endif
#define ManyOfCluster_First

#include "Cluster.h"
 
//---- ManyOfCluster -----------------------------------------------------------

class ManyOfCluster: public Cluster {
    int bits;
public:
    MetaDef(ManyOfCluster);
    ManyOfCluster(int id= cIdNone, VObjAlign a= eVObjHLeft, Point g= gPoint0, Collection *cp= 0);
    ManyOfCluster(int id, VObjAlign a, Point g, char *, ...);
    ManyOfCluster(int id, VObjAlign a, Point g, VObject *, ...);
    void Control(int id, int part, void *v);

    void SetItemOn(int id, bool state= TRUE);
    void SetItemOff(int id)
	{  SetItemOn(id, FALSE); }
    void SetAllItemsOn(bool state= TRUE);
    void SetAllItemsOff()
	{  SetAllItemsOn(FALSE); }
    bool IsItemOn(int id);
};

#endif ManyOfCluster

