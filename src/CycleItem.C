//$CycleItem$

#include "CycleItem.h"

//---- CycleItem ---------------------------------------------------------------

MetaImpl(CycleItem, (T(current), TE(align), TB(mode), 0));

CycleItem::CycleItem(int id, VObjAlign a, Collection *cp)
					: CompositeVObject(id, cp)
{
    Init(a);
}

CycleItem::CycleItem(int id, VObjAlign va_(a), ...)
					: CompositeVObject(id, (Collection*)0)
{
    va_list ap;
    va_start(ap, va_(a));
    SetItems(ap);
    Init(va_(a));
    va_end(ap);
}

CycleItem::CycleItem(int id, VObjAlign a, va_list ap)
					: CompositeVObject(id, ap)
{
    Init(a);
}

void CycleItem::Init(VObjAlign a)
{
    current= 0;
    align= a;
    mode= FALSE;
}

Metric CycleItem::GetMinSize()
{
    Metric m;

    if (mode)
	m= At(current)->GetMinSize();
    else {
	Iter next(list);
	VObject *dip;
	int w= 0, b1= 0, b= 0;

	while (dip= (VObject*) next()) {
	    m= dip->GetMinSize();
	    w= max(w, m.extent.x);
	    b= max(b, m.base);
	    b1= max(b1, m.extent.y-m.base);
	}
	m.extent.x= w;
	m.extent.y= b+b1;
	m.base= b;
    }
    return m;
}

void CycleItem::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    if (mode)
	At(current)->SetOrigin(at);
    else {
	Metric m(contentRect.extent);
	list->ForEach(VObject,Align)(at, m, align);
    }
}

void CycleItem::SetExtent(Point e)
{
    VObject::SetExtent(e);
    if (mode)
	At(current)->SetExtent(e);
    else {
	list->ForEach(VObject,SetExtent)(e);
    }
}

void CycleItem::Draw(Rectangle r)
{
    At(current)->DrawAll(r, FALSE);
}

Command *CycleItem::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    return At(current)->DispatchEvents(lp, t, vf);
}

void CycleItem::SetCurrentItem(int at)
{
    if (at != current) {
	if (current)
	    Control(cIdNone, cPartFocusRemove, At(current)); 
	current= at;
	if (mode)
	    Control(GetId(), cPartLayoutChanged, 0);
	else
	    ForceRedraw();
    }
}

void CycleItem::SendDown(int, int part, void*)
{
    int next= current;
    
    switch (part) {
    case cPartIncr:
	if (++next >= Size())
	    next= 0;
	break;
    case cPartDecr:
	if (--next < 0)
	    next= Size()-1;
	break;
    }
    SetCurrentItem(next);
}

ostream& CycleItem::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << current SP << align SP << mode SP;
}

istream& CycleItem::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    return s >> current >> Enum(align) >> Bool(mode);
}

