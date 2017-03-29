#include "ServerPort.h"
#include "Error.h"
#include "String.h"
#include "ServerFont.h"
#include "ServerBitmap.h"

#include "ServerConnection.h"

static byte *tblist, *tbcp;
static FontPtr lastfdp= 0;

//------------------------------------------------------------------------------

ServerPort::ServerPort(InpHandlerFun nf, void *priv1, bool ov, bool bl)
    : WindowPort (nf, priv1, ov, bl, 0)
{    
    gConnection->SendMsg(eMsgMakeWindow, -1, "ssiI", ov, bl, (int)this, &ref);
    out= 1;
    in= MQS;
    
    if (tblist == 0)
	tbcp= tblist= new byte[MaxTextBatchCnt];
}

void ServerPort::DevDestroy2()
{
    gConnection->SendMsg(eMsgDestroy, ref);
    gConnection->Flush();
}

void ServerPort::DevClip(Rectangle r, Point p)
{
    gConnection->SendMsg(eMsgClip, ref, "rp", &r, &p);
}

void ServerPort::DevResetClip()
{
    gConnection->SendMsg(eMsgResetClip, ref);
}

void ServerPort::DevStrokeLine(Ink pat, int ps, Rectangle *r,
					GrLineCap cap, Point p1, Point p2)
{
    gConnection->SendMsg(eMsgStrokeLine, ref, "ssppir", pat, cap, &p1, &p2, ps, r);
}

void ServerPort::DevStrokeRect(Ink pat, int ps, Rectangle *r)
{
    gConnection->SendMsg(eMsgStrokeRect, ref, "sri", pat, r, ps);
}

void ServerPort::DevFillRect(Ink pat, Rectangle *r)
{
    gConnection->SendMsg(eMsgFillRect, ref, "sr", pat, r);
}

void ServerPort::DevStrokeRRect(Ink pat, int ps, Rectangle *r, Point dia)
{
    gConnection->SendMsg(eMsgStrokeRRect, ref, "srip", pat, r, ps, &dia);
}

void ServerPort::DevFillRRect(Ink pat, Rectangle *r, Point dia)
{
    gConnection->SendMsg(eMsgFillRRect, ref, "srp", pat, r, &dia);
}

void ServerPort::DevStrokeOval(Ink pat, int ps, Rectangle *r)
{
    gConnection->SendMsg(eMsgStrokeOval, ref, "sri", pat, r, ps);
}

void ServerPort::DevFillOval(Ink pat, Rectangle *r)
{
    gConnection->SendMsg(eMsgFillOval, ref, "sr", pat, r);
}

void ServerPort::DevStrokeWedge(Ink pat, int ps, GrLineCap cap,
						Rectangle *r, int s, int l)
{
    Point p(s,l);
    gConnection->SendMsg(eMsgStrokeWedge, ref, "sisrp", pat, ps, cap, r, &p);
}

void ServerPort::DevFillWedge(Ink pat, Rectangle *r, int s, int l)
{
    Point p(s,l);
    gConnection->SendMsg(eMsgFillWedge, ref, "srp", pat, r, &p);
}

void ServerPort::DevStrokePolygon(Rectangle*, Ink, Point*, int, GrPolyType, int, GrLineCap)
{
}

void ServerPort::DevFillPolygon(Rectangle*, Ink, Point*, int, GrPolyType)
{
}

void ServerPort::DevShowBitmap(Ink pat, Rectangle *r, Bitmap *bm)
{
    gConnection->SendMsg(eMsgShowBitmap, ref, "sri", pat, r,
						((ServerBitmap*)bm)->GetId());
}

bool ServerPort::DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, Point)
{
    if (isnew) {
	tbcp= tblist;
	lastfdp= 0;
    }
    
    if (tbcp-tblist >= MaxTextBatchCnt-15)
	return TRUE;
	
    if (fdp != lastfdp) {
	*tbcp++= '#';
	*tbcp++= 'f';
	*tbcp++= (byte) ((ServerFont*)fdp)->GetId();
	lastfdp= fdp;
    }
    if (delta.x) {
	short d= delta.x;
	*tbcp++= '#';
	*tbcp++= 'x';
	*tbcp++= d >> 8;
	*tbcp++= d & 0xff;
    }
    if (delta.y) {
	short d= delta.y;
	*tbcp++= '#';
	*tbcp++= 'y';
	*tbcp++= d >> 8;
	*tbcp++= d & 0xff;
    }
    if (c == '#')
	*tbcp++= '#';
    *tbcp++= c;
    return FALSE;
}

void ServerPort::DevShowTextBatch(Ink pat, Rectangle *r, Point pos)
{
    gConnection->SendMsg(eMsgShowTextBatch, ref, "srpb", pat, r, &pos,
						    (int)(tbcp-tblist), tblist);
}

void ServerPort::DevGiveHint(int code, int len, void *vp)
{
    switch (code) {
    case eHintTextBatch:
    case eHintTextUnbatch:
    case eHintLock:
    case eHintUnlock:
	return;
    case eHintFlush:
	break;
    default:
	gConnection->SendMsg(eMsgGiveHint, ref, "sb", code, len, vp);
    }
    gConnection->Flush();
}

void ServerPort::DevScrollRect(Rectangle r, Point p)
{
    gConnection->SendMsg(eMsgScrollRect, ref, "rpZ", &r, &p);
}

void ServerPort::DevTop(bool top)
{
    gConnection->SendMsg(eMsgTop, ref, "sZ", top);
}

void ServerPort::DevHide()
{
    gConnection->SendMsg(eMsgHide, ref);
}

void ServerPort::DevShow(WindowPort *father, Rectangle r)
{
    gConnection->SendMsg(eMsgShow, ref, "ir",
				father ? ((ServerPort*)father)->ref : -1, &r);
}

void ServerPort::DevSetRect(Rectangle *r)
{
    gConnection->SendMsg(eMsgSetRect, ref, "r", r);
}

inline int addone(int i)
{
    return (i % MQS) + 1;
}

void ServerPort::Enqueue(Response *rp)
{
    if (addone(addone(in)) != out) {
	in= addone(in);
	rps[in]= *rp;
    }    
}

Response *ServerPort::Dequeue()
{
    Response *r;
    
    if (addone(in) == out)
	return 0;
    r= &rps[out];
    out= addone(out);
    return r;
}
   
void ServerPort::DevGetEvent(Token *t, int timeout, bool overread)
{
    Response *r, rp;
    bool first= overread;
    
//    do {
	while ((r= Dequeue()) == 0) {
	    if (gConnection->SafeRead(&rp, timeout, overread)) {
		if (first || ! overread) {
		    t->Code= eEvtNone;
		    t->Flags= 0;
		    return;
		}
	    }
	    if (rp.port)
		((ServerPort*)rp.port)->Enqueue(&rp);
	    first= FALSE;
	}
//    } while (r->port == 0);
    
    *t= r->t;
};

void ServerPort::DevGrab(bool on, bool fullscreen)
{
    gConnection->SendMsg(eMsgGrab, ref, "ss", on, fullscreen);
}
    
void ServerPort::DevSetMousePos(Point p, bool m)
{
    gConnection->SendMsg(eMsgSetMousePos, ref, "ps", &p, (short) m);
};

void ServerPort::DevBell(long d)
{
    gConnection->SendMsg(eMsgBell, ref, "i", (int) d);
}

void ServerPort::DevSetCursor(GrCursor c)
{
    gConnection->SendMsg(eMsgSetCursor, ref, "s", (short) c);
}
