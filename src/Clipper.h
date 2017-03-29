#ifndef Clipper_First
#ifdef __GNUG__
#pragma once
#endif
#define Clipper_First

#include "VObject.h"

enum ClipperFlags {
    eClipperDefault  = eVObjDefault,
    eClipperLast     = eVObjLast+0
};

extern bool gBatch;


class Clipper: public VObject {
    Point minExtent, relOrigin, offset;
protected:
    VObject *vop;                          // the VObject I show
    Ink *bgcolor;
    
    void FeedbackOnce(Command *tracker, Point ap, Point pp, bool turniton);
    
public:
    MetaDef(Clipper);
    Clipper(VObject *vp= 0, Point minsize= gPoint_1, int id= cIdNone,
						    Ink *bg= ePatWhite);
    void Open(bool mode);
    Metric GetMinSize();
    void SetMinExtent(Point e);
    Point GetMinExtent()
	{ return minExtent; }
    void SetFocus(Rectangle, Point);
    void Enable(bool b= TRUE, bool redraw= TRUE);
    VObject *Detect(BoolFun find, void *arg);
    void SetBgInk(Ink *);
    Ink *GetBgInk();
	
    //---- drawing ------------------------
    void DrawInner(Rectangle, bool);
    void DrawInFocus(VoidObjMemberFunc, Object*, void*, void*, void*, void*);
    void Feedback(Object*, void*, void*, void*);
	
    //---- event handling -----------------
    Command *DispatchEvents(Point, Token, Clipper*);
    Point ContainerPoint(Point p);
    bool TrackOnce(Command**, TrackPhase, Point, Point, Point);
    Command *TrackInContent(Point, Token, Command*);
    void SendDown(int, int, void*);
    void Control(int, int, void*);
    
    //---- resize -------------------------
    void SetOrigin(Point);
    void SetExtent(Point e);
    
    //---- views --------------------------
    virtual void ShowsView(VObject *vp);
    virtual void RemoveView(VObject *vp);
    virtual void ViewSizeChanged(Point, Point);
    virtual Rectangle GetViewSize();
    Rectangle GetViewedRect();
    VObject *GetViewedVObject();
    
    //---- scrolling ----------------------
    Point GetRelOrigin()
	{ return relOrigin; }
    virtual Point AutoScroll(Point);
    virtual void Scroll(int mode, Point scroll, bool redraw= TRUE);
    void RevealRect(Rectangle, Point);
    void RevealAlign(Rectangle r, VObjAlign al= VObjAlign(eVObjHLeft+eVObjVTop));
  
    //---- input/output --------------------
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);

    //---- inspecting ----------------------
    void InspectorId(char *buf, int sz);
    void Parts(Collection*);
};

#endif Clipper_First

