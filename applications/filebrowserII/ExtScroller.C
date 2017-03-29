//$ExtScrollBar,ExtScroller,myScrollBarButton$

#include "ExtScroller.h"
#include "BrowserCmdNo.h"
#include "Buttons.h"
#include "SeqColl.h"
#include "Menu.h"

static u_short funcBits[]= {
#   include "images/FuncImage.im"
};

static u_short funcBitsInv[]= {
#   include "images/FuncImageInv.im"
};

static Bitmap *funcImage, *funcImageInv; 

MetaImpl0(ExtScrollBar);

class myScrollBarButton: public ImageButton {
    Menu *markMenu;
public:
    myScrollBarButton(Menu *m, Bitmap *b1, Bitmap *b2);
    void DrawInner(Rectangle r, bool highlight);
    void DoOnItem(int, VObject*, Point);
    Metric GetMinSize()
	{ return ImageButton::GetMinSize().extent + gPoint4; }
};

myScrollBarButton::myScrollBarButton(Menu *m, Bitmap *b1, Bitmap *b2)
				    : ImageButton(cIdFuncButton, b1, b2, TRUE)
{
    markMenu= m;
    SetFlag(eVObjHFixed | eVObjVFixed); 
}

void myScrollBarButton::DoOnItem(int mode, VObject*, Point where)
{
    if (mode == 1) {
	Control(GetId(), cPartSetupFunc, 0);
	int id= markMenu->Show(where, this);
	Control(GetId(), id, 0);
    }
}

void myScrollBarButton::DrawInner(Rectangle r, bool highlight)
{
    contentRect= contentRect.Inset(2);
    ImageButton::DrawInner(r, highlight);
    contentRect= contentRect.Expand(2);
}

ExtScrollBar::ExtScrollBar(Menu *m, Direction d): (cIdNone, d)
{
    SeqCollection *sp= Guard(list, SeqCollection);
    if (funcImage == 0) {
	funcImage= new Bitmap(16, funcBits);
	funcImageInv= new Bitmap(16, funcBitsInv); 
    }
    ImageButton *b= new myScrollBarButton(m, funcImage, funcImageInv);
    b->SetContainer(this);
    sp->AddFirst(b);
}

MetaImpl0(ExtScroller);

ExtScroller::ExtScroller(Menu *m, VObject *v, Point e, int id, ScrollDir)
					: Scroller(v, e, id, eScrollDefault)
{
    sb[0]= new ExtScrollBar(m, eVert);
    sb[0]->SetContainer(this);    
    SeqCollection *sp= Guard(list, SeqCollection);
    Object *op= sp->RemoveFirst();
    sp->AddFirst(sb[0]);
    SetModified();
    SafeDelete(op);
}
