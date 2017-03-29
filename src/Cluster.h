#ifndef Cluster_First
#ifdef __GNUG__
#pragma once
#endif
#define Cluster_First

#include "VObject.h"

//---- Cluster -----------------------------------------------------------

class Cluster: public CompositeVObject {
protected:
    short *wd, *ht, *bs, rows, cols;
    Point minGap, actGap;
    Metric minsize;
    VObjAlign align;

protected:
    virtual short CalcCols();
    virtual void CacheMinSize();
    
public:
    MetaDef(Cluster);

    Cluster(int id, VObjAlign a= eVObjVBase, Point g= gPoint0, Collection *cp= 0);
    Cluster(int id, VObjAlign a, Point g, VObject *,...);
    Cluster(int id, VObjAlign a, Point g, va_list);
    ~Cluster();

    VObjAlign GetAlign()
	{ return align; }
    Point GetActGap()
	{ return actGap; }
    Point GetMinGap()
	{ return minGap; }    
    void CalcDimensions();
    void Init(VObjAlign, Point);
    void SetOrigin(Point at);
    void SetExtent(Point e);
    int Base();
    Metric GetMinSize();
    VObject *GetItem(int x, int y)
	{ return At(x*rows+y); }
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

#endif Cluster_First
