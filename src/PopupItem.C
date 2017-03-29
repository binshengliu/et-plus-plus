//$PopupItem,PopupButton$
#include "PopupItem.h"
#include "ShadowItem.h"
#include "DevBitmap.h"
#include "Menu.h"

static int delay_cnt= 0;

//---- PopupButton ---------------------------------------------------------------

MetaImpl(PopupButton, (TP(menu), T(delay), 0));

PopupButton::PopupButton(int id, int dfltid, Menu *m, int dl) : Button(id)
{
    menu= m;
    SetSelectedItem(dfltid);
    delay= dl;
    if (delay > 0)
	SetFlag(eButtonIdle);
    else
	delay= 99999;
}

PopupButton::~PopupButton()
{
    // SafeDelete(menu);
}

Metric PopupButton::GetMinSize()
{
    Metric m;
    Iter next(menu->GetCollection());
    register VObject *vop;

    while (vop= (VObject*) next())
	m.Merge(vop->GetMinSize());

    return m;
}

int PopupButton::Base()
{
    return GetMinSize().base;
}

void PopupButton::SetSelectedItem(int id)
{
    if (menu) {                     
	VObject *old= 0, *clone, *theitem= menu->FindItem(id);
	if (theitem) {
	    // optimize copying of the seleced item for the common cases
	    // tom@izf.tno.nl
	    if (theitem->IsKindOf(TextItem))
		clone= menu->MakeMenuItem(theitem->AsString(), theitem->GetId());
	    else if (theitem->IsKindOf(ImageItem)) {
		Bitmap *newb, *b= ((ImageItem*)theitem)->GetBitmap();
		newb= new Bitmap(b->Size(), b->GetDevBitmap()->image, b->GetDepth());
		clone= new ImageItem(theitem->GetId(), newb);
	    } else // last resort
		clone= (VObject*) theitem->DeepClone();  
	    old= SetAt(0, (VObject*) clone);
	    menu->SetSelectedItem(id);
	} else
	    old= SetAt(0, new TextItem("oops!"));
	if (old)
	    delete old;
    }                               
}                      

int PopupButton::GetSelectedItem()
{
    if (menu) {
	Rectangle r= menu->GetSelection();
	if (r != gRect0) {
	    VObject *vop= menu->GetItem(r.origin.x, r.origin.y);
	    if (vop)
		return vop->GetId();
	}
    }
    return cIdNone;
}

void PopupButton::DoOnItem(int m, VObject*, Point where)
{
    if (m == 1 || m == 2)
	delay_cnt++;
    else
	delay_cnt= 0;
    if ((delay_cnt > delay || m == 3 ) && menu) {
	int id= menu->Show(where, this);
	if (id != cIdNone) {
	    SetSelectedItem(id);
	    Control(GetId(), id, 0);
	}
	delay_cnt= 0;
    }
}

ostream& PopupButton::PrintOn(ostream &s)
{
    Button::PrintOn(s);
    return s << menu SP;
}

istream& PopupButton::ReadFrom(istream &s)
{
    Button::ReadFrom(s);
    return s >> menu;
}

//---- PopupItem ---------------------------------------------------------------

MetaImpl(PopupItem, (TP(pb), 0));
    
PopupItem::PopupItem(int id, int dfltid, Menu *m, Point g)
						    : VObjectPair(0, 0, g)
{
    left= new TextItem("title:");
    right= new ShadowItem(cIdNone, pb= new PopupButton(id, dfltid, m));
}

PopupItem::PopupItem(int id, int dfltid, char *tit, Menu *m, Point g)
						    : VObjectPair(0, 0, g)
{
    left= new TextItem(tit);
    right= new ShadowItem(cIdNone, pb= new PopupButton(id, dfltid, m));
}

void PopupItem::SetSelectedItem(int id)
{
    pb->SetSelectedItem(id);
}

int PopupItem::GetSelectedItem()
{
    return pb->GetSelectedItem();
}
