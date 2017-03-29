#ifndef CollView_First
#ifdef __GNUG__
#pragma once
#endif
#define CollView_First

#include "View.h"

//---- CollectionView options --------------------------------------------------

enum CollViewOptions {
    eCVGrid             = BIT(eViewLast+1),
    eCVDontStuckToBorder= BIT(eViewLast+2),
    eCVClearSelection   = BIT(eViewLast+3),
    eCVInvertSelection  = BIT(eViewLast+4),
    eCVExpandRows       = BIT(eViewLast+5),
    eCVExpandCols       = BIT(eViewLast+6),
    eCVModified         = BIT(eViewLast+7),
    eCVDefault          = 0,
    eCVLast             = eViewLast + 7
};

//---- CollectionView ----------------------------------------------------------

class CollectionView: public View {
protected:
    class Collection *coll;
    Rectangle selection;
    Point gap, minExtent;
    short *xPos, *yPos, rows, cols;
    VObject *defaultItem;
    Menu *menu;

public:
    MetaDef(CollectionView);

    CollectionView(EvtHandler*, class Collection *, CollViewOptions o= eCVDefault,
							   int r= 0, int c= 1);
    ~CollectionView();

    void SetCollection(class Collection*, bool freeold= TRUE);
    class Collection *GetCollection()
	{ return coll; }
    void SetDefaultItem(class VObject *d);

    //---- layout
    void SetMinExtent(Point);
    Metric GetMinSize();
    void SetOrigin(Point at);
    void SetGap(Point g)
	{ gap= g; }
    void Update();
	
    //---- drawing
    void Draw(Rectangle);
    virtual void DrawGrid(Point from, Point to);
    
    //---- event handling
    Command *DoLeftButtonDownCommand(Point, Token, int);
    Command *DoKeyCommand(int, Point, Token);
    virtual void DoOnItem(int, VObject*, Point);
    virtual void DoSelect(Rectangle sel, int clicks);
    virtual void DoSelect2(Rectangle sel, int clicks);

    //---- updating (change propagation)
    void DoObserve(int, int, void*, Object*);
    void Modified();
    
    //---- menus
    void SetMenu(Menu *m)
	{ menu= m; }
    Menu *GetMenu();
    void DoCreateMenu(Menu *);
    
    //---- selection
    Rectangle GetSelection()
	{ return selection; }
    int SetSelection(Rectangle);
    void SetNoSelection()
	{ SetSelection(gRect0); }
	
    //---- mapping
    Point PointToItem(Point, bool *outside= 0);
    Rectangle ItemRect(int, int);
    class VObject *GetItem(int x, int y);
    Point ItemPos(class VObject *);
    Rectangle ItemRect(Rectangle r);
    void InvalidateItemRect(Rectangle r)
	{ InvalidateRect(ItemRect(r)); }
    void ConstrainScroll(Point*);

    //---- input/output
    ostream& PrintOn (ostream&);
    bool PrintOnWhenObserved(Object*);
    istream& ReadFrom(istream&);
    
    //---- inspecting
    void Parts(Collection*);
};

//---- CellSelector ------------------------------------------------------------

class CellSelector: public Command {
    CollectionView *lvp;
    VObject *itemptr;
    Point item;
    int clickCount;
public:
    CellSelector(CollectionView*, int clicks);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

#endif CollView_First

