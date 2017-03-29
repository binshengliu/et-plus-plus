#include <stdio.h>

#include "SunWindowSystem.h"
#include "Error.h"
#include "String.h"
#include "SunWindowPort.h"
#include "SunFont.h"
#include "SunBitmap.h"
#include "SunClipboard.h"
#include "pixrect.h"
#include "pixfont.hh"
#include "pixwin.hh"

#define CURSORS "cursors"
#define VFontExt "vf"
#define MAXSLOTS 32

struct pixfont *cursors;
struct SunWindowPort *allwindows;
int parentNo, topmostNo, rootFd= -1;
struct pixrect *tmppr, *tmppr2;
struct SunWindowPort *overlayport;
struct fullscreen_t *FS;
struct pixrect *pens[MAXPENS];
suncolormap *Cmap;
int cmseq= 1;

//---- color -------------------------------------------------------------------

#define CMSNAME "et_cms"
#define CMSSIZE 254
#define CMSSTART 2

static int cmsno= 1;

suncolormap::suncolormap(suncolormap *from)
{
    int i;
    
    if (from) {
	length= from->length;
	name= strsave(form("%s-%d", CMSNAME, cmsno++));
    } else {
	length= CMSSIZE;
	name= strsave(CMSNAME);
    }
    
    changed= TRUE;
    start= 0;
    red= new u_char[length];
    green= new u_char[length];
    blue= new u_char[length];
    usecnt= new int[length];
    type= new char[length];

    
    if (from) {
	for (i= 0; i < length; i++) {
	    red[i]= from->red[i];
	    green[i]= from->green[i];
	    blue[i]= from->blue[i];
	    usecnt[i]= from->usecnt[i];
	    type[i]= from->type[i];
	}
	depth= from->depth;
    } else {
	int r, g, b, j;

	for (i= 0; i < length; i++) {
	    usecnt[i]= 0;
	    type[i]= 0;
	}
	
	red[0]= green[0]= blue[0]= 255;         /* white */
	red[1]= green[1]= blue[1]= 0;           /* black */
	usecnt[0]= usecnt[1]= 2000000;

	i= 2;
	for (r= 0; r < 6; r++) {
	    for (g= 0; g < 6; g++) {
		for (b= 0; b < 6; b++) {
		    red[i]= r*51;
		    green[i]= g*51;
		    blue[i]= b*51;
		    usecnt[i]= 1000000;
		    i++;
		}
	    }
	}
	for (j= 1; i < length-1 && j < 10; i++, j+= 2) {
	    red[i]= green[i]= blue[i]= j * 25;
	    usecnt[i]= 2000000;
	}
	red[length-1]= green[length-1]= blue[length-1]= 0;           /* black */
	usecnt[length-1]= 2000000;
	depth= 8;
    }
}

suncolormap::~suncolormap()
{
    SafeDelete(name);
    SafeDelete(red);
    SafeDelete(green);
    SafeDelete(blue);
    SafeDelete(usecnt);
    SafeDelete(type);
}

void suncolormap::install(pixwin *pw)
{
    if (changed) {
	pw_setcmsname(pw, name);    
	pw_putcolormap(pw, 0, length, red, green, blue);    
	changed= FALSE;
	cmseq++;
    }
}

int suncolormap::mapcolor(int r, int g, int b)
{
    register int i, dist, minix= 0, mindist= 3*256*256;
    register int rd, gd, bd;
	
    for (i= minix= 0; i < length; i++) {
	if (type[i])
	    continue;
	rd= red[i]  - r;
	gd= green[i]- g;
	bd= blue[i] - b;
	dist= rd*rd + gd*gd + bd*bd;
	if (dist < mindist) {
	    mindist= dist;
	    minix= i;
	}
    }
    usecnt[minix]++;
    return minix;
}

//---- pixrect cache -----------------------------------------------------------

static struct cache {
    short type, k1, k2, k3;
    struct pixrect *pr;
} Cache[MAXSLOTS];

int findpr(char type, pixrect **pr, int wd, int ht, int k1, int k2, int k3)
{
    static int last= 0;
    int i, found= FALSE, emptyslot= -1;
    int stop;
    
    if (last)
	stop= last-1;
    else
	stop= MAXSLOTS-1;
    for (i= last; i != stop; i= (i+1) % MAXSLOTS) {
	if (Cache[i].pr == 0) {
	    emptyslot= i;
	    continue;
	}
	if (Cache[i].type == type
		&& Cache[i].pr->pr_size.x == wd && Cache[i].pr->pr_size.y == ht
		&& Cache[i].k1 == k1 && Cache[i].k2 == k2 && Cache[i].k3 == k3) {
	    found= TRUE;
	    break;
	}
    }
    if (! found) {
	if (emptyslot >= 0)
	    i= emptyslot;
	else
	    pr_destroy(Cache[i].pr);
	Cache[i].pr= mem_create(wd, ht, 1);
	Cache[i].type= type;
	Cache[i].k1= k1;
	Cache[i].k2= k2;
	Cache[i].k3= k3;
    }
    *pr= Cache[last= i].pr;
    return found;
}

