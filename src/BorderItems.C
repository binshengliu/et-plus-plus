//$AbstractBorderItem,BorderItem$

#include "BorderItems.h"
#include "String.h"

//---- AbstractBorderItem ------------------------------------------------------

AbstractMetaImpl(AbstractBorderItem, (T(interiorOffset), T(extentDiff), T(border),
    TP(interior), TP(titleBar), T(lineWidth), T(titleGap), T(shadowWidth), 0));

AbstractBorderItem::AbstractBorderItem(VObject *ti, VObject *in, int lw, int id)
					    : CompositeVObject(id, in, ti, 0)
{
    interior= in;
    titleBar= ti;
    lineWidth= lw;
}

AbstractBorderItem::AbstractBorderItem(char *ti, VObject *in, int lw, int id)
		    : CompositeVObject(id, in, 0)
{
    interior= in;
    lineWidth= lw;
    Add(titleBar= new TextItem(ti));
}

Metric AbstractBorderItem::GetMinSize()
{
    Point e= interior->GetMinSize().extent;
    
    interiorOffset= border;
    extentDiff= 2*interiorOffset + shadowWidth;
    
    if (titleBar) {
	if (titleBar->IsKindOf(CompositeVObject))
	    ((CompositeVObject*)titleBar)->SetModified();
	Metric tm= titleBar->GetMinSize();
	tm.extent.x+= cHTitleGap*2;
	tm.extent.y+= titleGap;
	e.x= max(e.x, tm.extent.x);
	interiorOffset.y+= tm.extent.y;
	extentDiff.y+= tm.extent.y;
    }
    return e+extentDiff;
}

void AbstractBorderItem::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    interior->SetOrigin(at+interiorOffset);
    if (titleBar) {
	at.y+= border.y-1;
	//if (titleBar->TestFlag(eVObjHFixed))
	    at.x+= (Width() - shadowWidth - titleBar->Width())/2;
	//else
	//    at.x+= border.x+cHTitleGap;
	titleBar->SetOrigin(at);
    }
}

void AbstractBorderItem::SetExtent(Point e)
{
    VObject::SetExtent(e);
    e-= extentDiff;
    if (titleBar) {
	if (titleBar->TestFlag(eVObjHFixed))
	    titleBar->CalcExtent();
	else
	    titleBar->SetExtent(Point(e.x-cHTitleGap*2, interiorOffset.y-border.y));
    }
    interior->SetExtent(e);
}

ostream& AbstractBorderItem::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << border SP << lineWidth SP << interiorOffset SP << extentDiff SP
						<< interior SP << titleBar SP;
}

istream& AbstractBorderItem::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    return s >> border >> lineWidth >> interiorOffset >> extentDiff >>
					interior >> titleBar;
}

void AbstractBorderItem::SetTitle(char *itsTitle, bool redraw)
{
    if (titleBar == 0)
	return;
	
    // ugly hack
    VObject *tit= titleBar->FindItem(cIdWindowTitle);
    if (tit == 0)
	tit= titleBar;
    if (tit && tit->IsKindOf(TextItem)) {
	((TextItem*)tit)->SetString(itsTitle);
	tit->CalcExtent();
	if (titleBar->IsKindOf(CompositeVObject))
	    ((CompositeVObject*)titleBar)->SetModified();
	if (redraw && IsOpen()) {
	    Rectangle oldr= titleBar->contentRect;
	    titleBar->CalcExtent();
	    titleBar->SetExtent(oldr.extent);
	    titleBar->SetOrigin(oldr.origin);
	    InvalidateRect(oldr);
	}
    }
}

char *AbstractBorderItem::GetTitle()
{
    if (titleBar == 0)
	return 0;
	
    VObject *tit= titleBar->FindItem(cIdWindowTitle);
    if (tit == 0)
	tit= titleBar;
    if (tit && tit->IsKindOf(TextItem))
	return tit->AsString();
    return 0;
}

void AbstractBorderItem::InspectorId(char *buf, int sz)
{
    if (titleBar == 0)
	return;
	
    VObject *tit= titleBar->FindItem(cIdWindowTitle);
    if (tit && tit->IsKindOf(TextItem)) 
	strn0cpy(buf, ((TextItem*)tit)->AsString(), sz);
}

