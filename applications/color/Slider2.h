#ifndef Slider2_First
#define Slider2_First

#include "Slider.h"

//---- Thumb2 -------------------------------------------------------------------

class Thumb2: public VObject {
    class Slider2 *slider;
    GrCursor cursor;
public:
    MetaDef(Thumb2);
    Thumb2(Slider2 *sl, GrCursor c);
    void Draw(Rectangle r);
    Metric GetMinSize();
    Command *DoLeftButtonDownCommand(Point, Token, int);
    GrCursor GetCursor(Point);
};

class Slider2: public CompositeVObject {
    Point minVal, maxVal, currVal;

public:
    MetaDef(Slider2);
    Slider2(int id, Point max, Point min= gPoint0, Direction dir= eVert);

    class Thumb2 *GetThumb()
	{ return (Thumb2*) At(0); }
    void Update(bool redraw= TRUE);
    void SetOrigin(Point);
    void Draw(Rectangle);
    void SetVal(Point val, bool redraw= FALSE);
    Point GetVal()
	{ return currVal; }
    void SetMax(Point max, bool redraw= FALSE);
    void SetMin(Point min, bool redraw= FALSE);
    void MoveThumb(Point);
    Metric GetMinSize();
    void Control(int id, int part, void *val);
};

#endif Slider2_First

