#include <stdio.h>

#include "ServerConnection.h"
#include "ServerPort.h" 
#include "Error.h"
#include "String.h"
#include "SUNOS/sunsystem.h"
#include "SUNWINDOW/sunsockets.h"

extern "C" int close(int);

ServerConnection *gConnection;

static char *cSendErrorMsg= "ServerConnection::SendMsg";

ServerConnection::ServerConnection() : SysEvtHandler(0)
{
    ifp= ofp= 0;
    sequence= 4711;
    
    sock= ConnectService(SERVERNAME, SERVICENAME);
    if (sock >= 0) {
	ifp= fdopen(sock, "r");
	ofp= fdopen(sock, "w");
    }
    SetResourceId(sock);
}

bool ServerConnection::HasInterest()
{
    Flush();
    return TRUE;
}

void ServerConnection::Notify(SysEventCodes, int)
{
    Response rp;
    
    while (!SafeRead(&rp, 0, FALSE))
	if (rp.port)
	    rp.port->ServerSend(&rp.t);
}

void ServerConnection::Flush()
{
    if (ofp)
	fflush(ofp);
}

void ServerConnection::Close()
{
    if (ofp && ifp) {
	if (ofp) {
	    fflush(ofp);
	    fclose(ofp);
	}
	if (ifp)
	    fclose(ifp);
	::close(sock);
	Remove();
    }
}

bool ServerConnection::SafeRead(Response *b, int timeout, bool overread)
{
    for (;;) {
	if (ofp)
	    fflush(ofp);
	    
	b->port= 0;
	if (overread) {
	    bool first= TRUE;
	    timeout= 0;
	    do {
		if (myread(ifp, timeout, b, sizeof(struct Response)))
		    break;
		first= FALSE;
	    } while (b->t.Code == eEvtLocMove);
	    if (first)
		return TRUE;
	} else if (myread(ifp, timeout, b, sizeof(struct Response)))
	    return TRUE;
	    
	if (b->port) {
	    if (b->t.Code == eEvtDamage) {
		b->port->ServerSend(&b->t);
		continue;
	    }
	    return FALSE;
	}
	if (b->t.Pos.x > 0) {
	    b->t.At= (int) new byte[b->t.Pos.x];
	    fread((char*)b->t.At, b->t.Pos.x, 1, ifp);
	}
	return FALSE;
    }
}

//-------------------------------------------------------------------------------

/*
 i  integer                    integer
 s  short                      short
 p  Point                      Point*
 r  Rectangle                  Rectangle*
 b  byte string                int, byte*
*/

void ServerConnection::SendMsg(int tag, int ref, char *va_(format), ...)
{
    register char c, *fmt;
    short s, *sp;
    byte *bs= 0;
    Point *pp;
    int i, *ip, bl, intCnt= 0, shortCnt= 0, pointCnt= 0, rectCnt= 0;
    Rectangle *rp;
    Message m;
    Response resp;
    va_list ap;
    
    va_start(ap,va_(format));
    fmt= va_(format);
    
    m.tag= tag;
    m.id= ref;
    m.seq= sequence;
    if (fmt == 0) {
	if (fwrite((char*) &m, sizeof(Message), 1, ofp) == 0) 
	    SysError(cSendErrorMsg, "fwrite");
	goto out2; 
    }
    
    while (c= *fmt++) {
	switch (c) {
	case 's':               // short
	    s= va_arg(ap, int);
	    switch (shortCnt) {
	    case 0:
		m.s1= s;
		break;
	    case 1:
		m.s2= s;
		break;
	    case 2:
		m.s3= s;
		break;
	    default:
		Error(cSendErrorMsg, "no more shorts");
		break;
	    }
	    shortCnt++;
	    break;
	case 'i':               // int
	    i= va_arg(ap, int);
	    switch (intCnt) {
	    case 0:
		m.i1= i;
		break;
	    default:
		Error(cSendErrorMsg, "no more ints");
		break;
	    }
	    intCnt++;
	    break;
	case 'p':               // Point
	    pp= va_arg(ap, Point*);
	    switch (pointCnt) {
	    case 0:
		m.p1= *pp;
		break;
	    case 1:
		m.p2= *pp;
		break;
	    default:
		Error(cSendErrorMsg, "no more points");
		break;
	    }
	    pointCnt++;
	    break;
	case 'r':               // Rectangle
	    rp= va_arg(ap, Rectangle*);
	    switch (rectCnt) {
	    case 0:
		m.r1= *rp;
		break;
	    default:
		Error(cSendErrorMsg, "no more rectangles");
		break;
	    }
	    rectCnt++;
	    break;
	case 'b':               // byte*
	    bl= va_arg(ap, int);
	    bs= va_arg(ap, byte*);
	    switch (shortCnt) {
	    case 0:
		m.s1= bl;
		break;
	    case 1:
		m.s2= bl;
		break;
	    case 2:
		m.s3= bl;
		break;
	    default:
		Error(cSendErrorMsg, "no more shorts");
		break;
	    }
	    shortCnt++;
	    break;
	case ' ':
	    break;
	default:
	    if (c >= 'A' && c <= 'Z')
		goto out;
	    Error("ServerConnection", "SendFmt");
	    break;
	}
    }
out:
    
    if (fwrite((char*) &m, sizeof(Message), 1, ofp) == 0) 
	SysError(cSendErrorMsg, "fwrite");
    if (bs) {
	int res= 0;
	while (bl > 0) {
	    res= fwrite((char*) bs+res, 1, bl, ofp);
	    if (res == 0)
		SysError(cSendErrorMsg, "fwrite");
	    bl-= res;
	}
    }
	
    if (c) {
	for (;;) {
	    SafeRead(&resp, -1, FALSE); // block
	    if (resp.port)
		((ServerPort*)resp.port)->Enqueue(&resp);
	    else if (resp.t.Flags == sequence)
		break;
	}

	do {
	    switch (c) {
	    case 'S':               // short
		sp= (short*) va_arg(ap, int*);
		*sp= resp.t.Code;
		break;
	    case 'I':               // int
		ip= va_arg(ap, int*);
		*ip= resp.t.Code;
		break;
	    case 'P':               // Point
		pp= va_arg(ap, Point*);
		*pp= resp.t.Pos;
		break;
	    case 'R':               // Rectangle
		rp= va_arg(ap, Rectangle*);
		break;
	    case 'B':               // byte*
		bl= va_arg(ap, int);
		bs= va_arg(ap, byte*);
		if (bl != resp.t.Pos.x)
		    Error(cSendErrorMsg, "diff in bytecnt");
		bl= min(bl, resp.t.Pos.x);
		if (resp.t.At) {
		    if (bl > 0)
			BCOPY((byte*)resp.t.At, bs, bl);
		    delete (void*)resp.t.At;
		    resp.t.At= 0;
		}
		break;
	    case 'Z':
	    case 'Y':
	    case ' ':
		break;
	    default:
		Error("ServerConnection", "SendFmt");
		break;
	    }
	} while (c= *fmt++);
    }
	
out2:
    sequence++;
    va_end(ap);
}
