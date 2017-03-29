#ifndef NeWSWindowPort_First
#define NeWSWindowPort_First

#include "WindowPort.h"

class NeWSWindowPort: public WindowPort {
public:
    int psid, bcnt;
    FontPtr lbfont, lfont;
    byte bbuf[200];
    
public:

    NeWSWindowPort(InpHandlerFun, void*, bool, bool);

    //---- window management
    void DevShow(WindowPort *father, Rectangle);
    void DevHide();
    void DevDestroy2();
    void DevSetRect(Rectangle*);
    void DevTop(bool);

    //---- clipping
    void DevClip(Rectangle, Point);
    void DevResetClip();
    
    //---- graphic
    void DevStrokeLine2(GrPattern, int, Rectangle*, GrLineCap, Point, Point);
    void DevStrokeRect2(GrPattern, int, Rectangle*);
    void DevFillRect(GrPattern, Rectangle*);
    void DevStrokeRRect2(GrPattern, int, Rectangle*, Point);
    void DevFillRRect2(GrPattern, Rectangle*, Point);
    void DevStrokeOval2(GrPattern, int, Rectangle*);
    void DevFillOval2(GrPattern, Rectangle*);
    void DevStrokeWedge2(GrPattern, int, GrLineCap, Rectangle*, int, int);
    void DevFillWedge2(GrPattern, Rectangle*, int, int);
    void DevStrokePolygon(Rectangle*, GrPattern, Point*, int, GrPolyType, int, GrLineCap);
    void DevFillPolygon(Rectangle*, GrPattern, Point*, int, GrPolyType);
    void DevShowBitmap(GrPattern, Rectangle*, struct Bitmap*);
    bool DevShowChar(FontPtr fdp, Point pos, byte c, bool isnew, Point);
    void DevShowTextBatch(GrPattern, Rectangle*, Point);

    //---- scrolling
    void DevScrollRect(Rectangle, Point);
	
    //---- input
    void DevGetEvent(Token *t, int timeout, bool);    
    void DevGrab(bool, bool);
	
    //---- cursor
    void DevSetCursor(GrCursor);
    
    //---- mouse
    void DevSetMousePos(Point, bool);
    
    //---- misc
    void DevBell(long);
    void DevGiveHint(int, int, void*);
};

#endif NeWSWindowPort_First
