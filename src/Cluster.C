//$Cluster$
#include "Cluster.h"
#include "Math.h"
#include "Storage.h"

//---- Cluster -----------------------------------------------------------------

MetaImpl(Cluster, (T(rows), T(cols), T(minGap), T(actGap), 
		   T(minsize), TE(align),0));

Cluster::Cluster(int id, VObjAlign a, Point g, Collection *cp)
					: CompositeVObject(id, cp)
{
    Init(a, g);
}

Cluster::Cluster(int id, VObjAlign a, Point g, VObject *va_(vop), ...)
					: CompositeVObject(id, (Collection*)0)
{
    va_list ap;
    va_start(ap, va_(vop));
    Add(va_(vop));
    SetItems(ap);
    Init(a, g);
    va_end(ap);
}

Cluster::Cluster(int id, VObjAlign a, Point g, va_list ap)
					: CompositeVObject(id, ap)
{
    Init(a, g);
}

Cluster::~Cluster()
{
    SafeDelete(wd);
    SafeDelete(ht);
    SafeDelete(bs);
}

void Cluster::Init(VObjAlign a, Point g)
{
    minGap= actGap= g;
    align= a;
    rows= cols= 0;
    wd= ht= bs= 0;
    CalcDimensions();
}

void Cluster::CalcDimensions()
{
    int x, y, s= Size();
    
    if (s > 0) {
	x= CalcCols();
	y= (s-1)/x+1;
	
	if (rows != y) {
	    rows= y;
	    ht= (short*) Realloc(ht, rows * sizeof(short));
	    bs= (short*) Realloc(bs, rows * sizeof(short));
	}
	if (cols != x) {
	    cols= x;
	    wd= (short*) Realloc(wd, cols * sizeof(short));
	}
	modified= FALSE;
    } else
	rows= cols= 0;
    CacheMinSize();
}

short Cluster::CalcCols()
{
    int x, s= Size();
    register VObjAlign aa;
    
    aa= (VObjAlign)(align & ~(eVObjHExpand|eVObjVExpand));
    if ((aa & eVObjH) && (aa & eVObjV))
	x= intsqrt(s);
    else if (aa & eVObjH)
	x= 1;
    else if (aa & eVObjV)
	x= s;
    else
	x= 1; // tom@iyf.tno.nl
    return x;
}

void Cluster::SetOrigin(Point at)
{
    register VObject *dip;
    register int x, y;
    Point a;
    
    VObject::SetOrigin(at);
    
    a.x= at.x;
    for (x= 0; x < cols; x++) {
	a.y= at.y;
	for (y= 0; y < rows; y++) {
	    if (dip= GetItem(x,y))
		dip->Align(a, Metric(wd[x], ht[y], bs[y]), align);
	    a.y+= ht[y] + actGap.y;
	}
	a.x+= wd[x] + actGap.x;
    }
}

Metric Cluster::GetMinSize()
{
    if (modified)
	CalcDimensions();
    return minsize;
}

int Cluster::Base()
{
    return minsize.Base();
}

void Cluster::CacheMinSize()
{
    Metric m;
    
//    if (Size() > 0) {
	register VObject *dip;
	register int x, y, hh, bb;
	int totalw, totalh, totalb, mid;
    
	mid= rows/2;
	if (EVEN(rows))
	    mid--;
	    
	totalb= totalh= totalw= 0;
	for (y= 0; y < rows; y++) {
	    hh= bb= 0;
	    for (x= 0; x < cols; x++) {
		if (dip= GetItem(x, y)) {
		    m= dip->GetMinSize();
		    if (align & eVObjVBase) {
			hh= max(hh, m.Base());
			int eb= m.Height()-m.Base();
			bb= max(bb, eb);
		    } else
			hh= max(hh, m.Height());
		    if (y == 0)
			wd[x]= m.Width();
		    else
			wd[x]= max(wd[x], m.Width());
		}
	    }
	    if (y == mid) {
		totalb= totalh+hh;
		if (EVEN(rows))
		    totalb+= bb;
	    }
	    ht[y]= hh+bb;
	    bs[y]= hh;
	    totalh+= ht[y];
	}
	for (x= 0; x < cols; x++)
	    totalw+= wd[x];
	    
	m.extent.x= totalw + minGap.x*(cols-1);
	m.extent.y= totalh + minGap.y*(rows-1);
	m.base= totalb + minGap.y * mid;
//    }
    
    minsize= m;
}

void Cluster::SetExtent(Point e)
{   
    register VObject *dip;
    register int x, y, divx= 0, divy= 0, b;
    Metric m;
    bool hexp= (align & eVObjHExpand),
	 vexp= (align & eVObjVExpand);
    
    Point d= e-minsize.Extent();
    
    VObject::SetExtent(e);
	
    actGap= minGap;
    if (hexp)
	divx= d.x/cols;
    else if (cols > 1)
	divx= d.x/(cols-1);
    else
	divx= 0;
    if (vexp)
	divy= d.y/rows;
    else if (rows > 1)
	divy= d.y/(rows-1);
    else
	divy= 0;
    actGap.x+= divx;
    actGap.y+= divy;
    
    for (x= 0; x < cols; x++) {
	for (y= 0; y < rows; y++) {
	    if (dip= GetItem(x, y)) {
		m= dip->GetMinSize();
		if (hexp)
		    m.extent.x= wd[x] + divx;
		if (vexp)
		    m.extent.y= ht[y] + divy;
		if (m.Extent() != dip->GetExtent())
		    dip->SetExtent(m.Extent());
	    }
	}
    }
    b= rows/2;
    if (EVEN(rows))
	b--;
    minsize.base+= b * divy;
}

ostream& Cluster::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << minGap SP << align SP;
}

istream& Cluster::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    s >> minGap >> Enum(align);
    actGap= minGap;
    return s;
}
