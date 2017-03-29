#include "SunClipboard.h"

#include "Error.h"
#include "Clipboard.h"
#include "IO/membuf.h"
#include "FileType.h"
#include "StreamConnection.h"
#include "sunsockets.h"
#include "String.h"

extern char *gProgname;

//---- ClipboardConnection -----------------------------------------------------

class ClipboardConnection : public StreamConnection {
public:
    SunClipBoard *clip;

public:
    ClipboardConnection(SunClipBoard*, bool*);
    void Dispatch(ServerMessages m, byte *b, u_long);
};

//---- SunClipBoard ------------------------------------------------------------

SunClipBoard::SunClipBoard()
{
    bool ok;
    conn= new ClipboardConnection(this, &ok);
    if (ok)
	conn->send(eMsgEnroll, (byte*) gProgname, strlen(gProgname));
    else
	SafeDelete(conn);
}

SunClipBoard::~SunClipBoard()
{
    if (conn) {
	conn->send(eMsgCloseDown);
	SafeDelete(conn);
    }
}

char *SunClipBoard::DevGetType()
{
    return type;
}

void SunClipBoard::ScratchChanged(char *t)
{
    if (conn)
	conn->send(eMsgHaveClipboard, (byte*) t, strlen(t));
}

membuf *SunClipBoard::MakeBuf(char*)
{
    membuf *mb= 0;
    
    if (conn) {
	char *buf= 0;
	u_long l= 0;
	conn->send(eMsgGetClipboard);
	conn->Receive((byte**) &buf, &l);
	if (l > 0) {
	    mb= new membuf(0);
	    mb->setbuf(buf, (int)l);
	    mb->seek(l, TRUE);
	}
    }
    return mb;
}

//---- ClipboardConnection -----------------------------------------------------

ClipboardConnection::ClipboardConnection(SunClipBoard *sc, bool *ok)
							: StreamConnection(-1)
{
    clip= sc;
    *ok= Open(ConnectService(SERVERNAME, SERVICENAME));
}

void ClipboardConnection::Dispatch(ServerMessages m, byte *b, u_long ul)
{
    switch ((int)m) {
    case eMsgNone:
	return;
    
    case eMsgGetClipboard:
	b= (byte*) clip->GetBuf(&ul);
	send(eMsgGetClipboard, b, ul);
	break;
	
    case eMsgNotOwner:
	clip->NotOwner((char*)b);
	break;
	
    default:
	Warning("ClipboardConnection::Dispatch", "wrong request");
	Remove();
	break;
    }
}

