#ifndef PopupItem_First
#ifdef __GNUG__
#pragma once
#endif
#define PopupItem_First

#include "Buttons.h"
#include "VObjectPair.h"

//---- PopupButton -------------------------------------------------------------

class PopupButton: public Button {
protected:
    Menu *menu;
    int delay;
public:
    MetaDef(PopupButton);
    PopupButton(int id, int dfltid, Menu *m= 0, int dl= 0);
    ~PopupButton();
    Metric GetMinSize();
    int Base();
    void SetSelectedItem(int id);
    int  GetSelectedItem();
    void ItemSelected();
    void DoOnItem(int m, VObject*, Point where);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- PopupItem ---------------------------------------------------------------

class PopupItem: public VObjectPair {
protected:
    PopupButton *pb;
public:
    MetaDef(PopupItem);
    PopupItem(int id, int dfltid, Menu *m= 0, Point gap= gPoint8);
    PopupItem(int id, int dfltid, char *title, Menu *m= 0, Point gap= gPoint8);
    virtual void SetSelectedItem(int id);
    virtual int GetSelectedItem();
};

#endif PopupItem_First
