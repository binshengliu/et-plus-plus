#ifndef RcBoxShape_First
#define RcBoxShape_First

#include "BoxShape.h"

//---- Rounded Corner Box Shape ------------------------------------------------

class RcBoxShape : public BoxShape {
    Point rcdia;
public:
    MetaDef(RcBoxShape);
    
    RcBoxShape();
    short *GetImage();
    void SetSpan(Rectangle r);
    void Draw(Rectangle);
    void Outline(Point, Point);
    bool ContainsPoint(Point);
    Point Chop(Point);
    void SetDia(Point, bool redraw= TRUE);
    Point GetDia()
	{ return rcdia; }
    Point *MakeHandles(int *n);
    ShapeStretcher *NewStretcher(DrawView *dv, int handle);
    ostream& PrintOn(ostream&);    
    istream& ReadFrom(istream&);
};

#endif RcBoxShape_First

