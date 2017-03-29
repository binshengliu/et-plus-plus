#ifndef BackgroundItem_First
#ifdef __GNUG__
#pragma once
#endif
#define BackgroundItem_First

#include "VObject.h"

//---- BackgroundItem ----------------------------------------------------------

class BackgroundItem: public CompositeVObject {
protected:
    Ink *ink;
public:
    MetaDef(BackgroundItem);
    BackgroundItem(Ink *p, VObject *ip= 0);
    void DrawInner(Rectangle r, bool highlight);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

#endif BackgroundItem_First

