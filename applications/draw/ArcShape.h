#ifndef ArcShape_First
#define ArcShape_First

#include "OvalShape.h"

//---- Arc Shapes -------------------------------------------------------------

class ArcShape : public OvalShape {
public:
    MetaDef(ArcShape);

    ArcShape();
    void Init(Point p1, Point p2);
    short *GetImage();
    Point Chop(Point);
    Point GetConnectionPoint(Point);
    bool CanSplit();
    Rectangle RealRect(Rectangle rr, int s);
};

#endif ArcShape_First
