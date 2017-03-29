#ifndef LineShape_First
#define LineShape_First

#include "Shape.h"

//---- Line Shape and Sketcher -------------------------------------------------

class LineShape : public Shape {
    Point p1, p2;
    
public:
    MetaDef(LineShape);
    
    LineShape();
    void Init(Point pp1, Point pp2);
    void SetSpan(Rectangle);
    Rectangle GetSpan();
    void Flip(int n);
    void Moveby(Point delta);

    void Draw(Rectangle);
    void Outline(Point, Point);
    Rectangle InvalRect();

    bool ContainsPoint(Point);

    Point *MakeHandles(int*);
    int PointOnHandle(Point);
    bool PointAtLine(Point, Point, Point);

    short *GetImage();
    Point GetConnectionPoint(Point);
    Point Chop(Point);

    Rectangle GetTextRect();

    ostream& PrintOn(ostream&);    
    istream& ReadFrom(istream&);
};

#endif LineShape_First
