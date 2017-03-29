#ifndef BoxShape_First
#define BoxShape_First

#include "Shape.h"

//---- Box Shape ---------------------------------------------------------------

class BoxShape : public Shape {
public:
    MetaDef(BoxShape);
    
    BoxShape();
	
    short *GetImage();
    void Draw(Rectangle);
    void Outline(Point p1, Point p2);
    bool ContainsPoint(Point p);
};

#endif BoxShape_First

