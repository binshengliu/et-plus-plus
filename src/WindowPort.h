#ifndef WindowPort_First
#ifdef __GNUG__
#pragma once
#endif
#define WindowPort_First

#include "Port.h"
#include "WindowSystem.h"

enum WinState {
    eWsHidden,    // window allocated but not in hierarchy
    eWsShown,     // window is inserted in window hierarchy
    eWsClosed     // window will be removed
};

class WindowPort: public Port {
protected:
public:
    /* cursor stuff */
    Token pushbacktoken;
    bool havepushbacktoken;
    bool done;
    bool block;
    bool overlay;
    GrCursor cursor;
    void *privdata;
    InpHandlerFun ihf;
    WinState state;
    Rectangle *invalRects;          // array of inval rects
    Rectangle invalBounds;
    int inval;                      // current number of invalidated rects
    Point lastpos;
    
    //---- overridden ----------------------------------------------------
    void DevStrokeRect(int, Rectangle*);
    void DevStrokeRRect(int, Rectangle*, Point);
    void DevFillRRect(Rectangle*, Point);
    void DevStrokeOval(int, Rectangle*);
    void DevFillOval(Rectangle*);
    void DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point);
    void DevStrokeWedge(int, GrLineCap, Rectangle*, int, int);
    void DevFillWedge(Rectangle*, int, int);
    void DevStrokePolygon(Rectangle *r, Point*, int, GrPolyType t, int, GrLineCap);
    void DevFillPolygon(Rectangle *r, Point*, int, GrPolyType t);
    bool DevImageCacheBegin(ImageCache*, Rectangle);
    void DevImageCacheEnd(ImageCache*);
    //--------------------------------------------------------------------
    
    //---- device dependent stuff ----------------------------------------
    virtual void DevStrokeRect2(int, Rectangle*);
    virtual void DevStrokeRRect2(int, Rectangle*, Point);
    virtual void DevFillRRect2(Rectangle*, Point);
    virtual void DevStrokeOval2(int, Rectangle*);
    virtual void DevFillOval2(Rectangle*);
    virtual void DevStrokeLine2(int, Rectangle*, GrLineCap, Point, Point);
    virtual void DevStrokeWedge2(int, GrLineCap, Rectangle*, int, int);
    virtual void DevFillWedge2(Rectangle*, int, int);
    virtual void DevStrokePolygon2(Rectangle*, Point*, int, GrPolyType, int, GrLineCap);
    virtual void DevFillPolygon2(Rectangle*, Point*, int, GrPolyType);

    virtual void DevFullscreen(bool);
    virtual void DevScrollRect(Rectangle, Point);
    virtual void DevHide();
    virtual void DevShow(WindowPort *father, Rectangle);
    virtual void DevHide1();
    virtual void DevShow1(WindowPort *father, Rectangle);
    virtual void DevGrab(bool, bool);
    virtual void DevGetEvent(Token *t, int timeout, bool overread);    
    virtual void DevSetMousePos(Point, bool);
    virtual void DevBell(long d);
    virtual void DevTop(bool);
    virtual void DevSetCursor(GrCursor);
    virtual void DevSetExtent(Point);
    virtual void DevSetOrigin(Point);
    virtual void DevImageCacheEnd2(DevBitmap*, Rectangle*);
    virtual void DevImageCacheCopy2(DevBitmap*, Rectangle*, Point);
    virtual Rectangle DevGetRect();
    virtual void DevSetTitle(char *name);
    //---- end of device dependent stuff ---------------------------------

    void Damage(EventFlags f, Rectangle *r);
    Point DrawArrow(int, Point, Point);

public:
    MetaDef(WindowPort);
    WindowPort(InpHandlerFun ih, void *p1, bool ov, bool bl, bool wm);
    // WindowPort() { } // g++ ???
    ~WindowPort();

    virtual void SendInput(Token *t);
	
    //---- window management ---------------------------------------------
    void Top()
	// bring window to top of window stack
	{ DevTop(TRUE); }
    void Bottom()
	// bring window to bottom of window stack
	{ DevTop(FALSE); }
    void Hide();
	// remove a window temporary from the screen
    void Show(WindowPort *father, Rectangle);
	// reinsert a previously hidden window

    //---- size management -----------------------------------------------
    Rectangle GetRect();
    void SetOrigin(Point);
    void SetExtent(Point);

    //---- scrolling -------------------------------------------------------
    void ScrollRect(Rectangle,  Point);
    void InvalidateRect(Rectangle r);

    //---- input handling ------------------------------------------------
    void GetEvent(Token *t, int timeout= -1, bool overread= TRUE);    
				    // read next event
	
    void PushEvent(Token t); 
	
    void Grab(bool m, bool fs);     // get all input events (fs = fullscreen)

    void SetMousePos(Point p)       // set the mouse position in window
	{ DevSetMousePos(p, FALSE); }      // relative coordinates
	
    void MoveMousePos(Point delta)  // move the mouse position
	{ DevSetMousePos(delta, TRUE); }
	
    void Bell(long d)               // ring the bell
	{ DevBell(d); }
       

    //---- cursor stuff --------------------------------------------------
    GrCursor GetCursor();
    GrCursor SetWaitCursor(unsigned int d= 400, GrCursor c= eCrsHourglass);
    GrCursor SetCursor(GrCursor);
};

#endif WindowPort_First
