#ifndef VObject_First
#ifdef __GNUG__
#pragma once
#endif
#define VObject_First

#include "EvtHandler.h"
#include "Command.h"
#include "Collection.h"
#include "CmdNo.h"

enum VObjFlags {
    eVObjEnabled    =   BIT(eEvtLast+1),
    eVObjOpen       =   BIT(eEvtLast+2),
    eVObjHFixed     =   BIT(eEvtLast+3),
    eVObjVFixed     =   BIT(eEvtLast+4),
    eVObjLayoutCntl =   BIT(eEvtLast+5),
    eVObjKbdFocus   =   BIT(eEvtLast+6),
    eVObjDefault    =   eVObjEnabled,
    eVObjLast       =   eEvtLast + 6
};

enum VObjAlign {
    eVObjHLeft  = BIT(0),
    eVObjHCenter= BIT(1),
    eVObjHRight = BIT(2),
    eVObjHExpand= BIT(3),
    eVObjH      = eVObjHLeft|eVObjHCenter|eVObjHRight|eVObjHExpand, 
    eVObjVTop   = BIT(8),
    eVObjVBase  = BIT(9),
    eVObjVCenter= BIT(10),
    eVObjVBottom= BIT(11), 
    eVObjVExpand= BIT(12), 
    eVObjV      = eVObjVTop|eVObjVBase|eVObjVCenter|eVObjVBottom|eVObjVExpand 
};


class VObject : public EvtHandler {
    short id;
    VObject *container;

public:
    Rectangle contentRect;

protected:
    void Init(int, Rectangle, class View*);

public:
    MetaDef(VObject);

    VObject(class EvtHandler *next, Rectangle r, int id= cIdNone);
    VObject(Rectangle r, int id= cIdNone);
    VObject(int id= cIdNone);

    Point GetPortPoint(Point p);
    virtual Rectangle GetViewedRect();
    virtual void AddToClipper(class Clipper*);
    virtual void RemoveFromClipper(class Clipper*);

    //---- acessing ----------------------------
    int GetId()
	{ return id; }
    void SetId(int i)
	{ id= i; }
    bool Enabled()
	{ return TestFlag(eVObjEnabled); }
    virtual void Enable(bool b= TRUE, bool redraw= TRUE);
    void Disable(bool redraw= TRUE)
	{ Enable(FALSE, redraw); }
    bool IsOpen()
	{ return TestFlag(eVObjOpen); }

    //---- sizes -------------------------------
    virtual Metric GetMinSize();
    int Width()
	{ return contentRect.extent.x; }
    int Height()
	{ return contentRect.extent.y; }
    virtual int Base();
    Point GetExtent()
	{ return contentRect.extent; }
    Point GetOrigin()
	{ return contentRect.origin; };
    Rectangle ContentRect()
	{ return contentRect; }

    void SetContentRect(Rectangle, bool);
    virtual void SetExtent(Point);
    virtual void SetOrigin(Point);
    void SetWidth(int w)
	{ SetExtent(Point(w,Height())); }
    void SetHeight(int h)
	{ SetExtent(Point(Width(),h)); }
    void Move(Point delta, bool redraw= TRUE);

    void CalcExtent();
    virtual void Open(bool mode= TRUE);
    void Close()
	{ Open(FALSE); }

    void Align(Point at, Metric m, VObjAlign a);
    virtual bool ContainsPoint(Point p);

    //---- drawing ----------------------------
    virtual void SetFocus(Rectangle, Point);
    void Focus(Rectangle r)
	{ SetFocus(r, gPoint0); }
    void Focus()
	{ SetFocus(GetViewedRect(), gPoint0); }

    void Print();
    virtual void DrawAll(Rectangle, bool highlight= FALSE);
    virtual void DrawInner(Rectangle r, bool highlight= FALSE);
    virtual void DrawHighlight(Rectangle);
    virtual void Draw(Rectangle);
    virtual void Outline2(Point, Point);
    void Outline(Point delta);
	// alternative interface to Outline2
    void OutlineRect(Rectangle r);
	// alternative interface to Outline2
    virtual void Highlight(HighlightState);
    virtual void InvalidateRect(Rectangle r);
    virtual void InvalidateViewRect(Rectangle r);
    void ForceRedraw();
    virtual void UpdateEvent();
    virtual GrCursor GetCursor(Point p);

    virtual Command *Input(Point lp, Token t, Clipper *vf);
    virtual Command *DispatchEvents(Point, Token, Clipper*);
    virtual void DoOnItem(int, VObject*, Point);

    virtual VObject *Detect(BoolFun f, void *arg);
	// find first entry where f returns true
    VObject *FindItem(int);
    VObject *FindItem(Point);
    VObject *FindItem(VObject*);
    VObject *FindItemPtr(VObject *g);

