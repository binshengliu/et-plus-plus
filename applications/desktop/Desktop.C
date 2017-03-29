//$AcceptHandler,ClientConnection,desktop,Client$

#include "ET++.h"
#include "CollView.h"
#include "ObjList.h"
#include "Buttons.h"

#include "SUNWINDOW/Clipboard.h"
#include "SUNOS/SunSystem.h"
#include "SUNWINDOW/sunsockets.h"
#include "IO/stdio.h"
#include "StreamConnection.h"

static class Client *clipboard;
static bool cachegood= FALSE;
static byte *bp= 0;
static byte *type= 0;
static ServerMessages tag;
static u_long len;

//---- ClientConnection --------------------------------------------------------

class ClientConnection : public StreamConnection {
    class Client *cl;
    
public:
    ClientConnection(int f, Client *c) : StreamConnection(f)  
	{ cl= c; }
    void Dispatch(ServerMessages m, byte *b, u_long);
};

//---- Client -----------------------------------------------------------------

class Client: public TextItem {
public:
    class StreamConnection *conn;
    byte *name;
public:
    MetaDef(Client);
    Client(int sock);
    ~Client();
    void SetName(byte *n);
    void ResetName();
    void SetType(byte *t);
};

MetaImpl0(Client);

//---- desktop -----------------------------------------------------------------

class desktop: public Application {
    CollectionView *view;
public:
    MetaDef(desktop);
    desktop(int argc, char **argv);
    VObject *DoCreateDialog();
    void AddClient(Client *cl);
    void RemoveClient(Client *cl);
    void Update();
    void Broadcast(Client*, byte *type);
};

desktop *gDesktop;

//---- ClientConnection -----------------------------------------------------

void ClientConnection::Dispatch(ServerMessages m, byte *b, u_long)
{
    switch ((int)m) {
    
    case eMsgEnroll:
	cl->SetName(b);
	break;
	
    case eMsgError:
    case eMsgCloseDown:
	if (clipboard == cl) {    // need to copy
	    if (clipboard)
		clipboard->ResetName();
	    clipboard= 0;
	    type= 0;
	    cachegood= FALSE;
	}
	gDesktop->RemoveClient(cl);
	Remove();
	break;
    
    case eMsgHaveClipboard:
	if (clipboard)
	    clipboard->ResetName();
	clipboard= cl;
	type= b;
	gDesktop->Broadcast(cl, type);
	break;
	
    case eMsgGetClipboard:
	if (clipboard) {
	    if (! cachegood) {
		clipboard->conn->send(eMsgGetClipboard);
		tag= clipboard->conn->Receive(&bp, &len);
		cachegood= TRUE;
	    }
	    send(tag, bp, len);
	}
	break;
	
    default:
	Error("Dispatch", "unknown message");
    }
    gDesktop->Update();
}

//---- Client -----------------------------------------------------------------

Client::Client(int sock) : TextItem("<no name>")
{
    conn= new ClientConnection(sock, this);
}

Client::~Client()
{
    SafeDelete(conn);
}

void Client::SetName(byte *n)
{
    name= n;
    SetString((char*)name, TRUE);
}

void Client::ResetName()
{
    SetString((char*)name, TRUE);
}

void Client::SetType(byte *t)
{
    SetFString(TRUE, "%s (%s)", name, t);
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
    if (msgsock < 0)
	SysError("Notify", "accept");
    else
	gDesktop->AddClient(new Client(msgsock));
}

//---- desktop -----------------------------------------------------------------

MetaImpl0(desktop);

void desktop::AddClient(Client *cl)
{
    view->GetCollection()->Add(cl);
    view->Modified();
}

void desktop::RemoveClient(Client *cl)
{
    view->GetCollection()->Remove(cl);
    view->Modified();
}

void desktop::Update()
{
    view->ForceRedraw();
}

void desktop::Broadcast(Client *c, byte *type)
{
    Iter next(view->GetCollection());
    Client *cl;
    int l= strlen((char*)type);
    
    while (cl= (Client*) next())
	if (cl != c)
	    cl->conn->send(eMsgNotOwner, type, l);
    c->SetType(type);
    cachegood= FALSE;
}

desktop::desktop(int argc, char **argv) : (argc, argv)
{   
    int sock= -1;
    
    ApplInit();
    gDesktop= this;
    if ((sock= TcpService(SERVICENAME)) >= 0)
	gSystem->AddFileInputHandler(new AcceptHandler(sock));
	
    if ((sock= UnixService(SERVERNAME)) >= 0)
	gSystem->AddFileInputHandler(new AcceptHandler(sock));
    
    if (sock < 0)
	Warning("desktop", "no socket found");
}

VObject *desktop::DoCreateDialog()
{
    view= new CollectionView(this, new ObjList);
    return
	new Expander(cIdNone, eVert, 5,
	    new ActionButton(cQUIT, "quit"),
	    new Scroller(view, Point(150, 16*7)),
	    0
	);
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    desktop desktop(argc, argv);
    
    return desktop.Run();
}
