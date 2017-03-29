//$CloseBox,Window,WindowBorderItem$
#include "Window.h"

#include "Error.h"
#include "System.h"
#include "Menu.h"
#include "CmdNo.h"
#include "String.h"
#include "Buttons.h"
#include "WindowSystem.h"
#include "WindowPort.h"
#include "ObjectTable.h"

const int cIdBottomBox= 7789;

//---- CloseBox ----------------------------------------------------------------
  
class CloseBox: public Button {
public:
    MetaDef(CloseBox);
    CloseBox(int id) : Button(id, 0)
	{ SetFlag(eVObjHFixed|eVObjVFixed); }
    Metric GetMinSize()
	{ return Metric(10, 10, 9); }
    int Base()
	{ return 9; }
    void Draw(Rectangle)
	{ GrSetPenNormal(); GrStrokeRect(contentRect); }
};
   
MetaImpl0(CloseBox);

//---- WindowBorderItem --------------------------------------------------------------

MetaImpl(WindowBorderItem, (TB(fixed), 0));

static u_short StretchBoxNEBits[]= {
#   include "images/StretchBoxNE.image"
};

static u_short StretchBoxNWBits[]= {
#   include "images/StretchBoxNW.image"
};

static u_short StretchBoxSEBits[]= {
#   include "images/StretchBoxSE.image"
};

static u_short StretchBoxSWBits[]= {
#   include "images/StretchBoxSW.image"
};

static Bitmap *StretchBoxNW, *StretchBoxNE, *StretchBoxSW, *StretchBoxSE;
static RGBColor *windowHighlightColor;
    
WindowBorderItem::WindowBorderItem(bool fix, VObject *title, VObject *inner)
				  : AbstractBorderItem(title, inner, 1, cIdNone)
{
    fixed= fix;
    border= cBorder;
    titleGap= cTitleGap;
}

Rectangle WindowBorderItem::TitleRect()
{
    Rectangle r= contentRect;
    r.extent.y= 16+22;
    return r.Inset(Point(10,8));
}

void WindowBorderItem::Draw(Rectangle r)
{
    if ((GrHasColor() && GetWindow()->TestFlag(eBWinActive)) || gPrinting) {
	if (windowHighlightColor == 0)
	    ObjectTable::AddRoot(windowHighlightColor= new RGBColor ((float)0.8));
	GrPaintRect(TitleRect(), windowHighlightColor);
    }
    
    if (! interior->contentRect.ContainsRect(r)) {
	GrSetPenNormal();
	GrSetPenSize(2);
	GrStrokeRect(contentRect);
	GrSetPenSize(1);
	if (! fixed) {
	    if (StretchBoxNW == 0)
		ObjectTable::AddRoots(
		    StretchBoxNW= new Bitmap(8, StretchBoxNWBits),
		    StretchBoxNE= new Bitmap(8, StretchBoxNEBits),
		    StretchBoxSW= new Bitmap(8, StretchBoxSWBits),
		    StretchBoxSE= new Bitmap(8, StretchBoxSEBits),
		    0
		);
	    GrPaintBitMap(Rectangle(contentRect.NW()+gPoint2, 8),
						    StretchBoxNW, ePatBlack);
	    GrPaintBitMap(Rectangle(contentRect.NE()+Point(-9,2), 8),
						    StretchBoxNE, ePatBlack);
	    GrPaintBitMap(Rectangle(contentRect.SE()+Point(-9,-9), 8),
						    StretchBoxSE, ePatBlack);
	    GrPaintBitMap(Rectangle(contentRect.SW()+Point(2,-9), 8),
						    StretchBoxSW, ePatBlack);
	}
    }
    AbstractBorderItem::Draw(r);

    if (!GrHasColor() && GetWindow()->TestFlag(eBWinActive))
	GrInvertRect(TitleRect());
}

