#include <stdio.h>

#include "String.h"
#include "Error.h"
#include "SunSystem.h"
#include "sunsockets.h"
#include "SunWindowPort.h"
#include "SunBitmap.h"
#include "SunFont.h"
#include "Server.h"

const int cMaxConnections= 32;

class Connection *connections[cMaxConnections];
static class Connection *clipboard;
static bool cachegood= FALSE;
static struct Response cache;
static char *bp;

//---- ServerSunPort -----------------------------------------------------------

class ServerSunPort: public SunWindowPort {
    class Connection *connection;
public:
    ServerSunPort(Connection *c, void *p1, bool b1, bool b2)
						: SunWindowPort(0, p1, b1, b2)
	{ connection= c; }
    void Send(Token *t);
};

//---- dummies -----------------------------------------------------------------

class ClipBoard {
    int dummy;
public:
    ClipBoard();
    ~ClipBoard();
    bool NotOwner();
    char *GetBuf(int*);
};
ClipBoard::ClipBoard() {}
ClipBoard::~ClipBoard() {}
bool ClipBoard::NotOwner() { return FALSE; }
char *ClipBoard::GetBuf(int*) { return 0; }
int gWindow;
int gProgname;
int _FileDialog_ShowInWindow;
int _FileDialog__ctorFPC__;

//---- Connection --------------------------------------------------------------

const int MaxPorts      = 30,
	  MaxBitmaps    = 100,
	  MaxFonts      = 100;

class Connection : public SysEvtHandler {
    FILE *ifp, *ofp;
    int fd;
    WindowPort *ports[MaxPorts];
    Bitmap *bitmaps[MaxBitmaps];
    Font *fonts[MaxFonts];
    
public:
    Connection(int);
    ~Connection();
    
    void CloseAll();
    void Doit(struct Message*);  
    
    //---- I/O
    int Read(void*, int);
    void Write(void *vp, int l, bool flush= TRUE);
    bool MoreMessages()
	{ return (bool) (available(ifp) >= sizeof(struct Message)); }
    void Notify(SysEventCodes, int);
    
    //---- make new resources
    int NewWindowPort(void *priv, bool b1, bool b2);
    int NewBitmap(int l, Point p, u_short depth);
    int NewFont(int fid, int ps, int face);
    
    //---- accessing
    WindowPort *MapPort(int i)
	{ return (i<0 || i>=MaxPorts) ? 0 : ports[i]; }
    Bitmap *MapBitmap(int i)
	{ return (i<0 || i>=MaxBitmaps) ? 0 : bitmaps[i]; }
    Font *MapFont(int i)
	{ return (i<0 || i>=MaxFonts) ? 0 : fonts[i]; }
};

void ServerSunPort::Send(Token *t)
{
    if (state == eWsShown && t && t->Code != eEvtNone && t->Code != eEvtIdle) {
	struct Response rp;
    
	rp.port= (WindowPort*) privdata;
	rp.t= *t;
	connection->Write(&rp, sizeof(struct Response));
    }
}

Connection::Connection(int f) : SysEvtHandler(f)
{
    fd= f;
    gSystem->AddFileInputHandler(this);
    ifp= fdopen(f, "r");
    ofp= fdopen(f, "w");
    connections[f]= this;
}

Connection::~Connection()
{
    CloseAll();
    if (ifp)
	fclose(ifp);
    if (ofp)
	fclose(ofp);
    connections[fd]= 0;
}
    
void Connection::Notify(SysEventCodes, int)
{
    struct Message m;
    
    do {
	if (Read(&m, sizeof(struct Message))) 
	    return;
	Doit(&m);
    } while (MoreMessages());
}

int Connection::Read(void *p, int l)
{
    if (fread((char*)p, l, 1, ifp) == 0) {
	CloseAll();
	Remove();
	return TRUE;
    }
    return FALSE;
}

void Connection::Write(void *vp, int l, bool flush)
{
    if (ofp) {
	if (fwrite((char*)vp, l, 1, ofp) != 1)
	    SysError("Write", "");
	if (flush)
	    fflush(ofp);
    }
}

void Connection::CloseAll()
{
    register int i;
    
    for (i= 0; i < MaxPorts; i++) {
	if (ports[i]) {
	    if (ports[i]->state != eWsHidden)
		ports[i]->DevHide1();
	    // ports[i]->Destroy();
	    ports[i]= 0;
	}
    }
    for (i= 0; i < MaxBitmaps; i++)
	SafeDelete(bitmaps[i]);
    for (i= 0; i < MaxFonts; i++)
	fonts[i]= 0;
}

