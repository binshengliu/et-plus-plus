#include "X11.h"
#include "Port.h"
#include "XBitmap.h"
#include "XWinSystem.h"

XBitmap::XBitmap(Point sz, u_short *im, u_short dep) : DevBitmap(sz, im, dep)
{       
    changed= TRUE;
    inserver= FALSE;
    ximage= 0;
    gc= 0;
    pm= 0;
    if (im) {
	ximage= XCreateImage(display, XDefaultVisual(display, screen), depth,
	      (depth > 1) ? ZPixmap : XYPixmap, 0, (char*) im, size.x, size.y,
							    sizeof(short)*8, 0);
	/* all hardcoded bitmaps (array of short) are generated on a sparc: */
	ximage->byte_order= ximage->bitmap_bit_order= MSBFirst;
	ximage->bitmap_unit= ximage->bitmap_pad= sizeof(short)*8;
    }
}

XBitmap::XBitmap(Point sz, Pixmap p) : DevBitmap(sz, 0, 1)
{
    changed= inserver= TRUE;
    ximage= 0;
    gc= 0;
    pm= p;
}

XBitmap::~XBitmap()
{
    if (pm)
	XFreePixmap(display, pm);
    pm= 0;
    if (ximage)
	XDestroyImage(ximage);
    ximage= 0;
    if (gc)
	XFreeGC(display, gc);
    gc= 0;
}

void XBitmap::CreatePixmap()
{
    if (pm)
	return;
    pm= XCreatePixmap(display, rootwin, size.x, size.y, depth);
    if (gc == 0) {
	XGCValues gcv;
	if (depth == 1) {
	    gcv.foreground= 1;
	    gcv.background= 0;
	    gc= XCreateGC(display, pm, GCForeground|GCBackground, &gcv);
	} else
	    gc= XCreateGC(display, pm, 0, &gcv);
    }
    XFillRectangle(display, pm, gc, 0, 0, size.x, size.y);
}
 
Pixmap XBitmap::GetPixmap()
{
    if (changed) {
	CreatePixmap();
	if (!inserver) {
	    if (ximage)
		XPutImage(display, pm, gc, ximage, 0, 0, 0, 0, size.x, size.y);
	    inserver= TRUE;
	}
	changed= FALSE;
    }
    return pm;
}

XImage *XBitmap::GetImage()
{
    if (changed) {
	if (inserver && pm) {
	    if (ximage)
		XDestroyImage(ximage);
	    ximage= XGetImage(display, pm, 0, 0, size.x, size.y,
						(u_long) AllPlanes, XYPixmap);
	    inserver= FALSE;
	} else {
	    if (ximage == 0) {
		int si= ShortsPerLine() * size.y;
		short *data= new short[si];
		ximage= XCreateImage(display, XDefaultVisual(display, screen),
		    depth, (depth > 1) ? ZPixmap : XYPixmap, 0, (char*) data,
					    size.x, size.y, sizeof(short)*8, 0);
		ximage->bitmap_unit= ximage->bitmap_pad= sizeof(short)*8;
	    }
	}
	changed= FALSE;
    }
    return ximage;
}

void XBitmap::SetPixel(u_int x, u_int y, int value)
{
    XPutPixel(GetImage(), x, y, value);
    changed= TRUE;
}

int XBitmap::GetPixel(u_int x, u_int y)
{
    return (int) XGetPixel(GetImage(), x, y);
}
