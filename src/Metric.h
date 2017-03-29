#ifndef Metric_First
#ifdef __GNUG__
#pragma once
#endif
#define Metric_First

#include "Point.h"


class Metric {
public:
    Point extent;
    short base;

public:

    Metric();
    
    Metric(Point e, short b);
    
    Metric(Point e);

    Metric(int w);

    Metric(int w, int h);
	
    Metric(int w, int h, int b);

    Metric(const Metric&);

    Point Extent()
	{ return extent; }
	
    int Width()
	{ return extent.x; }
	
    int Height()
	{ return extent.y; }
	
    int Base()
	{ return base; }
	
    Metric Inset(Point);
    
    Metric Expand(Point);
    
    Metric Merge(const Metric&);
    Metric Cat(Direction d, const Metric &m, const Point &gap);
    
    operator Point()
	   { return extent; }
    
    friend Metric HCat(const Metric&, const Metric&, int hgap= 0);
    friend Metric VCat(const Metric&, const Metric&, int vgap= 0);
    friend Metric Cat(Direction d, const Metric&, const Metric&, Point gap= gPoint0);

    friend bool operator== (const Metric &m1, const Metric &m2)
	{ return (bool) (m1.extent == m2.extent && m1.base == m2.base); }

    friend bool operator!= (const Metric &m1, const Metric &m2)
	{ return (bool) (m1.extent != m2.extent && m1.base != m2.base); }

    friend Metric Min (const Metric&, const Metric&);

    friend Metric Max (const Metric&, const Metric&);        
};

SimpleMetaDef(Metric);


#endif Metric_First
