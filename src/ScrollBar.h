#ifndef ScrollBar_First
#ifdef __GNUG__
#pragma once
#endif
#define ScrollBar_First

#include "Expander.h"

class ScrollBar: public Expander {
    class Slider *slider;
public:
    MetaDef(ScrollBar);
    ScrollBar(int id= cIdNone, Direction d= eHor);
    
    void Draw(Rectangle r);
    void ViewSizeChanged(Point);
    void Init(Point e, Point sz, Point vsz);
    void Control(int id, int part, void *val);
    void BubbleUpdate(Point);
    void SetThumbRange(Point);
};

#endif ScrollBar_First