    //---- container handling ------------------
    virtual void SetContainer(VObject*);
    VObject *GetContainer()
	{ return container; }
	
    VObject *FindContainerOfClass(Class *cla);
    class View *GetView();
    class Clipper *Getclipper();
    class BlankWin *GetWindow();
    virtual Point ContainerPoint(Point);

    //---- keyboard focus
    bool WantsKbdFocus()
	{ return TestFlag(eVObjKbdFocus); }
	
    virtual Command *GetMover();
    virtual Command *GetStretcher();
    
    EvtHandler *GetNextHandler();
    virtual Command *DoRightButtonDownCommand(Point, Token, int, class Clipper*);
    virtual Command *DoLeftButtonDownCommand(Point, Token, int);
    virtual Command *DoMiddleButtonDownCommand(Point, Token, int);
    
    virtual Command *DoKeyCommand(int, Point, Token);
    virtual Command *DoCursorKeyCommand(EvtCursorDir, Point, Token);
    virtual Command *DoFunctionKeyCommand(int, Point, Token);
    
    virtual Command *DoOtherEventCommand(Point, Token);
    virtual Command *TrackInContent(Point, Token, Command*);

    //---- generic methods --------------------
    char *AsString();
    int Compare (Object*);
    bool IsEqual (Object*);
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

typedef VObject* VObjPtr;

//---- VObjectCommand ----------------------------------------------------------

class VObjectCommand: public Command {
protected:
    VObject *vop;
    Rectangle oldRect, lastRect, constrainRect, newRect;
    Point delta, grid;
    GrCursor newcursor, oldcursor;
    bool firstmove;
    int hysterese;

protected:
    VObjectCommand(VObject *g);
    void Init(VObject *g, Rectangle cr, Point gr, GrCursor cd, int hy);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackConstrain(Point, Point, Point*);    
    void TrackFeedback(Point, Point, bool);
    void DoIt();
    void UndoIt();
};

//---- VObjectMover ------------------------------------------------------------

class VObjectMover: public VObjectCommand {
protected:
    Point origin;

    void Init(VObject *g, Rectangle cr, Point gr, GrCursor, int hy);

public:
    VObjectMover(VObject*);
    VObjectMover(VObject*, Rectangle);
    VObjectMover(VObject*, Rectangle, Point, GrCursor cr= eCrsMoveHand, int hy= 2);

    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

//---- VObjectStretcher --------------------------------------------------------

class VObjectStretcher: public VObjectCommand {
protected:
    Point minSize, p1, p2;
    int corner;

    void Init(VObject *g, Rectangle cr, Point gr, GrCursor, int hy, Point ms);

public:
    VObjectStretcher(VObject*);
    VObjectStretcher(VObject*, Rectangle);
    VObjectStretcher(VObject*, Rectangle, Point ms);
    VObjectStretcher(VObject*, Rectangle, Point ms, Point,
				      GrCursor cr= eCrsMoveStretch, int hy= 2);

    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackConstrain(Point, Point, Point*);
};

//---- CompositeVObject --------------------------------------------------------------

enum CompositeVObjectFlags {
    eCompVObjDefault= eVObjDefault,
    eCompVObjLast   = eVObjLast+0
};

class CompositeVObject: public VObject {
protected:
    bool modified;
    Collection *list;
public:
    MetaDef(CompositeVObject);

    CompositeVObject(int id= cIdNone, Collection *cp= 0);
    CompositeVObject(int id, ...);
    CompositeVObject(int id, va_list ap);
    ~CompositeVObject();

    void FreeAll();
    void Open(bool mode= TRUE);
    void DoObserve(int, int, void*, Object*);
    void SetModified()
	{ modified= TRUE; }
    Collection *GetList();
    Iterator *MakeIterator();
    int Size();
    virtual void Add(VObject*);
    virtual VObject *Remove(VObject*);
    VObject *SetAt(int at, VObject *vop);

    void Draw(Rectangle);
    void Outline2(Point, Point);
    Command *DispatchEvents(Point, Token, Clipper*);
    void Enable(bool b= TRUE, bool redraw= TRUE);
    void SetContainer(VObject*);
    void SetItems(VObject **);
    void SetItems(va_list ap);
    void SetOrigin(Point);
    void SetExtent(Point e);
    Metric GetMinSize();
    int Base();
    VObject *Detect(BoolFun, void *arg);
    VObject *At(int n)
	{ return (VObject*)list->At(n); }
    void SendDown(int, int, void*);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
    void InspectorId(char *buf, int sz);
    void Parts(class Collection*);
};

#include "Cluster.h"
#include "Filler.h"
#include "ImageItem.h"
#include "TextItem.h"

#endif VObject_First
