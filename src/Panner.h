#ifndef Panner_First
#ifdef __GNUG__
#pragma once
#endif
#define Panner_First

//---- Panner ------------------------------------------------------------------

#include "Slider.h"
 
class Panner : public Slider {
    class Clipper *cl;
public:
    Panner(Clipper *mc, Point extent= gPoint_1);
    ~Panner();
    void Control(int id, int part, void *val);
    void DoObserve(int id, int part, void *vp, ObjPtr op);
    Metric GetMinSize();
};


#endif Panner_First

