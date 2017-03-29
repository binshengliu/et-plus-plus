//$FixedLineTextView$

#include "FixLineTView.h"

MetaImpl0(FixedLineTextView);

FixedLineTextView::FixedLineTextView(EvtHandler *eh, Rectangle r, Text *t, eTextJust m, eSpacing sp, 
				bool doWrap, TextViewFlags fl, Point b, int id)
			: TextView(eh, r, t, m, sp, doWrap, fl, b, id) 
{
}   

void FixedLineTextView::ConstrainScroll(Point *p)
{
    p->y= (p->y/LineHeight(0))*LineHeight(0);
}

Point FixedLineTextView::LineToPoint (int n, bool basePoint, bool relative)
{
    n= range(0, nLines,n);
    Point p(0, n * LineHeight(0));

    if (basePoint)
	p.y += BaseHeight(n);
    if (!relative)
	p += GetInnerOrigin();
    return p;
}

int FixedLineTextView::PointToLine(Point p) // p is in coordinates relative to contentRect
{
    int line, y= 0;
    p.y= max(0, p.y);
    line= p.y / LineHeight(0);
    return (min(nLines,line));
}

