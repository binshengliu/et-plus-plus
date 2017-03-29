#ifndef TextShape_First
#define TextShape_First

#include "Shape.h"

//---- Text Shape Commands -----------------------------------------------------

class TextShapeSketcher: public ShapeSketcher {
    int lineHeight, baseHeight;
public:
    TextShapeSketcher(class DrawView*, class Shape *pro, SketchModes);
    void TrackConstrain(Point, Point, Point *np);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

class TextShapeStretcher: public ShapeStretcher {
public:
    TextShapeStretcher(class DrawView*, class Shape *pro, int sq);
    void TrackConstrain(Point, Point, Point *np);
};

//---- Text Shape --------------------------------------------------------------

class TextShape : public Shape {
    class TextView *to;
    Shape *attachedTo;     // shape i am attached to
    bool captured;         // is the text captured ?? (otherwise paragraphed)
    static FontPtr defaultFont;
public:
    MetaDef(TextShape);
    
    TextShape();
    ~TextShape();
    
    void Init(Point, Point);

    bool IsGarbage();
    void SetContainer(VObject *vop);
    short *GetImage();
    GrCursor SketchCursor();

    Rectangle InvalRect();    // expand to make caret visible
    void InvalDiff(Rectangle r);
    void Draw(Rectangle);
    void Outline(Point p1, Point p2);
    void Highlight(HighlightState);
    class TextView *GetTextView()
	{ return to; }
    
    //---- properties ----------------------------------------
    void SetFont(class RunArray*);
    void ApplyFont(StChangeStyle, StyleSpec);
    void ResetProperty(ShapeProperties, Object*);
    void SetProperty(ShapeProperties, Object*);
    Object* GetProperty(ShapeProperties);
    void SetSimpleProperty(ShapeProperties, int);
    int GetSimpleProperty(ShapeProperties);

    ShapeSketcher *NewSketcher(DrawView*, SketchModes);
    class ShapeStretcher *NewStretcher(DrawView*, int);

    void SetSpan(Rectangle);
    void Moveby(Point delta);
    
    //---- observing -----------------------------------------
    void MakeDependentOn(Shape*);
    void DoObserve(int id, int p, void*, Object*);

    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
    
    friend FontPtr TextShapeDefaultFont();
};

#endif TextShape_First
