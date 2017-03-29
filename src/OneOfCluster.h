#ifndef OneOfCluster_First
#ifdef __GNUG__
#pragma once
#endif
#define OneOfCluster_First

#include "Cluster.h"

//---- OneOfCluster ------------------------------------------------------------

class OneOfCluster: public Cluster {
public:
    MetaDef(OneOfCluster);
    OneOfCluster(int id= cIdNone, VObjAlign a= eVObjHLeft, Point g= gPoint0, Collection *cp= 0);
    OneOfCluster(int id, VObjAlign a, Point g, VObject*, ...);
    OneOfCluster(int id, VObjAlign a, Point g, char *, ...);
    void Init(int n= 0);
    void SetItemOn(int id);
    int GetCurrentItem();
    void Control(int id, int part, void *v);
};

#endif OneOfCluster_First

