//$MenuBar,PullDownItem,PullDownMenu$
#include "MenuBar.h"
#include "OrdColl.h"

//---- MenuBar -------------------------------------------------------------

MetaImpl0(MenuBar);

MenuBar::MenuBar(EvtHandler *h, class Collection *l)
    : CollectionView(h, l, (CollViewOptions)(eCVClearSelection+eCVDontStuckToBorder), 1, 0)
{
    SetGap(Point(6, 0));
}

Command *MenuBar::DoKeyCommand(int, Point, Token)
{
    return gNoChanges;
}

//---- PullDownItem ------------------------------------------------------------

MetaImpl(PullDownItem, (TP(menu), 0));

PullDownItem::PullDownItem(PullDownMenu *m)
			: CompositeVObject(cIdNone, m ? m->GetTitle() : 0, 0)
{
    menu= m;
}

PullDownItem::~PullDownItem()
{
    SafeDelete(menu);
}

void PullDownItem::DoOnItem(int m, VObject*, Point)
{
    if (m == 1 && menu) {
	menu->SetExitRect(Rectangle(-6, -contentRect.extent.y-2,
				  contentRect.extent.x+12, contentRect.extent.y));
	//SetSelectedItem(gPoint_1);
	menu->SetNoSelection();
	UpdateEvent();
	int cmdno= menu->Show(contentRect.SW()+Point(2,3), this);
	if (cmdno >= 0)
	    PerformCommand(DoMenuCommand(cmdno));
    }
}

void PullDownItem::Parts(Collection* col)
{
    CompositeVObject::Parts(col);
    if (menu)
	col->Add(menu);
}

//---- PullDownMenu ------------------------------------------------------------

MetaImpl(PullDownMenu, (T(exitRect), 0));

PullDownMenu::PullDownMenu(char *title, bool s, int r, int c)
						: Menu(title, s, r, c, FALSE)
{
}

PullDownMenu::PullDownMenu(VObject *title, bool s, int r, int c)
						: Menu(title, s, r, c, FALSE)
{
}

void PullDownMenu::DoOnItem(int m, VObject *vop, Point p)
{
    if (p.y < -2 && !exitRect.ContainsPoint(p))
	Close();
    else
	Menu::DoOnItem(m, vop, p);
}

Point PullDownMenu::InitialPos()
{
    clipper->Scroll(cPartScrollAbs, gPoint0, FALSE);
    return gPoint0;
}
