#ifndef XBitmap_First
#define XBitmap_First

#include "X11.h"
#include "DevBitmap.h"

class XBitmap: public DevBitmap {
friend class XWindowPort;
    Pixmap pm;
    XImage *ximage;
    GC gc;
    bool inserver, changed;

private:
    void CreatePixmap();

public:
    XBitmap(Point sz, u_short *data, u_short depth);
    XBitmap(Point sz, Pixmap pm);
    ~XBitmap();
    
    Pixmap GetPixmap();
    XImage *GetImage();
    void SetPixel(u_int x, u_int y, int value);
    int GetPixel(u_int x, u_int y);
};

#endif XBitmap_First


