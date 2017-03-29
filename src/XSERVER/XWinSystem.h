#ifndef XWinSystem_First
#define XWinSystem_First

#include "X11.h"
#include "WindowSystem.h"

class XWindowSystem: public WindowSystem {
    class XClipBoard *xclip;
    XContext context;
public:
    MetaDef(XWindowSystem);
    XWindowSystem(bool &ok);
    ~XWindowSystem();
    
    void Notify(SysEventCodes, int);
    bool HasInterest();
    void RemoveWindow(WindowPort*);
    WindowPort *MakeWindow(InpHandlerFun ihf, void *priv1,
					bool overlay, bool block, bool wm);
    FontManager *MakeFontManager(char *name);
    DevBitmap *MakeDevBitmap(Point sz, u_short *data, u_short depth);
    DevBitmap *LoadDevBitmap(const char *name);
    void graphicDelay(unsigned int);
    bool interrupted();
    ClipBoard *MakeClipboard();
};

extern XDisplay *display;
extern XWindow rootwin;
extern int screen;
extern Pixmap batchPixmap;

extern WindowSystem *NewXWindowSystem();

#endif XWinSystem_First
