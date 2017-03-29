#ifndef XWindowPort_First
#define XWindowPort_First

#include "X11.h"
#include "WindowPort.h"

class XWindowPort: public WindowPort {
public:
    XWindow     id;     // current drawable id
    XWindow     winid;  // window id
    GC          gc;     // window graphic context
    XWindowPort *father;
    bool        ismapped;
    bool        isvisible;
    Rectangle   rect;
    bool        dontgrab;
    XColormap   cmap;
    bool        isicon;
    bool        solid;

public:
    MetaDef(XWindowPort);
    bool MapEvent(XEvent &xe, Token *t= 0);
    void SetLine(int psz);
    Point getMousePos(unsigned int&);
    friend void Allow();

public:
    XWindowPort(InpHandlerFun, void*, bool, bool, bool);
    ~XWindowPort();
    
    XWindow GetWinId()
	{ return winid; } 

    //---- window management
    void DevShow(WindowPort *father, Rectangle);
    void DevHide();
    void DevTop(bool);
    void DevSetExtent(Point);
    void DevSetOrigin(Point);
    Rectangle DevGetRect();
    
    //---- little help for the window manager (if any)
    void DevSetTitle(char *name);

    //---- clipping
    void DevClip(Rectangle, Point);
    
    //---- graphic
    void DevStrokeLine2(int, Rectangle*, GrLineCap, Point, Point);
    void DevStrokeRect2(int, Rectangle*);
    void DevFillRect(Rectangle*);
    void DevStrokeRRect2(int, Rectangle*, Point);
    void DevFillRRect2(Rectangle*, Point);
    void DevStrokeOval2(int, Rectangle*);
    void DevFillOval2(Rectangle*);
    void DevStrokeWedge2(int, GrLineCap, Rectangle*, int, int);
    void DevFillWedge2(Rectangle*, int, int);
    void DevStrokePolygon2(Rectangle*, Point*, int, GrPolyType t, int psz, GrLineCap cap);
    void DevFillPolygon2(Rectangle*, Point*, int, GrPolyType);
    void DevShowBitmap(Rectangle*, struct Bitmap*);
    bool DevShowChar(FontPtr fdp, Point pos, byte c, bool isnew, Point);
    void DevShowTextBatch(Rectangle*, Point);
    void DevSetPattern(struct DevBitmap*);
    bool DevSetColor(RGBColor*);
    void DevSetOther(int);

    //---- scrolling
    void DevScrollRect(Rectangle, Point);
	
    //---- input
    void DevGrab(bool, bool);
    void DevGetEvent(Token *t, int timeout, bool);    
    
    //---- cursor
    void DevSetCursor(GrCursor);
    
    //---- mouse
    void DevSetMousePos(Point, bool);
    
    //---- misc
    void DevBell(long);
    void DevGiveHint(int, int, void*);
    void DevImageCacheEnd2(DevBitmap*, Rectangle*);
    void DevImageCacheCopy2(DevBitmap*, Rectangle*, Point);
};

#endif XWindowPort_First