int Connection::NewWindowPort(void *priv, bool b1, bool b2)
{
    int id;
    WindowPort *wport= new ServerSunPort(this, priv, b1, b2);
    for (id= 0; id<MaxPorts; id++) {
	if (ports[id] == 0) {
	    ports[id]= wport;
	    break;
	}
    }
    if (id >= MaxPorts)
	return -1;
    return id;
}

int Connection::NewBitmap(int l, Point p, u_short depth)
{
    int id;
    Bitmap *bitmap;
    short *slist= new short[(l+2)/2];
    
    Read(slist, l);
    bitmap= new SunBitmap(p, (u_short*) slist, depth);
    // bitmap= gWindowSystem->MakeBitmap(p, slist);
    for (id= 0; id<MaxBitmaps; id++) {
	if (bitmaps[id] == 0) {
	    bitmaps[id]= bitmap;
	    break;
	}
    }
    if (id >= MaxBitmaps)
	return -1;
    return id;
}

int Connection::NewFont(int fid, int ps, int face)
{
    int id;
    Font *font;
    
    font= new_Font(fid, ps, face);
    for (id= 0; id<MaxFonts; id++) {
	if (fonts[id] == 0) {
	    fonts[id]= font;
	    break;
	}
    }
    if (id >= MaxFonts)
	return -1;
    return id;
}

void Connection::Doit(struct Message *m)
{
    byte *lp, c, list[MaxTextBatchCnt];
    struct Response rp;
    Font *font;
    Bitmap *bitmap;
    WindowPort *wport;
    Point pos;
    short d;
    bool first;
    register int i;
    
    rp.port= 0;
    rp.t.Flags= m->seq; 
    rp.t.At= 0;
    rp.t.Pos.x= 0;
    
    switch (m->tag) {
    case eMsgMakeWindow:
	rp.t.Code= NewWindowPort((void*) m->i1, (bool) m->s1, (bool) m->s2);
	Write(&rp, sizeof(struct Response));
	return;
	
    case eMsgMakeFont:
	rp.t.Code= NewFont(m->s1, m->s2, m->s3);
	rp.t.Pos.x= sizeof(Font)-sizeof(void*);
	Write(&rp, sizeof(struct Response), FALSE);
	Write((void*) ((u_long)MapFont(rp.t.Code)+sizeof(void*)), rp.t.Pos.x);
	return;
    
    case eMsgMakeBitmap:
	rp.t.Code= NewBitmap(m->s1, m->p1, m->s2);
	Write(&rp, sizeof(struct Response));
	return;
	
    case eMsgDelay:
	Wait(m->i1);
	Write(&rp, sizeof(struct Response));
	return;
    }

    if ((wport= MapPort(m->id)) == 0) {
	Error("Doit", "no such window (id:%d)", m->id);
	return;
    }
    
    GrSetPort(wport);
    
    switch (m->tag) {
    
    case eMsgShow:
	wport->DevShow1(MapPort(m->i1), m->r1);
	break;
	
    case eMsgTop:
	wport->DevTop((bool) m->s1);
	Write(&rp, sizeof(struct Response));
	break;
	
    case eMsgHide:
	wport->DevHide1();
	break;
	
    case eMsgDestroy:
	wport->Destroy();
	ports[m->id]= 0;
	break;
	
    case eMsgStrokeRect:
	wport->DevStrokeRect(m->s1, m->i1, &m->r1);
	break;
	
    case eMsgFillRect:
	wport->DevFillRect(m->s1, &m->r1);
	break;
	
    case eMsgStrokeRRect:
	wport->DevStrokeRRect(m->s1, m->i1, &m->r1, m->p1);
	break;
	
    case eMsgFillRRect:
	wport->DevFillRRect(m->s1, &m->r1, m->p1);
	break;
	
    case eMsgStrokeOval:
	wport->DevStrokeOval(m->s1, m->i1, &m->r1);
	break;
	
    case eMsgFillOval:
	wport->DevFillOval(m->s1, &m->r1);
	break;
	
    case eMsgStrokeWedge:
	wport->DevStrokeWedge(m->s1, m->i1, m->s2, &m->r1, m->p1.x, m->p1.y);
	break;
	
    case eMsgFillWedge:
	wport->DevFillWedge(m->s1, &m->r1, m->p1.x, m->p1.y);
	break;

    case eMsgClip:
	wport->DevClip(m->r1, m->p1);
	break;
	
    case eMsgSetRect:
	wport->DevSetRect(&m->r1);
	break;
	
    case eMsgGrab:
	wport->DevGrab((bool) m->s1, (bool) m->s2);
	break;
	
    case eMsgSetMousePos:
	wport->DevSetMousePos(m->p1, (bool) m->s1);
	break;
	
    case eMsgBell:
	wport->DevBell((long)m->i1);
	break;
	
    case eMsgStrokeLine:
	wport->DevStrokeLine(m->s1, m->i1, &m->r1, m->s2, m->p1, m->p2);
	break;
	
    case eMsgShowTextBatch:
	Read(list, m->s2);
	lp= list;
	pos= m->p1;
	first= TRUE;
	font= gSysFont;
	while (lp-list < m->s2) {
	    if ((c= *lp++) == '#') {
		switch (c= *lp++) {
		case '#':
		    break;
		case 'x':
		    d= *lp++ << 8;
		    d|= *lp++;
		    pos.x+= d;
		    continue;
		case 'y':
		    d= *lp++ << 8;
		    d|= *lp++;
		    pos.y+= d;
		    continue;
		case 'f':
		    if ((font= MapFont((int)(*lp++))) == 0) {
			Error("Doit", "no such font (id:%d)", (int)lp[-1]);
			font= gSysFont;
		    }
		    continue;
		}
	    }
	    wport->DevShowChar(font, gPoint0, c, first, pos);
	    first= FALSE;
	    pos.x+= font->Width(c);
	}
	wport->DevShowTextBatch(m->s1, &m->r1, m->p1);
	break;
	
    case eMsgShowBitmap:
	if ((bitmap= MapBitmap(m->i1)) == 0) {
	    Error("Doit", "no such bitmap (id:%d)", m->i1);
	    return;
	}
	wport->DevShowBitmap(m->s1, &m->r1, bitmap);
	break;
	
    case eMsgResetClip:
	wport->DevResetClip();
	break;
	
    case eMsgGiveHint:
	if (m->s2)
	    Read(list, m->s2);
	wport->DevGiveHint(m->s1, m->s2, list);
	break;
	
    case eMsgScrollRect:
	wport->DevScrollRect(m->r1, m->p1);
	Write(&rp, sizeof(struct Response));
	break;
	
    case eMsgSetCursor:
	wport->DevSetCursor((GrCursor)m->s1);
	break;

    case eMsgHaveClipboard:
	clipboard= this;
	rp.t.Code= eMsgNotOwner;
	for (i= 0; i < cMaxConnections; i++)
	    if (connections[i] && connections[i] != this)
		connections[i]->Write(&rp, sizeof(struct Response));
	SafeDelete(bp);
	cachegood= FALSE;
	break;
	
    case eMsgGetClipboard:
	if (clipboard) {
	    if (! cachegood) {
		clipboard->Write(&m, sizeof(struct Response));
		clipboard->Read(&cache, sizeof(struct Response));
		SafeDelete(bp);
		bp= new char[rp.t.Pos.x];
		clipboard->Read(bp, rp.t.Pos.x);
		cachegood= TRUE;
	    }
	    Write(&cache, sizeof(struct Response), FALSE);
	    Write(bp, rp.t.Pos.x);
	}
	break;

    default:
	Error("Doit", "unknown request (tag:%d)", m->tag);
	break;
    }
}

