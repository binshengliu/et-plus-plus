#ifndef ServerSystem_First
#define ServerSystem_First

#include "WindowSystem.h"

class ServerWindowSystem: public WindowSystem {
public:
    ServerWindowSystem(bool &ok);
    ~ServerWindowSystem();
       
    int Connect(char *hostname, int port);
    bool InitServer();
    
    WindowPort *MakeWindow(InpHandlerFun ihf, void *priv,
						    bool overlay, bool block);
    FontManager *MakeFontManager(char *name);
    Bitmap *MakeBitmap(Point sz, u_short *data, u_short depth);
    Bitmap *MakeBitmap(const char *name);
    
    void graphicDelay(unsigned int duration);
};

extern WindowSystem *NewServerSystem();

#endif System_First
