#ifndef Slider_First
#ifdef __GNUG__
#pragma once
#endif
#define Slider_First

#include "VObject.h"

typedef enum {
    eSliderPageUpLeft   = 1,
    eSliderPageDownRight= 2,
    eSliderThumb        = 3
} SliderPartCodes;

//---- Slider ------------------------------------------------------------------

class Slider: public CompositeVObject {
    Point minVal, maxVal, currVal, bubbleSize;

public:
    MetaDef(Slider);
    Slider(int id= cIdNone, Direction dir= eVert);

    class Thumb *GetThumb()
	{ return (Thumb*) At(0); }
    void Update(bool redraw= TRUE);
    void SetOrigin(Point);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void Draw(Rectangle);
    void SetVal(Point, bool redraw= FALSE);
    Point GetVal()
	{ return currVal; }
    void SetMax(Point, bool redraw= FALSE);
    Point GetMax()
	{ return maxVal; }
    void SetMin(Point, bool redraw= FALSE);
    Point GetMin()
	{ return minVal; }
    void SetThumbRange(Point, bool redraw= FALSE);
    GrCursor GetCursor(Point);
    void MoveThumb(Point);
    int Where(Point);
    Metric GetMinSize();
};

//---- Thumb -------------------------------------------------------------------

class Thumb: public VObject {
    Slider *slider;
    GrCursor cursor;
public:
    MetaDef(Thumb);
    Thumb(Slider *sl, GrCursor c);
    void Draw(Rectangle r);
    Metric GetMinSize();
    Command *DoLeftButtonDownCommand(Point, Token, int);
    GrCursor GetCursor(Point);
};

//---- PagerCommand ------------------------------------------------------------

class PagerCommand: public Command {
    Slider *slider;
    int part;
public:
    PagerCommand(Slider* s, int p);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

#endif Slider_First

