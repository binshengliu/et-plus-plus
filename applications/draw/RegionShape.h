#ifndef RegionShape_First
#define RegionShape_First

#include "PolyShape.h"

//---- RegionShape and Sketcher ------------------------------------------------

class RegionShape : public PolyShape {
public:
    MetaDef(RegionShape);
    
    RegionShape();
    short *GetImage();
    GrCursor SketchCursor();
    ShapeSketcher *NewSketcher(class DrawView*, SketchModes);
};

class RegionSketcher: public ShapeSketcher {
    Point lastp;
public:
    RegionSketcher(class DrawView*, Shape*, SketchModes);
    void TrackFeedback(Point, Point pp, bool turniton);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

#endif RegionShape_First

