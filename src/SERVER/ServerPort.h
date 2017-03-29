#ifndef GrServer_First
#define GrServer_First

#include "WindowPort.h"
#include "Server.h"

const int MQS= 100;

class ServerPort: public WindowPort {
    int ref; 
    Response rps[MQS+1];
    int in, out;

public:    
    Response *Dequeue();
    void Enqueue(Response*);
public:
    ServerPort(InpHandlerFun, void*, bool, bool);

    void DevDestroy2();
    void DevClip(Rectangle, Point);
    void DevResetClip();
    void DevStrokeLine(Ink, int, Rectangle*, GrLineCap, Point, Point);
    void DevStrokeRect(Ink, int, Rectangle*);
    void DevFillRect(Ink, Rectangle*);
    void DevStrokeRRect(Ink, int, Rectangle*, Point);
    void DevFillRRect(Ink, Rectangle*, Point);
    void DevStrokeOval(Ink pat,  int ps, Rectangle *r);
    void DevFillOval(Ink pat, Rectangle *r);
    void DevStrokeWedge(Ink, int, GrLineCap, Rectangle*, int, int);
    void DevFillWedge(Ink, Rectangle*, int, int);
    void DevStrokePolygon(Rectangle*, Ink, Point*, int, GrPolyType, int, GrLineCap);
    void DevFillPolygon(Rectangle*, Ink, Point*, int, GrPolyType);
    void DevShowBitmap(Ink, Rectangle*, struct Bitmap*);
    bool DevShowChar(FontPtr, Point, byte, bool, Point);
    void DevShowTextBatch(Ink, Rectangle*, Point);
    void DevGiveHint(int, int, void*);
    void DevSetCursor(GrCursor);
    void DevHide();
    void DevShow(WindowPort *father, Rectangle);
    void DevSetRect(Rectangle*);
    void DevGrab(bool, bool);
    void DevScrollRect(Rectangle, Point);
    void DevTop(bool);
    void DevGetEvent(Token *t, int timeout, bool overread);    
    void DevSetMousePos(Point, bool);
    void DevBell(long);   
};

#endif GrServer_First
