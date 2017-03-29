#ifndef GrSunWindow_First
#define GrSunWindow_First

#include "WindowPort.h"
#include "SunWindowSystem.h"

enum InkMode {
	eInkXor,
	eInkNone,
	eInkBlack,
	eInkWhite,
	eInkPat,
	eInkColor
};

class SunWindowPort: public WindowPort {
public:
    SunWindowPort *next;
    int fd, planes;
    Point inpOffset;
    Point fsorigin;
    struct pixwin *pw, *opw, *rpw;
    struct pixrect *pr, *opr, *rpr;
    SunWindowPort *fatherport;
    struct pw_pixel_cache *savepr;
    bool inbatch;
    Rectangle myrect;
    
    int tmode, cmode;
    InkMode ink;
    class SunBitmap *patbm;
    struct pixrect *pat;
    Point patorigin;
    struct suncolormap *cmap;

private:
    void lock(Rectangle *r);
    void unlock();
    void Replrop(int, int, int, int);
    void Rop(int, int, int, int);
    void BitBlt(int x, int y, int w, int h);
    void Traprop(int l, int t, int w, int h, int radx);
    void Vector(int x1, int y1, int x2, int y2);
    void Polygon(int n, struct pr_pos *pts, int l, int t);
    void line(int psz, int x0, int y0, int x1, int y1);
    void PolyDot(pixrect *pr, int dx, int dy, pr_pos *pts, int n, int psz);
    void PolyEllipse(pixrect *pr1, int x0, int y0, int x1, int x2,
					int y1, int y2, int a, int b, int psz);
    void Bresenham(pixrect *pr1, int fromX, int fromY, int X2, int Y2, int psz);
    void flushcolormap()
	{ if (cmap) cmap->install(pw); }
    pixrect *GetPattern(Point e);
    void Stencil(Rectangle *r, pixrect *pr);
    int AlignX(int x)
	{ return (x-origin.x) % patorigin.x; }
    int AlignY(int y)
	{ return (y-origin.y) % patorigin.y; }
    int Align2X(int x)
	{ return x-origin.x; }
    int Align2Y(int y)
	{ return y-origin.y; }
    void Notify(SysEventCodes, int);

public:
    MetaDef(SunWindowPort);
    SunWindowPort(InpHandlerFun, void*, bool, bool, bool);
    ~SunWindowPort();
    
    void DevClip(Rectangle, Point);
    void DevStrokeLine2(int, Rectangle*, GrLineCap, Point, Point);
    void DevStrokeRect2(int, Rectangle*);
    void DevFillRect(Rectangle*);
    void DevStrokeRRect2(int, Rectangle*, Point);
    void DevFillRRect2(Rectangle*, Point);
    void DevStrokePolygon2(Rectangle*, Point*, int, GrPolyType, int, GrLineCap);
    void DevFillPolygon2(Rectangle*, Point*, int, GrPolyType);
    void DevShowBitmap(Rectangle*, Bitmap*);
    bool DevShowChar(FontPtr, Point, byte, bool, Point);
    void DevShowTextBatch(Rectangle*, Point);
    void DevSetPattern(DevBitmap*);
    bool DevSetColor(RGBColor*);
    void DevSetOther(int);
    void DevGiveHint(int, int, void*);
    
    void DevSetCursor(GrCursor);

    void DevGrab(bool, bool);
    
    void DevScrollRect(Rectangle, Point);

    void DevTop(bool);
    void DevHide();
    void DevShow(WindowPort *father, Rectangle);

    Rectangle DevGetRect();
    void DevSetExtent(Point e);
    void DevSetOrigin(Point o);
    
    void DevGetEvent(Token *t, int timeout, bool);    
    void DevSetMousePos(Point, bool);

    void DevBell(long);
      
    void DevImageCacheEnd2(DevBitmap*, Rectangle*);
    void DevImageCacheCopy2(DevBitmap*, Rectangle*, Point);
    void TestDamage();
};

#endif GrSunWindow_First
