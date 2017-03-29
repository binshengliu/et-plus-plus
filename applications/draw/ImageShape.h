#ifndef ImageShape_First
#define ImageShape_First

#include "Shape.h"

//---- Image Shape --------------------------------------------------------------

class ImageShape : public Shape {
    Bitmap*bm;
public:
    MetaDef(ImageShape);
    
    ImageShape(Bitmap *b);
    ~ImageShape();
    
    void Draw(Rectangle);
    void Outline(Point p1, Point p2);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

#endif ImageShape_First

