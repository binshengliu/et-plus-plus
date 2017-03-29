#include "X11.h"
#include "XWinSystem.h"
#include "XWindowPort.h"
#include "XFont.h"
#include "XBitmap.h"
#include "XClipBoard.h"

#include "String.h"
#include "Error.h"

XDisplay *display;
XWindow rootwin;
int screen;
int xfd;
Pixmap batchPixmap;
    
//---- XWindowSystem -----------------------------------------------------------

MetaImpl0(XWindowSystem);

WindowSystem *NewXWindowSystem()
{
    bool ok= FALSE;
    WindowSystem *s= new XWindowSystem(ok);
    if (ok)
	return s;
    delete s;
    return 0;
}

XWindowSystem::XWindowSystem(bool &ok) : WindowSystem(ok, "X11R4")
{
    if (display= XOpenDisplay(0)) {
	XWindowAttributes wattr;
	if (gDebug)
	    XSynchronize(display, XTrue);
	screen= XDefaultScreen(display);
	rootwin= XRootWindow(display, screen);
	context= XUniqueContext();
	gScreenRect.extent.x= XDisplayWidth(display, screen);
	gScreenRect.extent.y= XDisplayHeight(display, screen);
	gDepth= XDefaultDepth(display, screen);
	SetResourceId(xfd= display->fd);
	gSystem->AddFileInputHandler(this);
	XGetWindowAttributes(display, rootwin, &wattr);
	if (wattr.all_event_masks & SubstructureRedirectMask)
	    hasExternalWindowManager= TRUE;
	else
	    hasExternalWindowManager= FALSE;
	ok= TRUE;
    }
}

XWindowSystem::~XWindowSystem()
{
    if (batchPixmap) {
	XFreePixmap(display, batchPixmap);
	batchPixmap= 0;
    }
    if (display)
	XCloseDisplay(display);
}

bool XWindowSystem::HasInterest()
{
    Allow();
    XFlush(display);
    return ! ShouldRemove();
}

void XWindowSystem::Notify(SysEventCodes, int)
{
    Token t;
    XWindowPort *port= 0;
    XEvent ev;
    
    XPending(display);
    do {
	XNextEvent(display, &ev);
	if (xclip->ProcessEvent(ev))
	    if (XFindContext(display, ev.xany.window, context, (caddr_t*)&port) == 0)
		if (port && port->MapEvent(ev, &t))
			port->SendInput(&t);
    } while (XQLength(display) > 0);
}

WindowPort *XWindowSystem::MakeWindow(InpHandlerFun ihf, void *priv,
					bool overlay, bool block, bool wm)
{
    XWindowPort *wp= new XWindowPort(ihf, priv, overlay, block, wm);
    XSaveContext(display, wp->GetWinId(), context, wp);
    return wp;
}

void XWindowSystem::RemoveWindow(WindowPort *wp)
{
    XDeleteContext(display, ((XWindowPort*)wp)->GetWinId(), context);
}

FontManager *XWindowSystem::MakeFontManager(char *name)
{
    return new XFontManager(name);
}

DevBitmap *XWindowSystem::MakeDevBitmap(Point sz, u_short *data, u_short depth)
{
    return new XBitmap(sz, data, depth);
}

void XWindowSystem::graphicDelay(unsigned int duration)
{
    XSync(display, XFalse);
    Wait(duration);
}

bool XWindowSystem::interrupted()
{
    XEvent ev;
    if (XCheckTypedEvent(display, KeyPress, &ev))
	if (((XKeyEvent*)&ev)->keycode == 8) // SUN L1 function key -> ET++ interrupt
	    return TRUE;
    return FALSE;
}

ClipBoard *XWindowSystem::MakeClipboard()
{
    return xclip= new XClipBoard;
}

DevBitmap *XWindowSystem::LoadDevBitmap(const char *name)
{
    u_int w, h;
    int xhot, yhot;
    Pixmap bm;
    
    if (XReadBitmapFile(display, rootwin, name, &w, &h, &bm, &xhot, &yhot) == BitmapSuccess)
	return new XBitmap(Point((int)w, (int)h), bm);
    return WindowSystem::LoadDevBitmap(name);
}
