#ifndef System_First
#ifdef __GNUG__
#pragma once
#endif
#define System_First

#include "Object.h"

//---- class SysEvtHandler ---------------------------------------
// abstract class for handling events from system resources

enum SysEventCodes {
    eSysEvtRead,
    eSysEvtWrite,
    eSysEvtZombie,
    eSysEvtSignal,
    eSysEvtAsyncSignal,
    eSysEvtTimeout
};

class SysEvtHandler : public Object {
friend class System;
    class Collection *owner;
    int resourceId;
    void SetOwner(Collection *o)
	{ owner= o; }
protected:
    ~SysEvtHandler();
public:
    MetaDef(SysEvtHandler);
    SysEvtHandler(int resource);
    int GetResourceId();
    void SetResourceId(int);
    virtual void Notify(SysEventCodes code, int val);
    virtual bool HasInterest();
    virtual bool ShouldRemove();
    void Remove();
    bool IsEqual(Object *op);
};

//---- Signals -----------------------------------------------------------------

enum Signals {
    eSigBus,
    eSigSegmentationViolation,
    eSigSystem,
    eSigPipe,
    eSigIllegalInstruction,
    eSigQuit,
    eSigInterrupt,
    eSigWindowChanged,
    eSigAlarm,    
    eSigChild,
    eSigUrgent
};

//---- System ------------------------------------------------------------------

typedef void (*CallFunc)(void*, void*, void*, void*);


class System : public Root {
protected:
    bool done;
    char *errorstr, *osid;

    Collection *fileInputHandler;
    Collection *fileOutputHandler;
    Collection *zombieHandler;    
    Collection *signalHandler;    
    Collection *asyncSignalHandler;    
    Collection *timeoutHandler;
    Collection *cleanupList;

public:  
    static bool anyremoved;

public:
    System(char *name= "GENERIC");
    virtual ~System();

    virtual bool Init();

    char *GetErrorStr()
	{ return errorstr; }
    virtual char *GetError();
    char *GetSystemId()
	{ return osid; }
    virtual void Control();
    void ExitControl()
	{ done= TRUE; }
    virtual void InnerLoop();
    virtual bool CanRead(int fd, int timeout);

    virtual class Directory *MakeDirectory(char *name);
    virtual class FileType *GetFileType(char *path, bool shallow= FALSE);
    virtual void  Rename(char *from, char *to);
	
    //---- expand the metacharacters in Pat as in the shell
    virtual bool ExpandPathName(char *patbuf, int buflen);    
    virtual bool AccessPathName(char *path, int mode= 0);
    virtual bool ChangeDirectory(char *path);
    virtual char *WorkingDirectory();
    virtual char *HomeDirectory();
    
    virtual void exit(int code, bool mode= TRUE);
    virtual void GetTtyChars(char &backspace, char &rubout);
    virtual void wait(unsigned int);
    virtual void abort(int code= 0);

    //---- pseudo tty connection --------------------------------
    virtual class PttyConnection *MakePttyConnection(char *prog, char **args);

    //---- environment manipulation -----------------------------
    virtual void Setenv(char *name, char *value);
	// set environment variable name to value   
    virtual void Unsetenv(char *name);
	// remove environment variable
    virtual char *getenv(char *);

    //---- Dynamic Loading --------------------------------------
    virtual int Call(CallFunc cf, void *p1, void *p2, void *p3, void *p4);
    virtual Object *Load(char *progname, char *name);

    //---- handling of system events ----------------------------
    SysEvtHandler *AddFileInputHandler(SysEvtHandler *);
    SysEvtHandler *AddFileOutputHandler(SysEvtHandler *);
    SysEvtHandler *AddZombieHandler(SysEvtHandler *);
    SysEvtHandler *AddSignalHandler(SysEvtHandler*, bool sync= TRUE);
    SysEvtHandler *AddTimeoutHandler(SysEvtHandler *);
    SysEvtHandler *RemoveTimeoutHandler(SysEvtHandler *);
    void AddCleanupObject(Object*);
    void Remove();
};

extern System   *gSystem;
extern char     gBackspace;
extern char     gRubout;
extern char     *gEtDir;

inline void Wait(unsigned int duration)
    { gSystem->wait(duration); }

inline void Exit(int code, bool mode= TRUE)
    { gSystem->exit(code, mode); }

inline void Abort()
    { gSystem->abort(); }

inline char *Getenv(char *name)
    { return gSystem->getenv(name); }

extern "C" void _exit(int code);

#endif System_First
