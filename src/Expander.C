//$Expander$
#include "Expander.h"

MetaImpl(Expander, (TE(dir), T(gap), 0));

Expander::Expander(int id, Direction d, Point g, VObject *va_(vop), ...)
					: CompositeVObject(id, (Collection*)0)
{
    va_list ap;
    va_start(ap,va_(vop));
    dir= d;
    gap= g;
    Add(va_(vop));
    SetItems(ap);
    va_end(ap);
}

Expander::Expander(int id, Direction d, Point g, Collection *c)
					: CompositeVObject(id, c)
{
    dir= d;
    gap= g;
}

Metric Expander::GetMinSize()
{    
    Iter next(MakeIterator());
    VObject *vop;
    Metric e;
    int xf= 0, yf= 0;

    e.extent[dir]= -gap[dir];
    while (vop= (VObject*) next()) {
	e.Cat(dir, vop->GetMinSize(), gap);
	if (vop->TestFlag(eVObjVFixed))
	    yf++;
	if (vop->TestFlag(eVObjHFixed))
	    xf++;
    }
    ResetFlag(eVObjVFixed|eVObjHFixed);
    if (dir == eVert) {
	SetFlag(eVObjVFixed, yf >= Size());
	SetFlag(eVObjHFixed, xf > 0);
    } else {
	SetFlag(eVObjVFixed, yf > 0);
	SetFlag(eVObjHFixed, xf >= Size());
    }
    return e;
}

int Expander::ExpandCnt()
{
    Iter next(MakeIterator());
    register Object *op;
    int n= 0, f= dir ? eVObjVFixed : eVObjHFixed;
    
    while (op= next())
	if (! op->TestFlag(f))
	    n++;
    return n;
}

void Expander::SetExtent(Point e)
{
    Iter next(MakeIterator());
    register VObject *vop;
    int d, div= 0, mod= 0, n= ExpandCnt();
    Point newsize;
    bool hfixed, vfixed;
    
    VObject::SetExtent(e);
    d= (e-GetMinSize().extent)[dir];
    if (n > 0) {
	div= d / n;
	mod= d % n;
    }
    
    while (vop= (VObject*) next()) {
	vfixed= vop->TestFlag(eVObjVFixed);
	hfixed= vop->TestFlag(eVObjHFixed);
	newsize= vop->GetMinSize().extent;
	if (dir == eVert) {
	    if (!hfixed)
		newsize.x= e.x;
	    if (!vfixed) {
		newsize.y+= div;
		if (mod > 0) {
		    newsize.y++;
		    mod--;
		}
	    }
	} else {
	    if (!vfixed)
		newsize.y= e.y;
	    if (!hfixed) {
		newsize.x+= div;
		if (mod > 0) {
		    newsize.x++;
		    mod--;
		}
	    }
	}
	vop->SetExtent(newsize);
    }
}

void Expander::SetOrigin(Point at)
{
    Iter next(MakeIterator());
    register VObject *vop;
    Metric m= GetMinSize();
    int y= 0;
    
    if (dir == eHor)
	y= at.y + m.base;

    VObject::SetOrigin(at);
    while (vop= (VObject*) next()) {
	if ((dir == eHor) && vop->TestFlag(eVObjVFixed)
				/* && vop->GetExtent().y != m.extent.y */)
	    at.y= y - vop->GetMinSize().base;
	vop->SetOrigin(at);
	if (dir)
	    at.y+= vop->Height() + gap.y;
	else
	    at.x+= vop->Width() + gap.x;
    }
}
