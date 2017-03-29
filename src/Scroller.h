#ifndef Scroller_First
#ifdef __GNUG__
#pragma once
#endif
#define Scroller_First

#include "Cluster.h"

typedef enum ScrollDir {
    eScrollNone       = 0,
    eScrollRight      = BIT(0),
    eScrollDown       = BIT(1),
    eScrollLeft       = BIT(2),
    eScrollUp         = BIT(3),
    eScrollHideScrolls= BIT(4),
    eScrollDefault    = eScrollRight | eScrollDown
} ScrollDir;

const int cScrollBarSize= 20;

class Scroller: public CompositeVObject {
protected:
    class ScrollBar *sb[4];
    class Clipper *vf;
    VObject *vop;
    Point ms;
    ScrollDir controls, visibleControls;
public:
    MetaDef(Scroller);
    Scroller(VObject *v= 0, Point e= gPoint0, int id= cIdNone,
		    ScrollDir= (ScrollDir)(eScrollDefault|eScrollHideScrolls));
    void Init(VObject*, Point minSize, int id);
    Clipper *GetClipper()
	{ return vf; }
    void Draw(Rectangle);
    void Control(int, int, void*);
    
    void SetBgInk(Ink *);
    Ink *GetBgInk();
    
    void SetExtent(Point e);
    void SetOrigin(Point at);
    
    Metric GetMinSize();
    void ViewSizeChanged(Point viewExtent);
    bool NeedControls(Point e, Point vs);
    void BubbleUpdate(Point bs);
    void Open(bool mode);

    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif Scroller_First


