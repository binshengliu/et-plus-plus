#ifndef PictureShape_First
#define PictureShape_First

#include "Shape.h"

//---- Image Shape --------------------------------------------------------------

class PictureShape : public Shape {
    class Picture *pict;
public:
    MetaDef(PictureShape);
    
    PictureShape(Picture *p);
    ~PictureShape();
    
    void Draw(Rectangle);
    void Outline(Point p1, Point p2);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

#endif PictureShape_First

