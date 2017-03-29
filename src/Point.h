#ifndef Point_First
#ifdef __GNUG__
#pragma once
#endif
#define Point_First

#include "Types.h"
#include "Class.h"

class Point {
public:
	short x;
	short y;

public:
	
	Point()
	    { x= y= 0; }
	
	Point(short xy)
	    { x= y= xy; }
	
	Point(short xx, short yy)
	    { x= xx; y= yy; }

	Point(int sx, int sy)
	    { x= (short) sx; y= (short) sy; }

	Point(float sx, float sy)
	    { x= (short) sx; y= (short) sy; }

	short &operator[](bool b) const
	    { if (b) return y; else return x; }

	friend Point operator+ (Point p1, Point p2)
	    { return Point(p1.x + p2.x, p1.y + p2.y); }

	friend Point operator- (Point p)
	    { return Point(-p.x, -p.y); }

	friend Point operator- (Point p1, Point p2)
	    { return Point(p1.x - p2.x, p1.y - p2.y); }

	friend Point operator* (Point p1, Point p2)
	    { return Point(p1.x * p2.x, p1.y * p2.y); }

	friend Point operator/ (Point p1, Point p2)
	    { return Point(p1.x / p2.x, p1.y / p2.y); }
	    
	friend Point Scale(Point p, Point num, Point denom);
	    
	friend Point Scale(Point p, float sx, float sy);
	    
	Point operator+= (Point p)
	    { x+= p.x; y+= p.y; return *this; }

	Point operator-= (Point p)
	    { x-= p.x; y-= p.y; return *this; }

	Point operator*= (Point p)
	    { x*= p.x; y*= p.y; return *this; }

	Point operator/= (Point p)
	    { x/= p.x; y/= p.y; return *this; }

	Point Half () const;

	friend bool operator== (Point p1, Point p2)
	    { return (bool) (p1.x == p2.x && p1.y == p2.y); }

	friend bool operator!= (Point p1, Point p2)
	    { return (bool) (p1.x != p2.x || p1.y != p2.y); }

	friend bool operator> (Point p1, Point p2)
	    { return (bool) (p1.x > p2.x && p1.y > p2.y); }

	friend bool operator< (Point p1, Point p2)
	    { return (bool) (p1.x < p2.x && p1.y < p2.y); }

	friend bool operator>= (Point p1, Point p2)
	    { return (bool) (p1.x >= p2.x && p1.y >= p2.y); }

	friend bool operator<= (Point p1, Point p2)
	    { return (bool) (p1.x <= p2.x && p1.y <= p2.y); }

	friend Point Abs (Point);

	friend Point Sign (Point);

	friend Point Min (Point, Point);

	friend Point Max (Point, Point);
	
	friend Point Range (Point, Point, Point);

	friend Point Half (Point);
	
	friend float Phi (Point);
	
	friend float Length (Point);
	    // return sqrt(p.x*p.x + p.y*p.y);
	    
	friend Point PolarToPoint(double ang, double fx, double fy);
	    // return Point(fx*cos(ang), fy*sin(ang));

	friend void Swap (Point &p1, Point &p2);

	friend ostream& operator<< (ostream& s, Point p);
	    
	friend istream& operator>> (istream& s, Point &p);
	
	char *AsString() const;
};

SimpleMetaDef(Point);

#ifndef FakeExternalDecl
    extern const Point gPoint_1, gPoint0, gPoint1, gPoint2, gPoint3, gPoint4, gPoint8,
		   gPoint10, gPoint16, gPoint32, gPoint64;
#endif

#endif Point
