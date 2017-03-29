#ifndef EnumItem_First
#ifdef __GNUG__
#pragma once
#endif
#define EnumItem_First

#include "Cluster.h"

//---- EnumItem ----------------------------------------------------------------

class EnumItem: public Cluster {
public:
    MetaDef(EnumItem);
    EnumItem(int id= cIdNone, VObjAlign a= eVObjVBase, VObject *ci= 0, Point gap= gPoint10);
    void Control(int, int, void*);
};

#endif EnumItem_First


