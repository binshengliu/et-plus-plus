#include "NeWSWindowSystem.h"

#include "NeWSWindowPort.h"
#include "NeWSFont.h"
#include "NeWSBitmap.h"

#include "String.h"
#include <sgtty.h>

#include "psio.h"
#include "news.h"
#undef ps_flush_PostScript
#define ps_flush_PostScript() psio_flush(PostScript)

extern "C" int ioctl(int,int,void*);

//---- NeWSEvtHandler -------------------------------------------------------------

extern NeWSWindowPort *wports[];

class NeWSEvtHandler : public SysEvtHandler {
public:
    NeWSEvtHandler(int fd) : SysEvtHandler(fd)
	{ }
    void Notify(SysEventCodes, int);
    bool HasInterest();
};

bool NeWSEvtHandler::HasInterest()
{
    ps_flush_PostScript();
    return ! ShouldRemove();
}

int lastwinid= -1;

void NeWSEvtHandler::Notify(SysEventCodes, int)
{
    int n, arg, id, code, flags, x, y, w, h;
    Token t;
    register NeWSWindowPort *port;    
    
    for (;;) {
	if (ps_getevent(&id, &code, &flags, &t.At, &x, &y, &w, &h)) {
	    if (id != lastwinid) {
		ps_focus(id);
		lastwinid= id;
	    }
	    port= wports[id];
	    if (port) {
		t.Code= code;
		t.Flags= flags;
		t.Pos.x= x;
		t.Pos.y= y;
		t.ext.x= w;
		t.ext.y= h;
		port->Send(&t);
	    }
	}
	n= psio_availinputbytes(PostScriptInput);
	if (n < 3)
	    break;
	ioctl(psio_fileno(PostScriptInput), FIONREAD, &arg);
	if (n+arg < 3)
	    break;
    }
    ps_flush_PostScript();
}

//---- NeWSWindowSystem -----------------------------------------------------------

WindowSystem *NewNeWSWindowSystem()
{
    bool ok= FALSE;
    WindowSystem *s= new NeWSWindowSystem(ok);
    if (ok)
	return s;
    delete s;
    return 0;
}

NeWSWindowSystem::NeWSWindowSystem(bool &ok) : WindowSystem(ok, "SunNeWS")
{
    if (ps_open_PostScript() != NULL) {
	gSystem->AddFileInputHandler(new NeWSEvtHandler(psio_fileno(PostScriptInput)));
	ps_initialize(form("%s/src/NEWS/et.ps", gEtDir));    // load routines
	ok= TRUE;
    }
}
    
WindowPort *NeWSWindowSystem::MakeWindow(InpHandlerFun ihf,
			    void *priv1, bool overlay, bool block)
{
    return new NeWSWindowPort(ihf, priv1, overlay, block);
}

FontManager *NeWSWindowSystem::MakeFontManager(char *name)
{
    return new NeWSFontManager(name);
}

Bitmap *NeWSWindowSystem::MakeBitmap(Point sz, u_short *data, u_short depth)
{
    return new NeWSBitmap(sz, data, depth);
}

Bitmap *NeWSWindowSystem::MakeBitmap(const char *name)
{
    *bp= new NeWSBitmap(name);
}

void NeWSWindowSystem::graphicDelay(unsigned int duration)
{
    Wait(duration);
}
