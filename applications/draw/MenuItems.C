//$PatternItem,LineStyleItem$

#include "MenuItems.h"
#include "WindowSystem.h"

//---- PatternMenuItem ---------------------------------------------------------

void PatternItem::Draw(Rectangle)
{
    if (ink == ePatWhite)
	GrStrokeRect(contentRect.Inset(2));
    else
	GrPaintRect(contentRect.Inset(2), ink);
}

//---- LineStyleItem ----------------------------------------------------------

void LineStyleItem::Draw(Rectangle r)
{
    r= contentRect.Inset(Point(5,0));
    GrSetPenSize(width);
    if (gColor && (cap != eDefaultCap)) {
	GrStrokeLine(ePatBlack, (GrLineCap) (cap & eStartArrow), r.W(), r.Center());
	GrStrokeLine(gHighlightColor, (GrLineCap) (cap & eEndArrow), r.Center(), r.E());
    } else
	GrStrokeLine(ePatBlack, cap, r.W(), r.E());
    GrSetPenSize(1);
    if (width >= 12) {
	u_char *b= (u_char*) form("%d", width);
	Point p= gSysFont->AdjustString(b, r.Center(), eAdjVCenter, eAdjHCenter);
	GrShowString(gSysFont, ePatWhite, p, b);
    }
}