//---- SunServerSystem ---------------------------------------------------------

class SunServerSystem: public SunSystem {
public:
    SunServerSystem()
	{ }
    bool Init();
};

//---- AcceptHandler -----------------------------------------------------------

class AcceptHandler : public SysEvtHandler {
public:
    AcceptHandler(int r) : SysEvtHandler(r)
	{ }
    void Notify(SysEventCodes, int);
};

void AcceptHandler::Notify(SysEventCodes, int)
{    
    int msgsock= accept(GetResourceId(), 0, 0);
    if (msgsock < 0) {
	SysError("Notify", "");
	return;
    }
    new Connection(msgsock);
}

//---- SunServerSystem ---------------------------------------------------------

bool SunServerSystem::Init()
{
    int sock= -1;
    
    if (SunSystem::Init())
	return TRUE;
    
    if ((sock= TcpService(SERVICENAME)) >= 0)
	AddFileInputHandler(new AcceptHandler(sock));
	
    if ((sock= UnixService(SERVERNAME)) >= 0)
	AddFileInputHandler(new AcceptHandler(sock));
    
    if (sock < 0) {
	Warning("SunServerSystem::Init", "no socket found");
	return FALSE;
    }
    
    cerr << "server ready\n";
    return FALSE;
}

//---- Main ---------------------------------------------------------------------

main()
{
    extern WindowSystem *NewSunWindowSystem();

    gSystem= new SunServerSystem();
    if (gSystem->Init()) {
	Error("ONENTRY", "can't init operating system");
	_exit(1);
    }
    gWindowSystem= NewSunWindowSystem();
    if (gWindowSystem == 0)
	_exit(1);
    gWindowSystem->Init();
    gSystem->Control();
    SafeDelete(gSystem);
    SafeDelete(gWindowSystem);
}
