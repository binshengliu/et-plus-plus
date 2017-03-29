#ifndef NeWSWindowSystem_First
#define NeWSWindowSystem_First

#include "WindowSystem.h"

class NeWSWindowSystem: public WindowSystem {
    int psfd;
public:
    NeWSWindowSystem(bool &ok);
	
    WindowPort *MakeWindow(InpHandlerFun ihf, void *priv1,
							bool overlay, bool block);
    FontManager *MakeFontManager(char *name);
    Bitmap *MakeBitmap(Point sz, u_short *data, u_short depth);
    Bitmap *MakeBitmap(const char *name);
    
    void graphicDelay(unsigned int);
};

extern WindowSystem *NewNeWSWindowSystem();

#endif NeWSWindowSystem_First
