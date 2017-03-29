#ifndef WindowSystem_First
#ifdef __GNUG__
#pragma once
#endif
#define WindowSystem_First

#include "Port.h"
#include "System.h"

typedef void (*InpHandlerFun)(void*, Token*);     
    // event handler function

//---- WindowSystem ------------------------------------------------------------

class WindowSystem : public SysEvtHandler {
protected:
    char *wsName;
    bool hasExternalWindowManager;
public:
    MetaDef(WindowSystem);
    WindowSystem(bool &ok, char *name= "GENERIC");

    virtual ~WindowSystem();
    
    char *GetWsName()
	{ return wsName; }
    bool HasExternalWindowManager()
	{ return hasExternalWindowManager; }

    virtual void Init();
    virtual class WindowPort *MakeWindow(InpHandlerFun ihf, void *priv1,
				bool overlay, bool block, bool wm);
    virtual void RemoveWindow(WindowPort*);
    virtual FontManager *MakeFontManager(char*);
    virtual DevBitmap *MakeDevBitmap(Point sz, u_short *data, u_short depth);
    virtual DevBitmap *LoadDevBitmap(const char *name);
    virtual void graphicDelay(unsigned int);
    virtual class ClipBoard *MakeClipboard();
    virtual bool interrupted();
};

extern WindowSystem *gWindowSystem;
extern Rectangle    gScreenRect;
extern bool         gWinInit;
extern bool         gColor;
extern int          gDepth;

inline bool Interrupted()
    { return gWindowSystem->interrupted(); }

inline void GraphicDelay(unsigned int duration)
    { gWindowSystem->graphicDelay(duration); }

#endif WindowSystem_First
