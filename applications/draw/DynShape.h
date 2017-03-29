#ifndef DynShape_First
#define DynShape_First

#include "Shape.h"

//---- Dyn Shape ---------------------------------------------------------------

class DynShape: public Shape {
public:
    MetaDef(DynShape);
    
    DynShape();
    short *GetImage();
    void Draw(Rectangle);
    void Outline(Point, Point);
};

#endif DynShape_First


