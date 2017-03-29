//$EditTextItem,RestrTextItem$

#include "EditTextItem.h"
#include "CheapText.h"
#include "RestrTView.h"
#include "BlankWin.h"
#include "Dialog.h"

//---- EditTextItem ------------------------------------------------------------

MetaImpl(EditTextItem, (T(oldTextSize), TB(freeView), 0));

EditTextItem::EditTextItem(int id, char* it, int w, int l, Point b)
						    : Clipper(0, gPoint0, id)
{
    if (it == 0)
	it= "";
    Init(new TextView(this, gFitRect, 
			    new CheapText((byte*)it),
		       eLeft, eOne, TRUE, eTextViewDefault, b),
	      w, l, it);
    freeView= TRUE;
    SetFlag(eVObjVFixed);
}

EditTextItem::EditTextItem(int id, TextView *t, int w, int l)
						    : Clipper(0, gPoint0, id)
{
    Init(t, w, l, 0);
    freeView= FALSE;
    SetFlag(eVObjVFixed);
    t->SetNextHandler(this);
}

EditTextItem::~EditTextItem()
{
    if (vop && freeView) {
	Text *t= Tv()->GetText();
	SafeDelete(t);
	SafeDelete(vop);
    }
}

void EditTextItem::Init(TextView *tv, int w, int l, char *it)
{
    Point me;

    vop= tv;
    vop->SetFlag(eVObjLayoutCntl | eTextNoFind | eViewNoPrint);
    Tv()->SetStopChars(l > 1 ? "\r" : "\r\n");
    me.x= w ? w : ((it) ? tv->GetMinSize().extent.x : 100);
    me.y= l*TextViewLineHeight(Tv()->GetText()->GetFont(), Tv()->GetSpacing());
    SetMinExtent(me+2*gBorder);
    SetNoSelection();    
    oldTextSize= GetTextSize(); 
}

int EditTextItem::Base()
{
    return vop->Base();
}

Metric EditTextItem::GetMinSize()
{
    return Metric(Clipper::GetMinSize().extent, Base());
}

bool EditTextItem::Validate()
{
    return TRUE;
}

void EditTextItem::SetContainer(VObject *vop)
{
    Clipper::SetContainer(vop);
    DialogView *vp= (DialogView*) FindContainerOfClass(Meta(DialogView));
    Tv()->SetContainer(vop);
    if (vp)
	vp->AddItemWithKbdFocus(this);
}

void EditTextItem::Enable(bool b, bool redraw)
{
    Clipper::Enable(b, redraw);
    Tv()->Enable(b, redraw);
}

Text *EditTextItem::SetText(Text *t)
{
    oldTextSize= GetTextSize();
    return Tv()->SetText(t);
}

void EditTextItem::SetString(byte *str, int len)
{
    Tv()->SetString(str, len);
    oldTextSize= GetTextSize();
    Tv()->RevealAlign(Rectangle(1,1));
    Changed();
}

void EditTextItem::Control(int id, int part, void *op)
{
    if (op == vop) {
	if (part == cPartChangedText) {
	    // notify only on >0 to 0 transition and vice versa
	    int lastTextSize= oldTextSize;
	    oldTextSize= GetTextSize();
	    if ((lastTextSize == 0) == (oldTextSize == 0))
		return;
	}
	id= GetId();
	op= this;
    }
    Clipper::Control(id, part, op);
}

void EditTextItem::SendDown(int id, int part, void *val)
{
    switch (part) {
    case cPartValidate:
	if (val) 
	    *(bool*)val= (bool) ! Validate();
	break;
    case cPartFocusChanged:   
	FocusChanged(id, (bool) val);
	break;
    case cPartHasSelection:  
	*(bool*)val= Tv()->HasSelection();
	break;
    default:
	Tv()->SendDown(id, part, val);
	Clipper::SendDown(id, part, val);
    }
}

void EditTextItem::FocusChanged(int id, bool dontRedraw)
{
    if (id == cIdStartKbdFocus)
	SetSelection(0, cMaxInt, !dontRedraw);
    else {
	SetNoSelection(!dontRedraw);
	if (!Validate()) {
	    UpdateEvent();
	    Token t= gToken;
	    t.Flags|= eFlgButDown;
	    GetWindow()->PushBackEvent(t);
	}
    }
}

//---- RestrTextItem -----------------------------------------------------------

MetaImpl0(RestrTextItem);

RestrTextItem::RestrTextItem(int id, RegularExp *rex, char* initText, int width, int lines)
    : EditTextItem(id,
	new RestrTextView((View*)0, rex, gFitRect,
	    new CheapText((byte*)(initText ? initText : ""))), width, lines)
{
}