//---- UrgHandler --------------------------------------------------------------

static int sigurgcnt;

class UrgHandler : public SysEvtHandler {
public:
    UrgHandler() : SysEvtHandler(eSigUrgent)
	{ }
    void Notify(SysEventCodes, int);
};

void UrgHandler::Notify(SysEventCodes, int)
{
    sigurgcnt++;
    if (sigurgcnt > 2)
	abort();    //_exit(1);
}

//---- SunWindowSystem ---------------------------------------------------------

MetaImpl0(SunWindowSystem);

WindowSystem *NewSunWindowSystem()
{
    bool ok= FALSE;
    WindowSystem *s= new SunWindowSystem(ok);
    
    if (ok)
	return s;
    delete s;
    return 0;
}

SunWindowSystem::SunWindowSystem(bool &ok) : WindowSystem(ok, "SunWindow")
{
    char parentName[WIN_NAMESIZE];

    ok= FALSE;

    if (we_getparentwindow(parentName) != 0)
	return;
	    
    parentNo= win_nametonumber(parentName);
    rootFd= open(parentName, 0);
    if (rootFd < 0) {
	Error("SunWindowSystem", "can't open rootwindow\n");
	return;
    }
    topmostNo= win_getlink(rootFd, WL_TOPCHILD);    
    
    struct pixwin *pw= pw_open(rootFd);
    gDepth= pw->pw_pixrect->pr_depth;
    gScreenRect.origin= gPoint0;
    gScreenRect.extent.x= pw->pw_pixrect->pr_size.x;
    gScreenRect.extent.y= pw->pw_pixrect->pr_size.y;
    pw_close(pw);

    char *pathname= form("./%s.%s", CURSORS, VFontExt);
    if ((cursors= pf_open(pathname)) == NULL) {
	sprintf(pathname, "%s/sunfonts/%s.%s", gEtDir, CURSORS, VFontExt);
	if ((cursors= pf_open(pathname)) == NULL) {
	    Error("SunWindowSystem", "can't open cursorfont %s (install the vfonts in ET_DIR/sunfonts)", pathname);
	    return;
	}
    }
    
    SetResourceId(eSigWindowChanged);
    gSystem->AddSignalHandler(this);
    gSystem->AddSignalHandler(urghandler= new UrgHandler, FALSE);
    
    if (gDepth > 1)
	Cmap= new suncolormap(0);

    ok= TRUE;
}
	
SunWindowSystem::~SunWindowSystem()
{
    register int i;
    
    if (tmppr)
	pr_destroy(tmppr);
    if (tmppr2)
	pr_destroy(tmppr2);
    if (cursors)
	pf_close(cursors);
    for (i= 0; i < MAXSLOTS; i++)
	if (Cache[i].pr)
	    pr_destroy(Cache[i].pr);
    for (i= 0; i < MAXPENS; i++)
	if (pens[i])
	    pr_destroy(pens[i]);
    if (FS)
	fullscreen_destroy(FS);
}

bool SunWindowSystem::HasInterest()
{
    return overlayport == 0;
}

void SunWindowSystem::Notify(SysEventCodes, int)
{
    register SunWindowPort *p;
    
    for (p= allwindows; p; p= p->next)
	p->TestDamage();
}

bool SunWindowSystem::interrupted()
{
    int sc= sigurgcnt;
    sigurgcnt= 0;
    return (sc > 0);
}

WindowPort *SunWindowSystem::MakeWindow(InpHandlerFun ihf, void *priv1,
					bool overlay, bool block, bool wm)
{
    return new SunWindowPort(ihf, priv1, overlay, block, wm);
}

void SunWindowSystem::RemoveWindow(WindowPort *wp)
{
}

FontManager *SunWindowSystem::MakeFontManager(char *name)
{
    return new SunFontManager(name);
}

DevBitmap *SunWindowSystem::MakeDevBitmap(Point sz, u_short *data, u_short depth)
{
    return new SunBitmap(sz, data, depth);
}

DevBitmap *SunWindowSystem::LoadDevBitmap(const char *name)
{
    FILE *infp;
    
    if (infp= fopen((char*) name, "r")) {
	pixrect *pr;
	colormap_t *cmap= 0, cmap1;
	
	if (pr= pr_load(infp, &cmap1)) {
	    if (cmap1.length > 0) {
		cmap= new colormap_t;
		*cmap= cmap1;
	    }
	}
	fclose(infp);
	if (pr)
	    return new SunBitmap(pr, cmap);
    }
    return WindowSystem::LoadDevBitmap(name);
}

ClipBoard *SunWindowSystem::MakeClipboard()
{
    return new SunClipBoard;
}