Command *WindowBorderItem::DoLeftButtonDownCommand(Point p, Token t, int cl)
{
    if (Rectangle(contentRect.NW()+gPoint2, 8).ContainsPoint(p)
	    || Rectangle(contentRect.NE()+Point(-9,2), 8).ContainsPoint(p)
	    || Rectangle(contentRect.SE()+Point(-9,-9), 8).ContainsPoint(p)
	    || Rectangle(contentRect.SW()+Point(2,-9), 8).ContainsPoint(p)) {
	t.Flags|= eFlgCntlKey;
    }
    return VObject::DoLeftButtonDownCommand(p, t, cl);
}

ostream& WindowBorderItem::PrintOn(ostream &s)
{
    AbstractBorderItem::PrintOn(s);
    return s << fixed SP;
}

istream& WindowBorderItem::ReadFrom(istream &s)
{
    AbstractBorderItem::ReadFrom(s);
    return s >> Enum(fixed);
}

//---- Window ------------------------------------------------------------------

MetaImpl(Window, (TP(menu), 0));

Window::Window(EvtHandler *eh, Point e, WindowFlags f, VObject *in, char *title)
				      : BlankWin(eh, in, gPoint_1, (BWinFlags)f)
{
    contentRect.extent= e;
    
    if (gWindowSystem->HasExternalWindowManager() && !TestFlag(eBWinBlock)) {
	vop= new BorderItem(in, gPoint4, 0);
    } else {
    vop= new TextItem(cIdWindowTitle, title == 0 ? "" : title);
    vop->SetFlag(eVObjHFixed);
    
    if (! TestFlag(eBWinFixed)) {
	vop= new Cluster(cIdNone, eVObjVBase, gPoint0,
	    new CloseBox(cIdCloseBox),
	    vop,
	    new CloseBox(cIdBottomBox),
	    0
	);
    }
	
    vop= new WindowBorderItem(TestFlag(eBWinFixed), vop, in);
    }
}

Window::~Window()
{
    SafeDelete(menu);
}

class WindowPort *Window::MakePort()
{
    if (portDesc == 0) {
	portDesc= gWindowSystem->MakeWindow((InpHandlerFun) &BlankWin::input, this,
		TestFlag(eBWinOverlay), TestFlag(eBWinBlock),
			!gWindowSystem->HasExternalWindowManager());
	portDesc->cursor= cursor;
    }
    return portDesc;
}

Command *Window::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    if (!gWindowSystem->HasExternalWindowManager() && !TestFlag(eBWinOverlay) && (t.Code == eEvtExit || t.Code == eEvtEnter)) {
	InvalidateRect(((WindowBorderItem*)vop)->TitleRect());
	SetFlag(eBWinActive, t.Code == eEvtEnter);
    }
    return BlankWin::DispatchEvents(lp, t, vf);
}

void Window::Control(int id, int part, void *vp)
{
    if (id == cIdCloseBox || (id == 555 && part == 555)) {
	if (TestFlag(eWinCanClose))
	    Close();
	else if (TestFlag(eWinDestroy)) {
	    Control(cIdWinDestroyed, part, this);
	    Close();
	    gSystem->AddCleanupObject(this);
	} else 
	    BlankWin::Control(id, part, vp);
    } else if (id == cIdBottomBox)
	Bottom();
    else
	BlankWin::Control(id, part, vp);
}

void Window::SetTitle(char *itsTitle, bool redraw)
{
    if (gWindowSystem->HasExternalWindowManager())
	MakePort()->DevSetTitle(itsTitle);
    else
	Guard(vop, AbstractBorderItem)->SetTitle(itsTitle, redraw);
}

char *Window::GetTitle()
{
    if (gWindowSystem->HasExternalWindowManager())
	return "???";
    return Guard(vop, AbstractBorderItem)->GetTitle();
}

class Menu *Window::GetMenu()
{
    if (menu == 0)
	menu= new Menu("Window");
    return menu;
}
