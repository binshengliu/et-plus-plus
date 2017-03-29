#ifndef PolyShape_First
#define PolyShape_First

#include "LineShape.h"

//---- Polygons ----------------------------------------------------------------

class PolyShape : public Shape {
protected:
public:
    GrPolyType type;
    int npts, size;
    Point *pts;

public:
    MetaDef(PolyShape);
    
    PolyShape();
    ~PolyShape();
    
    int AddPt(Point ps);    
    short *GetImage();
    Point *GetPts()
	{ return pts; }
    Point GetPt(int i);
    int GetPtCnt()
	{ return npts; }
    void SetSpan(Rectangle r);
    void CalcBBox();
    void MovePoint(int, Point, bool redraw= TRUE);
    void Draw(Rectangle);
    void Outline(Point, Point);
    bool ContainsPoint(Point p);
    void StrokePartial(int s, int l);
    Point *MakeHandles(int *);
    ShapeSketcher *NewSketcher(DrawView*, SketchModes);
    ShapeStretcher *NewStretcher(DrawView*, int handle);
    void ScalePts(int, Point*, Point*, Point, Point);
    bool CanSplit();
    ostream& PrintOn(ostream&);    
    istream& ReadFrom(istream&);
};

class PolySketcher: public ShapeSketcher {
    Point firstp;
public:
    PolySketcher(DrawView*, Shape*, SketchModes);
    void TrackFeedback(Point, Point pp, bool turniton);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    PolyShape *Poly()
	{ return ((PolyShape*) newshape); }
};

class SplineStretcher: public ShapeStretcher {
    class PolyShape *lsp;
public:
    SplineStretcher(DrawView*, Shape*, int);
    void DoIt();
    void UndoIt();
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool);
    void TrackConstrain(Point, Point, Point*);
};

#endif PolyShape_First
