#include <stdio.h>

#include "String.h"
#include "Error.h"
#include "Clipboard.h"
#include "SUNOS/SunSystem.h"
#include "sunsockets.h"
#include "StreamConnection.h"

const int cMaxConnections= 32;

class ClientConnection *connections[cMaxConnections];
static ClientConnection *clipboard;

//---- dummies -----------------------------------------------------------------

char *gProgname;
bool gWinInit;
void InitProgenv() {};

//---- ClientConnection --------------------------------------------------------

class ClientConnection : public StreamConnection {
    char *name;
    
public:
    ClientConnection(int f) : StreamConnection(f)
	{ connections[f]= this; }
    ~ClientConnection()
	{ connections[GetResourceId()]= 0; }
    
    void Dispatch(ServerMessages m, byte *buf, u_long len);
};

void ClientConnection::Dispatch(ServerMessages m, byte *b, u_long)
{
    static bool cachegood= FALSE;
    static int tag;
    static u_long len;
    static byte *buf= 0, *type= 0;

    int i;
    
    switch ((int)m) {
    
    case eMsgEnroll:
	name= (char*) b;
	if (type)
	    send(eMsgNotOwner, type, strlen((char*) type));
	if (gDebug)
	    cerr << "eMsgEnroll: " << name NL;
	break;
	
    case eMsgError:
	if (clipboard == this) {
	    clipboard= 0;
	    type= 0;
	    cachegood= FALSE;
	}
	break;

    case eMsgCloseDown:
	if (gDebug)
	    cerr << "eMsgCloseDown: " << name NL;
	if (clipboard == this) {    // need to copy
	    clipboard= 0;
	    type= 0;
	    cachegood= FALSE;
	}
	Remove();
	break;
    
    case eMsgHaveClipboard:
	clipboard= this;
	type= b;
	
	for (i= 0; i < cMaxConnections; i++)
	    if (connections[i] && connections[i] != this)
		connections[i]->send(eMsgNotOwner, type, strlen((char*) type));
	SafeDelete(buf);
	cachegood= FALSE;
	if (gDebug)
	    cerr << "eMsgHaveClipboard: " << name SP << (char*) type NL;
	break;
	
    case eMsgGetClipboard:
	if (clipboard) {
	    if (! cachegood) {
		clipboard->send(eMsgGetClipboard);
		tag= clipboard->Receive(&buf, &len);
		cachegood= TRUE;
	    }
	    send((enum ServerMessageTags) tag, buf, len);
	}
	if (gDebug)
	    cerr << "eMsgGetClipboard: " << name NL;
	break;
	
    default:
	Error("Dispatch", "unknown message");
    }
}

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
	SysError("Notify", "accept");
	return;
    }
    new ClientConnection(msgsock);
}

//---- SunClipboardSystem ------------------------------------------------------

class SunClipboardSystem: public SunSystem {
public:
    SunClipboardSystem()
	{ }
    bool Init();
};

bool SunClipboardSystem::Init() 
{
    int sock= -1;
    
    if (SunSystem::Init())
	return TRUE;
    
    if ((sock= TcpService(SERVICENAME)) >= 0)
	AddFileInputHandler(new AcceptHandler(sock));
	
    if ((sock= UnixService(SERVERNAME)) >= 0)
	AddFileInputHandler(new AcceptHandler(sock));
    
    if (sock < 0) {
	Warning("SunClipboardSystem::Init", "no socket found");
	return TRUE;
    }
    
    cerr << "clipboard ready\n";
    return FALSE;
}

//---- Main --------------------------------------------------------------------

main(int, char *argv[])
{
    gProgname= argv[0];
    gSystem= new SunClipboardSystem();
    if (gSystem->Init()) {
	Warning("main", "can't init operating system");
	return 1;
    }
    gSystem->Control();
    SafeDelete(gSystem);
    return 0;
}
