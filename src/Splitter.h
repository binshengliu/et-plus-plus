#ifndef Splitter_First
#ifdef __GNUG__
#pragma once
#endif
#define Splitter_First

#include "VObject.h"

class Splitter: public CompositeVObject {
    Point split, ms;
    VObject *vop;
    void TestOpen(int i, int w, int h, Point &minsize);
public:
    MetaDef(Splitter);
    Splitter(VObject *v, Point minsize= gPoint0, int id= cIdNone);
    void Init(VObject *vop, Point minsize);
    
    void Open(bool mode= TRUE);
    Metric GetMinSize();
    void SetExtent(Point e);
    void SetOrigin(Point at);
    void SetSplit(bool, Point);
    void Control(int, int, void*);
    void SendDown(int, int, void*);
    istream& ReadFrom(istream&);
    ostream& PrintOn(ostream &s);
};

#endif Splitter_First
