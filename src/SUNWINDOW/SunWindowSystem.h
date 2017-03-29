#ifndef SunWindowSystem_First
#define SunWindowSystem_First

#include "WindowSystem.h"

#define MAXPENS 32

class suncolormap {
public:
    int start, length;
    u_char *red, *green, *blue;
    char *type;
    char *name;
    int *usecnt;
    int changed;
    int cmseq;
    int depth;
public:
    suncolormap(suncolormap *from);
    ~suncolormap();
    void install(struct pixwin *pw);
    int mapcolor(int r, int g, int b);
};

//---- SunWindowSystem ---------------------------------------------------------

class SunWindowSystem: public WindowSystem {
    SysEvtHandler *damagehandler, *urghandler;

public:
    MetaDef(SunWindowSystem);
    SunWindowSystem(bool &ok);
    ~SunWindowSystem();
    
    void Notify(SysEventCodes, int);
    bool HasInterest();
    WindowPort *MakeWindow(InpHandlerFun ihf, void *priv1,
					    bool overlay, bool block, bool wm);
    void RemoveWindow(WindowPort *wp);
    FontManager *MakeFontManager(char *name);
    DevBitmap *MakeDevBitmap(Point sz, u_short *data, u_short depth);
    DevBitmap *LoadDevBitmap(const char *name);
    ClipBoard *MakeClipboard();
    bool interrupted();
};

extern int findpr(char type, struct pixrect **pr, int wd, int ht, int k1, int k2, int k3);
extern WindowSystem *NewSunWindowSystem();

extern struct pixrect *batchpr;
extern struct pixrect *tmppr;
extern int rootFd, parentNo, topmostNo;
extern struct pixfont *cursors;
extern struct SunWindowPort *allwindows;
extern struct SunWindowPort *overlayport;
extern struct fullscreen_t *FS;
extern struct pixrect *pens[MAXPENS];

#endif SunWindowSystem_First
