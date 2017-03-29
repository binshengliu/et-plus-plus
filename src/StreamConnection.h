#ifndef StreamConnection_First
#ifdef __GNUG__
#pragma once
#endif
#define StreamConnection_First

#include <stdio.h>
#include "System.h"
#include "SUNWINDOW/Clipboard.h"

//---- StreamConnection --------------------------------------------------------

class StreamConnection : public SysEvtHandler {
protected:
    FILE *ifp, *ofp;
    
public:
    StreamConnection(int);
    ~StreamConnection();
    
    bool Open(int fd);
    void send(ServerMessages tag, byte *buf= 0, u_long l= 0);
    ServerMessages Receive(byte**, u_long*);
    void Notify(SysEventCodes, int);
    virtual void Dispatch(ServerMessages m, byte *buf, u_long len);
};

#endif StreamConnection_First

