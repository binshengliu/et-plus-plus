#ifndef Expander_First
#ifdef __GNUG__
#pragma once
#endif
#define Expander_First

#include "VObject.h"

//---- Expander --------------------------------------------------------------

class Expander: public CompositeVObject {
protected:
    Direction dir;
    Point gap;
public:
    MetaDef(Expander);
    Expander(int id, Direction d, Point g, VObject *vop, ...);
    Expander(int id, Direction d, Point g= gPoint0, Collection *c= 0);
   
    void SetExtent(Point e);
    void SetOrigin(Point at);
    Metric GetMinSize();
    int ExpandCnt();
};

#endif Expander_First

