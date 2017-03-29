#ifndef Commands_First
#define Commands_First

#include "Shape.h"

class ShapeSelector : public DrawCommand {
public:
    ShapeSelector(DrawView *dv) : DrawCommand(dv)
	{ }
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

//---- Group Command ------------------------------------------------------------

class GroupCommand: public DrawCommand {
    class Group *group;
public:
    GroupCommand(DrawView*);
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- Ungroup Command ------------------------------------------------------------

class UngroupCommand: public DrawCommand {
    class Group *group;
    class ObjList *newshapes;
public:
    UngroupCommand(DrawView*, class Group *gp);
    ~UngroupCommand();
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- SCutCopy Command ---------------------------------------------------------

class SCutCopyCommand: public DrawCommand {
public:
    SCutCopyCommand(DrawView*, int cmd, char *name= 0);
    void SaveDoIt();
    void RestoreUndoIt();
};

//---- SPaste Command -----------------------------------------------------------

class SPasteCommand: public DrawCommand {
    class ObjList *newshapes;
public:
    SPasteCommand(DrawView*, class ObjList*, Point at);
    SPasteCommand(DrawView*, Shape*, Point at);
    ~SPasteCommand();
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- Duplicate Command --------------------------------------------------------

class DupCommand: public DrawCommand {
    ObjList *dups;
public:
    Point ddelta;
    
    DupCommand(DrawView*);
    ~DupCommand();
    void Done(Command *nextcmd);
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- FrontBack Command --------------------------------------------------------

class FrontBackCommand: public DrawCommand {
    ObjList *newshapes;
public:
    FrontBackCommand(DrawView*, int cmd, char *s);
    ~FrontBackCommand();
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- Connect Command ----------------------------------------------------------

class ConnectCommand: public DrawCommand {
    class ObjList *connections;
public:
    ConnectCommand(DrawView*dv);
    ~ConnectCommand();
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- CursorMoveCommand --------------------------------------------------------

class CursorMoveCommand: public DrawCommand {
    bool scroll;
protected:
public:
    Point undoDelta, dragDelta;
public:
    CursorMoveCommand(DrawView*, Point d, bool scroll= TRUE);
    void Done(Command*);
    void SaveDoIt();
    void RestoreRedoIt();
    void RestoreUndoIt();
};

//---- Shape Dragger ------------------------------------------------------------

class ShapeDragger: public CursorMoveCommand {
    bool onlyone;
    Rectangle span, bbox;
    Shape *sp;
public:
    ShapeDragger(DrawView*, Shape *);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackFeedback(Point ap, Point pp, bool);
    void TrackConstrain(Point, Point, Point *);
};

//---- Property Commands --------------------------------------------------------

class PropertyCommand: public ChangePropertyCommand {
    Object *newprop;
    class ObjArray *oldprop;
    ShapeProperties what;
public:
    PropertyCommand(DrawView*, ShapeProperties what, Object *prop, char *cmd);
    ~PropertyCommand();
    void SaveProperty(Shape*, int);
    void RestoreProperty(Shape*, int);
    void SetProperty(Shape*, int);
};

class SimplePropertyCommand: public ChangePropertyCommand {
    int newprop, *oldprop;
    ShapeProperties what;
    bool free;
public:
    SimplePropertyCommand(DrawView*, ShapeProperties what, int prop, char *cmd);
    ~SimplePropertyCommand();
    void SaveProperty(Shape*, int);
    void RestoreProperty(Shape*, int);
    void SetProperty(Shape*, int);
};

#endif Commands_First
