//$WindowSystem,TimeoutHandler$

#include <stdio.h>

#include "WindowSystem.h"
#include "String.h"
#include "Error.h"
#include "WindowPort.h"
#include "ClipBoard.h"
#include "DevBitmap.h"


Rectangle       gScreenRect(0, 0, 1152, 900);
bool            gWinInit= FALSE;
WindowSystem    *gWindowSystem= 0;
int             gDepth= 1;
bool            gColor= FALSE;
bool            forcemono;

static DevBitmap *LoadSunRasterfile(const char *name);
static DevBitmap *LoadIconfile(const char *name);

//---- TimeoutHandler ----------------------------------------------------------

extern WindowPort *focusport;
Point lastpos;

class TimeoutHandler : public SysEvtHandler {
public:
    TimeoutHandler() : SysEvtHandler(0)
	{ }
    void Notify(SysEventCodes, int);
};

void TimeoutHandler::Notify(SysEventCodes, int)
{
    Token t(eEvtIdle, eFlgNone, lastpos);
    if (focusport)
	focusport->SendInput(&t);
}

//---- WindowSystem ------------------------------------------------------------

MetaImpl0(WindowSystem);

WindowSystem::WindowSystem(bool&, char *name) : SysEvtHandler(0)
{
    wsName= name;
}

WindowSystem::~WindowSystem()
{
    gWinInit= FALSE;
    SafeDelete(gFontManager);
}

void WindowSystem::Init()
{
    if (forcemono)
	gDepth= 1;
    gColor= gDepth > 1;
    
    gFontManager= MakeFontManager(gEtDir);
    if (gFontManager == 0)
	Fatal("Init", "no font manager");

    if (gFontManager->Init())
	Fatal("Init", "initialization of font manager failed");
	
    gInkManager= new InkManager();
    gInkManager->Init();

    gSystem->AddTimeoutHandler(new TimeoutHandler);
}

WindowPort *WindowSystem::MakeWindow(InpHandlerFun, void*, bool, bool, bool) 
{
    return 0;
}

void WindowSystem::RemoveWindow(WindowPort*)
{
}

FontManager *WindowSystem::MakeFontManager(char*)
{
    return 0;
}

DevBitmap *WindowSystem::MakeDevBitmap(Point, u_short*, u_short)
{
    return 0;
}

DevBitmap *WindowSystem::LoadDevBitmap(const char *name)
{
    DevBitmap *dbm;
    
    if (dbm= LoadSunRasterfile(name))
	return dbm;
    if (dbm= LoadIconfile(name))
	return dbm;
    return 0;
}

class ClipBoard *WindowSystem::MakeClipboard()
{
    return new ClipBoard;
}

void WindowSystem::graphicDelay(u_int duration)
{
    Wait(duration);
}

bool WindowSystem::interrupted()
{
    return FALSE;
}

//---- portable Sun rasterfile load --------------------------------------------

struct rasterfile {
	int     ras_magic;      /* magic number */
	int     ras_width;      /* width (pixels) of image */
	int     ras_height;     /* height (pixels) of image */
	int     ras_depth;      /* depth (1, 8, or 24 bits) of pixel */
	int     ras_length;     /* length (bytes) of image */
	int     ras_type;       /* type of file; see RT_* below */
	int     ras_maptype;    /* type of colormap; see RMT_* below */
	int     ras_maplength;  /* length (bytes) of following map */
	/* color map follows for ras_maplength bytes, followed by image */
};

#define RAS_MAGIC       0x59a66a95
#define RT_STANDARD     1       /* Raw pixrect image in 68000 byte order */
#define RMT_EQUAL_RGB   1       /* red[ras_maplength/3],green[],blue[] */

static DevBitmap *LoadSunRasterfile(const char *name)
{
    DevBitmap *bm= 0;
    FILE *fp;

    if (fp= fopen((char*) name, "r")) {
	struct rasterfile ras;
	
	fread((char*) &ras, sizeof(struct rasterfile), 1, fp);
	if (ras.ras_magic == RAS_MAGIC) {
	    int x, y;
	    bm= gWindowSystem->MakeDevBitmap(Point(ras.ras_width, ras.ras_height), 0, ras.ras_depth);
	    int spl= bm->ShortsPerLine();
	    
	    if (ras.ras_maptype == RMT_EQUAL_RGB) {
		int l= ras.ras_maplength;
		if (l > 0) {
		    bm->SetColormapSize(l);
		    char *t= new char[l];
		    fread(t, 1, l, fp);
		    l/= 3;
		    for (x= 0; x < l; x++)
			bm->SetColormapEntry(x, t[x], t[x+l], t[x+2*l]);
		    delete t;
		} else {
		    l= 256;
		    bm->SetColormapSize(l);
		    for (x= 0; x < l; x++)
			bm->SetColormapEntry(x, x, x, x);
		}
	    }
	    
	    for (y= 0; y < ras.ras_height; y++)
		for (x= 0; x < spl*2; x++)
		    bm->SetByte(x, y, fgetc(fp));
	}
	fclose(fp);
    }
    return bm;
}