//---- BorderItem --------------------------------------------------------------

MetaImpl(BorderItem, (TE(titleAlign), 0));

BorderItem::BorderItem(VObject *ip, Point b, int lw, VObjAlign a)
			    : AbstractBorderItem((VObject*)0, ip, lw, cIdNone)
{
    border= b;
    titleAlign= a;
}

BorderItem::BorderItem(VObject *ti, VObject *ip, Point b, int lw, VObjAlign a)
				    : AbstractBorderItem(ti, ip, lw, cIdNone)
{
    border= b;
    titleAlign= a;
}

BorderItem::BorderItem(char *ti, VObject *ip, Point b, int lw, VObjAlign a)
				    : AbstractBorderItem(ti, ip, lw, cIdNone)
{
    border= b;
    titleAlign= a;
}

BorderItem::BorderItem(int id, char *ti, VObject* ip, Point b, int lw, VObjAlign a)
				    : AbstractBorderItem(ti, ip, lw, id)
{
    border= b;
    titleAlign= a;
}

void BorderItem::Draw(Rectangle r)
{
    if (! interior->contentRect.ContainsRect(r)) {
	GrSetPenNormal();
	if (lineWidth >= 0) {
	    GrSetPenSize(lineWidth);
	    if (! Enabled())
		GrSetPenPattern(ePatGrey50);
	}
	if (titleBar) {
	    if (lineWidth >= 0) {
		int h= titleBar->Height()/2;
		Rectangle rr= contentRect;
		rr.origin.y+= h;
		rr.extent.y-= h;
		GrStrokeRect(rr);
	    }
	    GrEraseRect(titleBar->contentRect);
	} else if (lineWidth >= 0)
	    GrStrokeRect(contentRect);
	GrSetPenNormal();
    }
    AbstractBorderItem::Draw(r);
}

void BorderItem::SetOrigin(Point at)
{
    int lw= lineWidth >= 0 ? lineWidth : 0;  
    VObject::SetOrigin(at);
    if (titleBar) {
	int th= titleBar->Height();
	int tgap= border.x+lw+cBorderTitleGap;
	titleBar->Align(Point(at.x+tgap, at.y), Metric(Width()-2*tgap, th),
				(VObjAlign)((titleAlign & eVObjH) | eVObjVTop));
	int d= th - (border.y+lw);
	if (d > 0)
	    at.y+= d;
    }
    interior->SetOrigin(at+border+Point(lw));
}

Metric BorderItem::GetMinSize()
{
    int lw= lineWidth >= 0 ? lineWidth : 0;  
    Metric m= interior->GetMinSize();
    
    if (titleBar) {
	Metric tm= titleBar->GetMinSize();
	m.extent.x= max(m.extent.x, tm.extent.x+2*cBorderTitleGap);
	int d= tm.extent.y - (border.y+lw);
	if (d > 0) {
	    m.extent.y+= d;
	    m.base+= d;
	}
    }
    SetFlag(eVObjHFixed, interior->TestFlag(eVObjHFixed));
    SetFlag(eVObjVFixed, interior->TestFlag(eVObjVFixed));
    //SetFlag(eVObjVFixed);
    return m.Expand(border+Point(lw));
}

void BorderItem::SetExtent(Point e)
{
    int d= 0, lw= lineWidth >= 0 ? lineWidth : 0;  

    VObject::SetExtent(e);
    if (titleBar) {
	titleBar->CalcExtent();
	d= titleBar->Height() - (border.y+lw);
	if (d > 0)
	    e.y-= d;
    }
    interior->SetExtent(e-2*(border+Point(lw)));
}

int BorderItem::Base()
{
    int d= 0, lw= lineWidth >= 0 ? lineWidth : 0;
      
    if (titleBar) {
	d= titleBar->Height() - (border.y+lw);
	if (d < 0)
	    d= 0;
    }
    return interior->Base()+d+border.y+lw;
}

ostream& BorderItem::PrintOn(ostream &s)
{
    AbstractBorderItem::PrintOn(s);
    return s << titleAlign SP;
}

istream& BorderItem::ReadFrom(istream &s)
{
    AbstractBorderItem::ReadFrom(s);
    return s >> Enum(titleAlign);
}
