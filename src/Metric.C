//$MetricClass$

#include "Metric.h"
#include "Class.h"
#include "String.h"

SimpleMetaImpl(Metric)
{
    Metric *m= (Metric*) addr;
    sprintf(buf, "w: %d  h: %d  b: %d", m->extent.x, m->extent.y, m->base);
}

Metric::Metric()
{
    extent.x= extent.y= base= 0;
}

Metric::Metric(Point e, short b)
{
    extent= e;
    base= b;
}

Metric::Metric(Point e)
{
    extent= e;
    base= extent.y;
}

Metric::Metric(int w)
{
    extent.x= extent.y= w;
    base= w;
}

Metric::Metric(int w, int h)
{
    extent.x= w;
    base= extent.y= h;
}
    
Metric::Metric(int w, int h, int b)
{
    extent.x= w;
    extent.y= h;
    base= b;
}

Metric::Metric(const Metric &m)
{
    extent.x= m.extent.x;
    extent.y= m.extent.y;
    base= m.base;
}

Metric Metric::Inset(Point e)
{
    extent-= 2*e;
    base-= e.y;
    return *this;
}

Metric Metric::Expand(Point e)
{
    extent+= 2*e;
    base+= e.y;
    return *this;
}

Metric Metric::Merge(const Metric &m)
{
    extent.x= max(extent.x, m.extent.x);
    base= max(base, m.base);
    int descender= max(extent.y-base, m.extent.y-m.base);
    extent.y= base+descender;
    return *this;
}

Metric Metric::Cat(Direction d, const Metric &m, const Point &gap)
{
    if (d == eVert) {
	extent.x= max(extent.x, m.extent.x);
	extent.y+= m.extent.y + gap.y;
	base= extent.y;
    } else {        // eHor
	base= max(base, m.base);
	extent.x+= m.extent.x + gap.x;
	extent.y= base+max(extent.y-base, m.extent.y-m.base);
    }
    return *this;
}

Metric HCat(const Metric &m1, const Metric &m2, int hgap)
{
    Metric m;
    m.base= max(m1.base, m2.base);
    m.extent.x= m1.extent.x + m2.extent.x + hgap;
    m.extent.y= m.base+max(m1.extent.y-m1.base, m2.extent.y-m2.base);
    return m;
}

Metric VCat(const Metric &m1, const Metric &m2, int vgap)
{
    Metric m;
    m.extent.x= max(m1.extent.x, m2.extent.x);
    m.base= m.extent.y= m1.extent.y + m2.extent.y + vgap;
    // m.base= m.extent.y/2;
    return m;
}

Metric Cat(Direction d, const Metric &m1, const Metric &m2, Point gap)
{
    if (d == eVert)
	return VCat(m1, m2, gap.y);
    return HCat(m1, m2, gap.x);
}

Metric Min(const Metric &m1, const Metric &m2)
{
    Metric m;
    m.extent.x= min(m1.extent.x, m2.extent.x);
    m.base= min(m1.base, m2.base);
    int descender= min(m1.extent.y-m1.base, m2.extent.y-m2.base);
    m.extent.y= descender+m.base;
    return m;
}

Metric Max(const Metric &m1, const Metric &m2)
{
    Metric m;
    m.extent.x= max(m1.extent.x, m2.extent.x);
    m.base= max(m1.base, m2.base);
    int descender= max(m1.extent.y-m1.base, m2.extent.y-m2.base);
    m.extent.y= descender+m.base;
    return m;
}     

