#ifndef MenuBar_First
#ifdef __GNUG__
#pragma once
#endif
#define MenuBar_First

#include "Clipper.h"
#include "Menu.h"
#include "CollView.h"

class MenuBar: public CollectionView {
public:
    MetaDef(MenuBar);
    MenuBar(EvtHandler*, Collection*);
    Command *DoKeyCommand(int, Point, Token);
};

class PullDownItem: public CompositeVObject {
    class PullDownMenu *menu;
public:
    MetaDef(PullDownItem);
    PullDownItem(class PullDownMenu *m= 0);
    ~PullDownItem();
    void DoOnItem(int, VObject*, Point);
    void Parts(Collection*);
};

class PullDownMenu: public Menu {
    Rectangle exitRect;
public:
    MetaDef(PullDownMenu);
    PullDownMenu(char *title, bool s= FALSE, int r= 0, int c= 1);
    PullDownMenu(VObject *title, bool s= FALSE, int r= 0, int c= 1);
    void SetExitRect(Rectangle er) 
	{ exitRect= er; }
    void DoOnItem(int item, VObject *vop, Point p);
    Point InitialPos();
};

#endif MenuBar_First
