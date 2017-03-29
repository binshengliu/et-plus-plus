#ifndef MenuItems_First
#define MenuItems_First

#include "DialogItems.h"

//---- PatternMenuItem ---------------------------------------------------------

class PatternItem : public VObject {
    Ink *ink;
public:
    PatternItem(int id, Ink *p) : VObject(id)
	{ ink= p; }
    void Draw(Rectangle);
    Metric GetMinSize()
	{ return Metric(40, 20); }
};

//---- LineStyleItem ----------------------------------------------------------

class LineStyleItem : public LineItem {
    GrLineCap cap;
    int width;
public:
    LineStyleItem(int id, int w= 1, GrLineCap lc= eDefaultCap)
						    : LineItem(TRUE, w, 20, id)
	{ cap= lc; width= w; }
    void Draw(Rectangle);
    Metric GetMinSize()
	{ return Metric(Point(40, max(15, width+4))); }
};

#endif MenuItems