//---- portable Sun & X11 icon load --------------------------------------------

static DevBitmap *LoadIconfile(const char *name)
{
    static byte *swapmap= 0;
    
    FILE *fp;
    bool swapbits= FALSE;
    int tmp, version= 1, width= 0, height= 0, depth= 1, bitsperitem= 16;
    int b, x= 0, y= 0, bytesperline, bytesperitem,itemsperline, imbytesperline;
    u_long mask, m, d;
    char buf[200], token[100], c;
    DevBitmap *dbm;
    
    if ((fp= fopen((char*) name, "r")) == 0)
	return 0;
    
    while (fscanf(fp, "%s", buf) == 1) {
	if (strcmp(buf, "#define") == 0) {
	    swapbits= TRUE;
	    if (fscanf(fp, "%s %d", token, &tmp) == 2) {
		int l= strlen(token);
		if (strcmp(&token[l-6], "_width") == 0) {
		    width= tmp;
		    continue;
		}
		if (strcmp(&token[l-7], "_height") == 0) {
		    height= tmp;
		    continue;
		}
	    }
	}
	if (sscanf(buf, "Format_version=%d", &version) == 1)
	    continue;
	if (sscanf(buf, "Width=%d", &width) == 1)
	    continue;
	if (sscanf(buf, "Height=%d", &height) == 1)
	    continue;
	if (sscanf(buf, "Depth=%d", &depth) == 1)
	    continue;
	if (sscanf(buf, "Valid_bits_per_item=%d", &bitsperitem) == 1)
	    continue;
	if (strcmp(buf, "char") == 0) {
	    bitsperitem= sizeof(char) * 8;
	    continue;
	}
	if (strcmp(buf, "short") == 0) {
	    bitsperitem= sizeof(short) * 8;
	    continue;
	}
	if (strcmp(buf, "int") == 0) {
	    bitsperitem= sizeof(int) * 8;
	    continue;
	}
	if (strcmp(buf, "*/") == 0)
	    break;
	if (strcmp(buf, "{") == 0)
	    break;
    }

    if (width <= 0 || height <= 0)
	return 0;
	
    dbm= gWindowSystem->MakeDevBitmap(Point(width, height), 0, depth);
    itemsperline= (width-1) / bitsperitem + 1;
    bytesperitem= bitsperitem / 8;
    bytesperline= itemsperline * bytesperitem;
    mask= 0xff << (bytesperitem-1)*8;
    imbytesperline= dbm->BytesPerLine();
    
    if (swapbits && swapmap == 0) {
	int b;
	swapmap= new byte[256];
	for (int i= 0; i < 256; i++) {
	    b= 0;
	    if (i & 0x01)
		b|= 0x80;
	    if (i & 0x02)
		b|= 0x40;
	    if (i & 0x04)
		b|= 0x20;
	    if (i & 0x08)
		b|= 0x10;
	    if (i & 0x10)
		b|= 0x08;
	    if (i & 0x20)
		b|= 0x04;
	    if (i & 0x40)
		b|= 0x02;
	    if (i & 0x80)
		b|= 0x01;
	    swapmap[i]= b;
	}
    }
    
    while (fscanf(fp, "%i%c", &tmp, &c) >= 1) {
	m= mask;
	for (b= bytesperitem-1; b >= 0; b--) {
	    d= (tmp & m) >> (b*8);
	    m>>= 8;
	    if (x < imbytesperline && y < height) {
		if (swapbits)
		    dbm->SetByte(x, y, (int) swapmap[d]);
		else
		    dbm->SetByte(x, y, (int) d);
	    }
	    
	    x++;
	    if (x == bytesperline) {
		x= 0;
		y++;
	    }
	}
    }
    return dbm;
}
