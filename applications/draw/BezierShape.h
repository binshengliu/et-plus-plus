#ifndef BezierShape_First
#define BezierShape_First

#include "PolyShape.h"

//---- Box Shape ---------------------------------------------------------------

class BezierShape : public PolyShape {
public:
    MetaDef(BezierShape);
    
    BezierShape();
	
    short *GetImage();
    ShapeSketcher *NewSketcher(DrawView *dv, SketchModes m);
    void Outline(Point p1, Point p2);
    void Draw(Rectangle);
    void Highlight(HighlightState);
    ShapeStretcher *NewStretcher(DrawView *dv, int handle);
    void MoveAround(int n, Point d, bool redraw= TRUE);
};

#endif BezierShape_First

