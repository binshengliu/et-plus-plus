#ifndef PttyConnection_First
#ifdef __GNUG__
#pragma once
#endif
#define PttyConnection_First

#include <stdio.h>

#include "Types.h"
#include "Root.h"

//---- abstract class for a connection with a pseudo tty ---------------------

struct PttyChars { // struct to return the settings of the various tty characters
    char erase;
    char kill;
    char rprnt;
    char susp;
    char intr;
    char quit;
    char eof;
};

class PttyConnection : public Root {
protected:
    PttyConnection();
public:
    virtual ~PttyConnection();
    virtual FILE *GetFile();
    virtual int GetPid();
    virtual int GetFileNo();
    virtual bool SubmitToSlave(char *buf, int n);
    virtual int Read(char *buf, int size);
    virtual int GetMode();
    virtual void KillChild();
    virtual void CleanupPtty();
    virtual bool Echo();
    virtual bool RawOrCBreak();
    virtual void BecomeConsole();
    virtual void SetSize(int rows, int cols);
    virtual void GetPttyChars(PttyChars *);
};

#endif PttyConnection_First
