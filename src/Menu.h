#ifndef Menu_First
#ifdef __GNUG__
#pragma once
#endif
#define Menu_First

#include "CollView.h"
#include "LineItem.h"
#include "VObjectPair.h"
#include "BorderItems.h"

//---- MenuItem ----------------------------------------------------------------

class MenuItem : public VObjectPair {
    class Menu *nextMenu;
    short subid, lastxpos, enterxpos;
public:
    MetaDef(MenuItem);
    MenuItem(int id= cIdNone, VObject *v= 0, class Menu *nm= 0);
    ~MenuItem();

    void SetContainer(VObject *v);
    Menu *ContMenu()
	{ return nextMenu; }
    void Enable(bool b= TRUE, bool redraw= TRUE);
    VObject *Detect(BoolFun find, void *arg);
    void *Hit();
    virtual void DoOnItem(int, VObject*, Point);
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
    void Parts(Collection*);
};

//---- MenuLineItem -----------------------------------------------------------------

class MenuLineItem : public LineItem {
public:
    MetaDef(MenuLineItem);
    MenuLineItem();
    void Enable(bool, bool);
};

//---- Menu --------------------------------------------------------------------

enum MenuOptions {
    eMenuTitle      = BIT(eCVLast+1),       // show title
    eMenuIsNew      = BIT(eCVLast+2),
    eMenuDelTitle   = BIT(eCVLast+3),
    eMenuNoScroll   = BIT(eCVLast+4),
    eMenuDefault    = eMenuTitle,
    eMenuLast       = eCVLast + 4
};

class Menu : public CollectionView {
    static int level;
    VObject *title;
    int selection;
protected:
    class BlankWin *window;
    class Clipper *clipper;

    void InsertVItems(int atId, bool before, char *first, va_list ap);

public:
    MetaDef(Menu);
    Menu(char *t, bool s= FALSE, int r= 0, int c= 1, bool st= TRUE);
    Menu(VObject *t, bool s= FALSE, int r= 0, int c= 1, bool st= TRUE);
    ~Menu();

    //---- initialize
    void Init(VObject *t, bool s, bool st);

    //---- misc
    bool IsNew()
	{ return TestFlag(eMenuIsNew); }
    VObject *GetTitle()
	{ return title; }
    void Update();
    void InspectorId(char *buf, int sz);

    //---- show/hide
    virtual int Show(Point, VObject *v= 0);
    virtual Point InitialPos();
    void Close();

    //---- submenues
    void AppendMenu(Menu*, int id= 999);

    //---- inserting items
    void Insert(VObject*);
    void InsertBefore(int id, VObject*);
    void InsertAfter(int id, VObject*);
    void InsertItemBefore(int atId, char *, int myId);
    void InsertItemAfter(int atId, char *, int myId);
    void InsertItemsBefore(int atId, ...);
    void InsertItemsAfter(int atId, ...);
    void InsertItem(char*, int);
    VObject *MakeMenuItem(char *s, int id);

    //---- adding items
    void Append(VObject*);
    void AppendItem(char*, int);
    void AppendItems(char *, ...);

    //---- replacing items
    void ReplaceItem(int id, char *label);
    void ToggleItem(int id, bool toggle, char *toggleTrue, char *toggleFalse);
    void ToggleItem(int id, bool toggle, char *fmt, char *toggleTrue, char *toggleFalse);

    //---- removing items
    VObject *RemoveItem(int id);
    void RemoveAllItems(bool free= TRUE);
    
    //---- enable/disable items
    void EnableItem(int, bool b= TRUE);
    void EnableItems(int id, ...);
    void DisableAll();
    void DisableItem(int id)
	{ EnableItem(id, FALSE); }

    //---- set selected item
    int GetSelectedId()
	{ return selection; }
    void SetSelectedItem(int);

    //---- handling input
    void DoSelect2(Rectangle, int clicks);
    void DoOnItem(int, VObject*, Point);

    //---- find items
    VObject *Detect(BoolFun find, void *arg);
    Menu *FindMenuItem(int);

    //---- input/output
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

//---- ShadowBorderItem --------------------------------------------------------

class ShadowBorderItem: public AbstractBorderItem {
public:
    MetaDef(ShadowBorderItem);
    
    ShadowBorderItem(VObject *title, VObject *inner, int shadow= 5);
    ShadowBorderItem(char *title, VObject *inner, int shadow= 5);
    ShadowBorderItem(VObject *inner, int shadow= 5);
    
    void Draw(Rectangle);
};

#endif Menu_First
