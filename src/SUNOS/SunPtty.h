#ifndef SunPtty_First
#define SunPtty_First

#include "PttyConnection.h"

//---- Pseudotty connection for SunOs ----------------------------------------

class SunOsPttyConnection: public PttyConnection {
    FILE *slave;
    char *slaveTtyName;
    int mode, pid, tslot;
    PttyChars pttychars;
public:
    SunOsPttyConnection(char *name, char **args);
    ~SunOsPttyConnection();
    int GetFileNo();
    FILE *GetFile(); // returns 0 when connection cannot be established
    int GetPid();
    bool SubmitToSlave(char *, int);
    int Read(char *buf, int size);
    int GetMode();
    void KillChild();
    void CleanupPtty();
    bool Echo();
    bool RawOrCBreak();
    void BecomeConsole();
    void SetSize(int rows, int cols);
    void GetPttyChars(PttyChars *);
};

#endif SunPtty_First
