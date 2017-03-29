#ifndef LineItem_First
#ifdef __GNUG__
#pragma once
#endif
#define LineItem_First

#include "VObject.h"

//---- LineItem -----------------------------------------------------------------

class LineItem : public VObject {
    short lineWidth, minLength;
    bool dir;
public:
    MetaDef(LineItem);
    LineItem(bool d= TRUE, int lw= 1, int ml= 20, int id= cIdNone);
    Metric GetMinSize();
    void Draw(Rectangle);
};

#endif LineItem_First

