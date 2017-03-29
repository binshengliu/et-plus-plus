#ifndef DynShape2_First
#define DynShape2_First

#include "DynShape.h"

//---- DynShape2 ---------------------------------------------------------------

class DynShape2: public DynShape {
public:
    MetaDef(DynShape2);
    
    DynShape2();
    void Draw(Rectangle);
    void Outline(Point, Point);
};

#endif DynShape2_First


