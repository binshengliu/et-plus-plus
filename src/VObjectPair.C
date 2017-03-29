//$VObjectPair$

#include "VObjectPair.h"

//---- VObjectPair -------------------------------------------------------------

MetaImpl(VObjectPair, (TP(left), TP(right), T(gap), 0));

VObjectPair::VObjectPair(VObject *l, VObject *r, Point g)
{
    left= l;
    right= r;
    gap= g;
}

void VObjectPair::FreeAll()
{
    VObject::FreeAll();
    left->FreeAll();
    right->FreeAll();
}

void VObjectPair::Enable(bool b, bool redraw)
{
    VObject::Enable(b, redraw);
    left->Enable(b, FALSE);
    right->Enable(b, FALSE);
}

void VObjectPair::SetContainer(VObject *v)
{
    VObject::SetContainer(v);
    left->SetContainer(this);
    right->SetContainer(this);
}

int VObjectPair::Base()
{
    return max(left->Base(), right->Base());
}
    
Metric VObjectPair::GetMinSize()
{
    return HCat(left->GetMinSize(), right->GetMinSize(), gap.x);
}

void VObjectPair::Draw(Rectangle r)
{
    left->DrawAll(r, FALSE); 
    right->DrawAll(r, FALSE);
}

void VObjectPair::SendDown(int id, int p, void *val)
{
    left->SendDown(id, p, val);    
    right->SendDown(id, p, val);    
}

void VObjectPair::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    left->SetOrigin(at+Point(0, Base() - left->Base()));
    if (left->TestFlag(eVObjHFixed) && right->TestFlag(eVObjHFixed))
	at.x+= GetExtent().x-right->Width();
    else
	at.x+= left->Width() + gap.x;
    at.y+= Base() - right->Base();
    right->SetOrigin(at);
}
    
void VObjectPair::SetExtent(Point e)
{
    VObject::SetExtent(e);
    left->CalcExtent();
    right->CalcExtent();
    if (left->TestFlag(eVObjHFixed) && right->TestFlag(eVObjHFixed)) {
	/* gap.x= e.x-left->Width()-right->Width() */;
    } else {
	e.x= e.x - gap.x - left->Width();
	e.y= right->Height();
	right->SetExtent(e);
    }
}

Command *VObjectPair::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    Command *cmd;
    
    if (cmd= left->Input(lp, t, vf))
	return cmd;
    if (cmd= right->Input(lp, t, vf))
	return cmd;
    return VObject::DispatchEvents(lp, t, vf);
}

ostream& VObjectPair::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << left SP << right SP;
}

istream& VObjectPair::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    return s >> left >> right;
}

void VObjectPair::Parts(Collection* col)
{
    VObject::Parts(col);
    col->Add(left);
    col->Add(right);
}
