#ifndef Connection_First
#define Connection_First

#include "LineShape.h"

//---- Connection --------------------------------------------------------------

class Connection : public LineShape {
    Shape *startShape, *endShape;
    Point startCenter, endCenter;
public:
    MetaDef(Connection);
    
    Connection(View *v, Shape *s1= 0, Shape *s2= 0);
    ~Connection();
    
    void UpdatePoints();
    void DoObserve(int, int, void*, Object*);
    Rectangle GetTextRect();
    void Highlight(HighlightState);
    ShapeStretcher *NewStretcher(class DrawView*, int);
    int PointOnHandle(Point);
    bool ContainsPoint(Point);
    void Moveby(Point delta);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

#endif Connection_First
