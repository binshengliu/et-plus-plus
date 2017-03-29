#ifndef Shapes_First
#define Shapes_First

#include "Command.h"
#include "Globals.h"
#include "View.h"
#include "StyledText.h"

extern Point HandleSize;
extern Point HandleSize2;
extern Point HandleSize3;
extern Point MinShapeSize;

enum ShapeProperties {
    eShapePattern,
    eShapePenPattern,
    eShapePensize,
    eShapeArrows,
    eShapeSmooth,
    eShapeAdjust,
    eShapeSpacing,
    eShapeTextSize,
    eShapeTextFont,
    eShapeTextFace
};

extern class Ink *InkPalette[];

//---- DrawCommand -------------------------------------------------------------

class DrawCommand : public Command {
    static GrCursor oldcursor;
protected:
    bool moved;
    Point delta;
    class DrawView *view;
    class ObjList *saveselection, *saveselection2;
    
public:
    MetaDef(DrawCommand);
    DrawCommand(DrawView *dv, int cmd= 0, char *cmdname= 0);
    ~DrawCommand();

    void TrackConstrain(Point, Point, Point *);
    Command *TrackMouse(TrackPhase, Point, Point, Point);

    void DoIt();
    void RedoIt();
    void UndoIt();
    
    virtual void SaveDoIt();
    virtual void RestoreRedoIt();
    virtual void RestoreUndoIt();
};

//---- ShapeSketcher -----------------------------------------------------------

enum SketchModes { 
    eSMSquare       = BIT(0),
    eSMShowSizes    = BIT(1),
    eSMCenter       = BIT(2),
    eSMDefault      = 0
};

class ShapeSketcher: public DrawCommand {
    SketchModes mode;
protected:
    class Shape *proto, *newshape;
    
public:
    ShapeSketcher(DrawView*, class Shape *pro, SketchModes m= eSMDefault);
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
    void TrackConstrain(Point, Point, Point *np);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point, Point, bool);
};

//---- Shape -------------------------------------------------------------------

enum ShapeFlags {
    eShapeSplit    =   BIT(eObjLast+1),
    eShapeLast     =   eObjLast + 5
};

class Shape : public Object {
protected:
    static Point spts[10];
    int ink, penink;
    VObject *container;
    short pensize;
    GrLineCap arrows;
    bool isDeleted, isEnabled;
public:
    Rectangle bbox;
    
public:
    MetaDef(Shape);
    
    Shape();
    ~Shape();
    
    void Init1();
    DrawView *GetView();
    VObject *GetContainer()
	{ return container; }
    virtual void SetContainer(VObject *vop);

    //---- sizes ---------------------------------------
    virtual void Init(Point p1, Point p2);
    virtual void SetSpan(Rectangle);
    virtual Rectangle GetSpan();
    virtual void Moveby(Point delta);
    virtual void Flip(int);
    
    //---- drawing -------------------------------------
    void DrawAll(Rectangle);
    virtual void Draw(Rectangle);
    virtual void Outline(Point, Point);
    virtual void Highlight(HighlightState);
    void Invalidate();
    virtual Rectangle InvalRect();
    bool IsInRect(Rectangle *r)
	{ return (bool) (!isDeleted && InvalRect().Intersects(*r)); }
    virtual GrCursor SketchCursor();
    
    //---- interaction ---------------------------------
    virtual ShapeSketcher *NewSketcher(DrawView*, SketchModes);
    virtual class ShapeStretcher *NewStretcher(DrawView*, int);
    bool ContainsPoint1(Point);
    virtual bool ContainsPoint(Point);
    
    //---- handles -------------------------------------
    virtual Point *MakeHandles(int*);
    virtual void HighlightHandle(int);
    virtual int PointOnHandle(Point p);

    //---- properties ----------------------------------
    virtual short *GetImage();
    void SetSplit(bool b);
    bool GetSplit()
	{ return TestFlag(eShapeSplit); }
    virtual bool CanSplit();
    void SetDeleted(bool);
    bool GetDeleted()
	{ return isDeleted; }
    virtual bool IsGarbage();
    
    virtual void SetProperty(ShapeProperties, Object *op);
    virtual void ResetProperty(ShapeProperties, Object *op);
    virtual Object *GetProperty(ShapeProperties);
    
    virtual void SetSimpleProperty(ShapeProperties, int);
    virtual void ResetSimpleProperty(ShapeProperties, int);
    virtual int GetSimpleProperty(ShapeProperties);
    
    virtual Point GetConnectionPoint(Point);
    virtual Point Chop(Point);
    
    //---- text properties ----------------------------
    virtual Rectangle GetTextRect();        
    
    //---- input/output -------------------------------
    virtual ostream& PrintOn(ostream&);
    virtual istream& ReadFrom(istream&);
};

//---- Change Property Command -------------------------------------------------

class ChangePropertyCommand: public DrawCommand {
protected:
    int cnt;
public:
    ChangePropertyCommand(DrawView*, int, char*);
    virtual void SaveProperty(Shape*, int);
    virtual void RestoreProperty(Shape*, int);
    virtual void SetProperty(Shape*, int);
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- Shape Stretcher ---------------------------------------------------------

class ShapeStretcher: public ChangePropertyCommand {
    Rectangle bbox, bbox1, bboxn, theBbox, allBbox;
    Point p1, p2;
    Rectangle *oldSpans;
    float sx, sy;
    int flip;

protected:
    class Shape *sp;
    int handle;

public:
    ShapeStretcher(DrawView *view, Shape*, int);
    ~ShapeStretcher();

    void SaveProperty(Shape*, int);
    void RestoreProperty(Shape*, int);
    void SetProperty(Shape*, int);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackConstrain(Point, Point, Point*);
    void TrackFeedback(Point, Point, bool);
};

#endif Shapes_First

