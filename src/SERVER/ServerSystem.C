#include "ServerSystem.h"

#include "ServerPort.h"
#include "ServerFont.h"
#include "ServerBitmap.h"
#include "ServerConnection.h"

//---- ServerWindowSystem ------------------------------------------------------

WindowSystem *NewServerWindowSystem()
{
    bool ok= FALSE;
    
    WindowSystem *s= new ServerWindowSystem(ok);
    if (ok)
	return s;
    delete s;
    return 0;
}

ServerWindowSystem::ServerWindowSystem(bool &ok) : WindowSystem(ok, "ServerSunWindow")
{
    gConnection= new ServerConnection();
    if (gConnection->GetResourceId() >= 0) {
	gSystem->AddFileInputHandler(gConnection);
	ok= TRUE;
    }
}
	
ServerWindowSystem::~ServerWindowSystem()
{
    SafeDelete(gConnection);
}

WindowPort *ServerWindowSystem::MakeWindow(InpHandlerFun ihf,
				void *priv1, bool overlay, bool block)
{
    return new ServerPort(ihf, priv1, overlay, block);
}

FontManager *ServerWindowSystem::MakeFontManager(char *name)
{
    return new ServerFontManager(name);
}

Bitmap *ServerWindowSystem::MakeBitmap(Point sz, u_short *data, u_short depth)
{
    return new ServerBitmap(sz, data, depth);
}

Bitmap *ServerWindowSystem::MakeBitmap(const char*)
{
    return 0;
}

void ServerWindowSystem::graphicDelay(unsigned int duration)
{
    gConnection->SendMsg(eMsgDelay, -1, "iZ", duration);
}
