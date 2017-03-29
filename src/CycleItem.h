#ifndef CycleItem_First
#ifdef __GNUG__
#pragma once
#endif
#define CycleItem_First

#include "VObject.h"

//---- CycleItem ---------------------------------------------------------------

class CycleItem: public CompositeVObject {
protected:
    int current;
    VObjAlign align;
    bool mode;
public:
    MetaDef(CycleItem);
    CycleItem(int id= cIdNone, VObjAlign a= eVObjVBase, Collection *cp= 0);
    CycleItem(int id, VObjAlign a, ...);
    CycleItem(int id, VObjAlign a, va_list ap);
    void Init(VObjAlign a);
    void SetOrigin(Point at);
    void SetExtent(Point e);
    Metric GetMinSize();
    void Draw(Rectangle);
    Command *DispatchEvents(Point, Token, Clipper*);
    void SetCurrentItem(int at);
    int GetCurrentItem()
	{ return current; }    
    void SendDown(int id, int part, void *val);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

#endif CycleItem_First

