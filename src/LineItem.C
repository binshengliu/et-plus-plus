//$LineItem$

#include "LineItem.h"

//---- LineItem ----------------------------------------------------------------

MetaImpl(LineItem, (TB(dir), 0));

LineItem::LineItem(bool d, int lw, int ml, int id) : VObject(id)
{
    dir= d;
    lineWidth= lw;
    minLength= ml;
}

Metric LineItem::GetMinSize()
{
    if (dir)
	return Metric(minLength, lineWidth);
    return Metric(lineWidth, minLength);
}

void LineItem::Draw(Rectangle)
{
    GrPaintRect(contentRect, Enabled() ? ePatBlack : ePatGrey50);
}
