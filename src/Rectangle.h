#ifndef Rectangle_First
#ifdef __GNUG__
#pragma once
#endif
#define Rectangle_First

#include "Metric.h"

class Rectangle {
public:
    Point origin;
    Point extent;
      
public:
    
    Rectangle();

    Rectangle(short x, short y, short w, short h);

    Rectangle(Point o, Point e);
	
    Rectangle(Point e);

    Rectangle(short w, short h);
    
    int Width() const
	{ return extent.x; }

    int Height() const
	{ return extent.y; }

    int Left() const
	{ return origin.x; }

    int Top() const
	{ return origin.y; }
	
    Point Center() const
	{ return Point(origin.x+extent.x/2, origin.y+extent.y/2); }

    Point NW() const
	{ return origin; }
	
    Point N() const
	{ return Point(origin.x+extent.x/2, origin.y); }

    Point NE() const
	{ return Point(origin.x+extent.x-1, origin.y); }

    Point E() const
	{ return Point(origin.x+extent.x-1, origin.y+extent.y/2); }
	
    Point SE() const
	{ return origin + extent - 1; }
	
    Point S() const
	{ return Point(origin.x+extent.x/2, origin.y+extent.y-1); }
	
    Point SW() const
	{ return Point(origin.x, origin.y+extent.y-1); }

    Point W() const
	{ return Point(origin.x, origin.y+extent.y/2); }
	    
    friend Rectangle NormRect(Point p1, Point p2);
	
    friend Rectangle BoundingBox(int, Point *pts, Point *npts= 0);

    int Area () const
	{ return extent.x * extent.y; }

    Rectangle &Moveby (Point p)
	{ origin+= p; return *this; }
	
    friend Rectangle Moveby (Rectangle r, Point p)
	{ r.origin+= p; return r; }
	
    Rectangle operator+= (Point p)
	{ origin+= p; return *this; }
	
    Rectangle operator+ (Point p)
	{ return Rectangle(origin+p, extent); }
	
    Rectangle operator- (Point p)
	{ return Rectangle(origin-p, extent); }
	
    Rectangle operator-= (Point p)
	{ origin-= p; return *this; }

    void Moveto (Point p)
	{ origin= p; }

    void Scaleby (Point p)
	{ extent*= p; }

    friend bool operator== (const Rectangle &r1, const Rectangle &r2)
	{ return (bool) (r1.origin == r2.origin && r1.extent == r2.extent); }

    friend bool operator!= (const Rectangle &r1, const Rectangle &r2)
	{ return (bool) (r1.origin != r2.origin || r1.extent != r2.extent); }
	
    friend Rectangle Union(const Rectangle &r1, const Rectangle &r2);

    friend Rectangle Inter(const Rectangle &r1, const Rectangle &r2);

    friend int Difference(Rectangle *rp, const Rectangle &r1, const Rectangle &r2);
    
    Rectangle Intersect(const Rectangle &r);

    bool Clip(const Rectangle &r);
    
    bool ContainsRect(const Rectangle &r) const;
    bool OvalContainsRect(const Rectangle &r) const;
    
    Rectangle Inset(Point p);

    Rectangle Expand(Point p);
	
    Rectangle Merge(const Rectangle &r);

    bool ContainsPoint(Point p) const
	{ return (bool) (p >= origin && p < origin + extent); }

    bool Intersects(const Rectangle &r) const;
		  
    bool IsEmpty() const
	{ return (bool) (extent.x <= 0 || extent.y <= 0); }
	    
    bool IsNotEmpty() const
	{ return (bool) (extent.x > 0 && extent.y > 0); }

    Point Constrain(Point p);
	
    friend Point ConstrainMoveRect(const Rectangle &r1, const Rectangle &r2, Point delta);

    Point AmountToTranslateWithin(const Rectangle &r) const;

    int PointToAngle(Point) const;
    
    Point AngleToPoint(int) const;
    Point OvalAngleToPoint(int) const;
	    
    Rectangle WedgeBBox(int s, int e);
    
    //         7|0|1
    //        -------
    //         6| |2
    //        -------
    //         5|4|3

    int PointToCorner(Point p) const;

    Point CornerToPoint(int n) const;
    
    friend ostream& operator<< (ostream &s, const Rectangle &r);
		   
    friend istream& operator>> (istream &s, Rectangle &r);
    
    char *AsString() const;
};

SimpleMetaDef(Rectangle);

extern const Rectangle gRect0;

#endif Rectangle_First
