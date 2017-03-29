#ifndef ServerConnection_First
#define ServerConnection_First

#include "WindowPort.h"
#include "Server.h"
#include "System.h"

class ServerConnection : public SysEvtHandler {
    int sock, sequence;
    FILE *ifp, *ofp;

public:
    ServerConnection();
    void Notify(SysEventCodes, int);
    void Close();
    bool HasInterest();
    bool SafeRead(Response *b, int timeout, bool overread);
    void SendMsg(int tag, int ref, char *fmt= 0, ...);
    void Flush();
};

extern ServerConnection *gConnection;

#endif ServerConnection_First
