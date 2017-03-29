#ifndef FixLineTView_First
#ifdef __GNUG__
#pragma once
#endif
#define FixLineTView_First

#include "TextView.h"

//---- FixedLineTextView ----------------------------------------------------
// a TextView with a fixed interline spacing. The interline spacing is
// derived from the first line. FixedLineTextViews can be used for
// texts using only one font

class FixedLineTextView: public TextView {
public:
    MetaDef(FixedLineTextView);

    FixedLineTextView(EvtHandler *eh, Rectangle r, Text *t, 
	     eTextJust m= eLeft, eSpacing= eOne, bool wrap= TRUE,
	     TextViewFlags= (TextViewFlags) eVObjDefault, 
	     Point border= gBorder, int id= -1);    

    void ConstrainScroll(Point *p); // scroll complete lines

    //---- mapping
    // fixed linespaceing allows a more efficient implementation of these methods
    Point LineToPoint (int line, bool basePoint = FALSE, bool relative = TRUE);
    int PointToLine (Point p);    
};

#endif FixLineTView_First  
