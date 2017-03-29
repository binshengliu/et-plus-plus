#ifndef Filler_First
#ifdef __GNUG__
#pragma once
#endif
#define Filler_First

#include "VObject.h"

//---- Filler ------------------------------------------------------------------

class Filler : public VObject {
protected:
    Point minsize;
    Ink *ink;
public:
    MetaDef(Filler);
    Filler(Point extent, int flags= 0);
    Filler(Point extent, Ink *i, int flags= 0);
    Ink *SetInk(Ink *i, bool redraw= TRUE);
    Ink *GetInk()
	{ return ink; }
    Metric GetMinSize();
    void Draw(Rectangle);
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif Filler_First

