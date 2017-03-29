#ifndef SunSystem_First
#define SunSystem_First

#include "System.h"
#include "Directory.h"

//---- SunDirectory ------------------------------------------------------------

class SunDirectory : public Directory {
    void *dirp;
public:
    SunDirectory(char *name);
    ~SunDirectory();
    char *operator()();
};

//---- SunSystem ---------------------------------------------------------------

class SunSystem: public System {
    SysEvtHandler *zombiehandler;
    int maxfd;
public:
    SunSystem();
    ~SunSystem();
    bool Init();
    void InnerLoop();
    bool ExpandPathName(char *patbuf, int buflen);
    void GetTtyChars(char &backspace, char &rubout);
    class Directory *MakeDirectory(char *name);
    bool AccessPathName(char *path, int mode= 0);
    bool ChangeDirectory(char *path);
    char *WorkingDirectory();
    char *HomeDirectory();
    void  Rename(char *from, char *to);
    char *GetError();

    bool CanRead(int fd, int timeout);
    void wait(unsigned int duration);
    void SetUpMask(u_int*, class Collection*);
    void DispatchReadWrite(u_int*, class Collection*);
    void DispatchSignals(bool asynch);
    class PttyConnection *MakePttyConnection(char *prog, char **args);
    class FileType *GetFileType(char *path, bool shallow);

    //---- environment manipulation -----------------------------
    void Setenv(char *name, char *value);
    char *getenv(char *);

    //---- Dynamic Loading --------------------------------------
    int Call(CallFunc cf, void *p1, void *p2, void *p3, void *p4);
    Object *Load(char *progname, char *name);

};

extern System *NewSunSystem();

#endif SunSystem_First
