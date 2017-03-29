#ifndef OvalShape_First
#define OvalShape_First

#include "Shape.h"

//---- Oval Shapes -------------------------------------------------------------

class OvalShape : public Shape {
protected:
    short startangle, anglen;

public:
    MetaDef(OvalShape);

    OvalShape();
    void Flip(int n);
    short *GetImage();
    void SetAngles(Point, Point, short&, short&);
    Point Chop(Point);
    virtual Rectangle RealRect(Rectangle rr, int s);
    Rectangle InvalRect();
    void Draw(Rectangle);
    void Outline(Point, Point);
    bool ContainsPoint(Point);
    Point *MakeHandles(int *);
    ShapeStretcher *NewStretcher(class DrawView*, int handle);
    bool CanSplit();
    void SetAngle(bool, int);
    int GetAngle(bool);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

class WedgeStretcher: public ShapeStretcher {
    OvalShape *op;
    Point anglept, angpt;
    bool where;
    int oldangle, newangle;
public:
    WedgeStretcher(DrawView*, Shape*, int, bool); 
    void DoIt();
    void UndoIt();
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool);
    void TrackConstrain(Point, Point, Point*);
};

extern float Len(Point rad, Point p);

#endif OvalShape_First
