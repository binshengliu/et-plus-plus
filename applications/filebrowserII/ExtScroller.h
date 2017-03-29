#ifndef ExtScroller_First
#define ExtScroller_First

#include "Scroller.h"
#include "ScrollBar.h"

class ExtScrollBar: public ScrollBar {
public:
    MetaDef(ExtScrollBar);
    ExtScrollBar(class Menu *m, Direction d= eHor);
};

class ExtScroller: public Scroller {
protected:
public:
    MetaDef(ExtScroller);
    ExtScroller(class Menu *m, VObject *v= 0, Point e= gPoint0, int id= cIdNone,
		    ScrollDir= (ScrollDir)eScrollDefault);
};

#endif ExtScroller_First

