//$StreamConnection$

#include <stdio.h>

#include "StreamConnection.h"
#include "Error.h"

//---- StreamConnection --------------------------------------------------------------

StreamConnection::StreamConnection(int f) : SysEvtHandler(f)
{
    ifp= ofp= 0;
    Open(f);
}

StreamConnection::~StreamConnection()
{
    if (ifp)
	fclose(ifp);
    if (ofp)
	fclose(ofp);
}
    
bool StreamConnection::Open(int fd)
{
    if (fd > 0) {
	gSystem->AddFileInputHandler(this);
	if (ifp= fdopen(fd, "r")) {
	    if (ofp= fdopen(fd, "w")) {
		SetResourceId(fd);
		return TRUE;
	    }
	}
    }
    return FALSE;
}

void StreamConnection::send(ServerMessages tag, byte *buf, u_long l)
{
    struct Header rp;
 
    rp.tag= tag;
    rp.len= (u_int) l;
    if (fwrite((char*)&rp, sizeof(struct Header), 1, ofp) != 1)
	Error("send", "fwrite1");
    if (buf && l > 0) {
	if (fwrite((char*)buf, sizeof(char), (int) l, ofp) != l)
	    Error("send", "fwrite2");
    }
    fflush(ofp);
}

ServerMessages StreamConnection::Receive(byte **buf, u_long *l)
{
    struct Header m;
    m.tag= eMsgError;
    m.len= 0;
    
    if (fread((char*)&m, sizeof(struct Header), 1, ifp) != 1) {
	Error("Receive", "fread1");
	Remove();
	return eMsgError;
    }
    if (m.len > 0) {
	SafeDelete(*buf);
	*buf= new byte[m.len+1];
	if (fread((char*)*buf, sizeof(char), m.len, ifp) != m.len) {
	    Error("Receive", "fread2");
	    Remove();
	    return eMsgError;
	}
	(*buf)[m.len]= 0;
	*l= m.len;
    }
    return m.tag;
}

void StreamConnection::Notify(SysEventCodes, int)
{
    byte *bp= 0;
    u_long len= 0;
    ServerMessages msg= Receive(&bp, &len);
    Dispatch(msg, bp, len);
}

void StreamConnection::Dispatch(ServerMessages, byte*, u_long)
{
}
