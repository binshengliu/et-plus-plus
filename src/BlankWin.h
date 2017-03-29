#ifndef BlankWin_First
#ifdef __GNUG__
#pragma once
#endif
#define BlankWin_First

#include "Clipper.h"

extern Token gToken;
extern bool gBatch;
extern class BlankWin *gWindow;


enum BWinFlags {
    eBWinOverlay=   BIT(eClipperLast+1),
    eBWinBlock  =   BIT(eClipperLast+2),
    eBWinActive =   BIT(eClipperLast+3),
    eBWinFixed  =   BIT(eClipperLast+4),
    eBWinWasOpen=   BIT(eClipperLast+5),
    eBWinDefault=   0,
    eBWinLast   =   eClipperLast+5
};

class BlankWin: public Clipper {
protected:
    GrCursor cursor;
    class WindowPort *portDesc;     // Port descriptor
    EvtHandler *nexthandler;

    void Init(EvtHandler *eh, GrCursor curs, BWinFlags f);
    virtual class WindowPort *MakePort();
public:
    MetaDef(BlankWin);
    
    BlankWin(Point minsize= gPoint0, BWinFlags f= eBWinDefault);
    BlankWin(VObject *vp= 0, Point minsize= gPoint0, BWinFlags f= eBWinDefault,
		    Ink *bg= ePatWhite, GrCursor curs= eCrsBoldArrow);
    BlankWin(EvtHandler *eh, VObject *vp= 0, Point minsize= gPoint0,
						BWinFlags f= eBWinDefault);
    ~BlankWin();
    
    EvtHandler *GetNextHandler();
    
    void input(Token*);
    void Update();
    void SetOrigin(Point at);
    void SetExtent(Point e);
    void SetFocus(Rectangle, Point);
    void InvalidateRect(Rectangle);
    void PushBackEvent(Token);
    void UpdateEvent();
    void Open(bool mode= TRUE);
    void OpenAt(Point p, VObject *vf= 0);
    Command *GetMover();
    Command *GetStretcher();
    Command *DoLeftButtonDownCommand(Point, Token, int);
    GrCursor GetCursor(Point);

    PortDesc GetPortDesc()
	{ return (PortDesc)portDesc; }
    void SetPortDesc(PortDesc newport)
	{ portDesc= (class WindowPort*)newport; }
    Rectangle ScreenRect();
    Rectangle GetRect();
    Token ReadEvent(int timeout= -1, bool overread= TRUE);
    void Bell(long d= 100);
    void SetMousePos(Point);
    void Grab(bool on, bool fullscreen);
    void Top();
    void Bottom();

    //---- Menu related methods
    class Menu *GetMenu();
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);
};

//---- BlankWindowStretcher ---------------------------------------------------------

class BlankWindowStretcher: public VObjectStretcher {
    Point ddd;
public:
    BlankWindowStretcher(BlankWin *w, Rectangle r);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void TrackConstrain(Point ap, Point pp, Point *np);
};

#endif BlankWin_First

