#ifndef VObjectPair_First
#ifdef __GNUG__
#pragma once
#endif
#define VObjectPair_First

#include "VObject.h"

//---- VObjectPair -----------------------------------------------------------

class VObjectPair: public VObject {
protected:
    VObject *left, *right;
    Point gap;

public:
    MetaDef(VObjectPair);
    VObjectPair(VObject *left, VObject *right, Point gap= gPoint2);
    void SetContainer(VObject *v);
    int Base();
    Metric GetMinSize();
    void SetOrigin(Point);
    void SetExtent(Point);
    void Draw(Rectangle);
    void FreeAll();
    void Parts(Collection* col);
    Command *DispatchEvents(Point lp, Token t, Clipper *vf);
    void SendDown(int, int, void*);
    void Enable(bool b= TRUE, bool redraw= TRUE);
    ostream& PrintOn(ostream &s);
    istream& ReadFrom(istream &s);
};

#endif VObjectPair_First

