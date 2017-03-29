#include "SunSystem.h"
#include "Error.h"
#include "String.h"
#include "ObjList.h"
#include "SunFileType.h" 
#include "SunPtty.h"
#include "dynlink.h"
#include "sunsystem.h"


#ifndef __GNUG__
    extern "C" char *getenv(const char *);
#endif

extern int errno;
extern char* sys_errlist[];
extern int sys_nerr;

//---- signalHandler -----------------------------------------------------------

static bool signals[MAXSIGNALS];
static bool inCall;

static void sigHandler(Signals sig)
{
    if (inCall)
	sunsystem_LongJmp();
    else {
	signals[sig]= TRUE;
	if (gSystem)
	    ((SunSystem*)gSystem)->DispatchSignals(TRUE);
    }
}

//---- ZombieHandler -----------------------------------------------------------

class ZombieHandler : public SysEvtHandler {
    Collection *zombieHandler;    
public:
    ZombieHandler(Collection *zh) : SysEvtHandler(eSigChild)
	{ zombieHandler= zh; }
    void Notify(SysEventCodes, int);
};

void ZombieHandler::Notify(SysEventCodes, int)
{
    int pid;

    while ((pid= waitchild()) > 0) {
	Iter next(zombieHandler);
	SysEvtHandler *re;
	while (re= (SysEvtHandler*) next()) {
	    if (re->GetResourceId() == pid) {
		re->Notify(eSysEvtZombie, pid);
		re->Remove();
	    }
	}
    }
}

//---- DefaultSigHandler --------------------------------------------------------------

class DefaultSigHandler : public SysEvtHandler {
public:
    DefaultSigHandler(Signals s) : SysEvtHandler(s)
	{ }
    void Notify(SysEventCodes, int);
};

void DefaultSigHandler::Notify(SysEventCodes, int)
{
    cerr << "Got signal: " << sunsystem_signame((Signals)GetResourceId()) NL;
    if (gSystem)
	gSystem->abort();
}

//---- SunSystem ---------------------------------------------------------------

System *NewSunSystem()
{
    return new SunSystem();
}

SunSystem::SunSystem() : System("SUNOS")
{
}

SunSystem::~SunSystem()
{
    sunsystem_resetsignal(eSigBus);
    sunsystem_resetsignal(eSigSegmentationViolation);
    sunsystem_resetsignal(eSigSystem);
    sunsystem_resetsignal(eSigPipe);
    sunsystem_resetsignal(eSigIllegalInstruction);
    sunsystem_resetsignal(eSigQuit);
    sunsystem_resetsignal(eSigInterrupt);
    sunsystem_resetsignal(eSigWindowChanged);
    sunsystem_resetsignal(eSigAlarm);
    sunsystem_resetsignal(eSigChild);
    sunsystem_resetsignal(eSigUrgent);
    
    zombiehandler->Remove();
}

bool SunSystem::Init()
{
    if (System::Init())
	return TRUE;

    AddSignalHandler(zombiehandler= new ZombieHandler(zombieHandler));

    sunsystem_signal(eSigBus, sigHandler);
    sunsystem_signal(eSigSegmentationViolation, sigHandler);
    sunsystem_signal(eSigSystem, sigHandler);
    sunsystem_signal(eSigPipe, sigHandler);
    sunsystem_signal(eSigIllegalInstruction, sigHandler);
    sunsystem_signal(eSigQuit, sigHandler);
    sunsystem_signal(eSigInterrupt, sigHandler);
    sunsystem_signal(eSigWindowChanged, sigHandler);
    sunsystem_signal(eSigAlarm, sigHandler);
    sunsystem_signal(eSigChild, sigHandler);
    sunsystem_signal(eSigUrgent, sigHandler);

    //--- install default handlers
    gSystem->AddSignalHandler(new DefaultSigHandler(eSigBus), FALSE);
    gSystem->AddSignalHandler(new DefaultSigHandler(eSigSegmentationViolation), FALSE);
    gSystem->AddSignalHandler(new DefaultSigHandler(eSigSystem), FALSE);
    gSystem->AddSignalHandler(new DefaultSigHandler(eSigPipe), FALSE);
    gSystem->AddSignalHandler(new DefaultSigHandler(eSigIllegalInstruction), FALSE);
    return FALSE;
}

void SunSystem::SetUpMask(u_int* m, Collection *hl)
{
    Iter next(hl);
    register SysEvtHandler *re;

    while (re= (SysEvtHandler*) next()) 
	if (re->HasInterest()) {
	    FD_SET(re->GetResourceId(), (fd_set*) m);
	    maxfd= max(maxfd, re->GetResourceId());
	}
}

void SunSystem::DispatchReadWrite(u_int* m, Collection *hl)
{
    Iter next(hl);
    register SysEvtHandler *re;

    while (re= (SysEvtHandler*) next()) {
	int fd= re->GetResourceId();
	if (FD_ISSET(fd, (fd_set*) m) && re->HasInterest())
	    re->Notify(eSysEvtRead, fd);
    }
}

void SunSystem::DispatchSignals(bool async)
{
    Collection *shl= async ? asyncSignalHandler : signalHandler;
    Iter next(shl);
    register SysEvtHandler *re;
    int sig;

    while (re= (SysEvtHandler *)next()) {
	sig= re->GetResourceId();
	if (signals[sig] && re->HasInterest()) {
	    signals[sig]= FALSE;
	    re->Notify(eSysEvtSignal, sig);
	}
    }
}

void SunSystem::InnerLoop()
{
    fd_set readready, writeready;
    int nfds;
    
    if (signalHandler->Size())
	DispatchSignals(FALSE);          // poll interrupts

    maxfd= 0;
    FD_ZERO(&readready);
    FD_ZERO(&writeready);
    if (fileInputHandler->Size())
	SetUpMask((u_int*) &readready, fileInputHandler);
    if (fileOutputHandler->Size())
	SetUpMask((u_int*) &writeready, fileOutputHandler);

    nfds= sunwindow_select(maxfd+1, &readready, &writeready, 300);
    
    switch (nfds) {
    case -2:    // interrupt
	return;
    case -1:    // error
	SysError("SunSystem::InnerLoop", "sunwindow_select");
	return;
    case 0:     // timeout
	{
	    Iter next(timeoutHandler);
	    register SysEvtHandler *eh;
	
	    while (eh= (SysEvtHandler*) next())
		if (eh->HasInterest())
		    eh->Notify(eSysEvtTimeout, 0);
	}
	return;
    default:
	if (fileInputHandler->Size())
	    DispatchReadWrite((u_int*) &readready, fileInputHandler);
	if (fileOutputHandler->Size())
	    DispatchReadWrite((u_int*) &writeready, fileOutputHandler);
	return;
    }
}

bool SunSystem::CanRead(int fd, int timeout)
{
    fd_set readready;
    int rval;
		   
    if (timeout < 0) /* block */
	return FALSE;
    for (;;) {
	FD_ZERO(&readready);
	FD_SET(fd, &readready);
	if ((rval= sunwindow_select(fd+1, &readready, 0, timeout)) < 0) {
	    if (rval == -1)
		SysError("SunSystem::CanRead", "sunwindow_select");
	    continue;
	}
	if (rval == 0) // timeout
	    return TRUE;
	return FALSE;
    }
}

bool SunSystem::AccessPathName(char *path, int mode)
{
    if (access(path, mode) == 0)
	return FALSE;
    errorstr= sys_errlist[errno];
    return TRUE;
}

bool SunSystem::ChangeDirectory(char *path)
{
    return (bool) (chdir(path) == 0); 
}

char *SunSystem::WorkingDirectory()
{
    return workingdirectory();
}

char *SunSystem::HomeDirectory()
{
    return homedirectory();
}

Directory *SunSystem::MakeDirectory(char *name)
{
    return new SunDirectory(name);
}
    
FileType *SunSystem::GetFileType(char *path, bool shallow)
{
    return new SunFileType(path, shallow);
}

void SunSystem::Rename(char *f, char *t)
{
    ::rename(f, t);
    errorstr= sys_errlist[errno];
}

void SunSystem::GetTtyChars(char &backspace, char &rubout)
{
    if (sunsystem_GetTtyChars(&backspace, &rubout))
	System::GetTtyChars(backspace, rubout);
}

PttyConnection *SunSystem::MakePttyConnection(char *prog, char **args)
{
    return new SunOsPttyConnection(prog, args);
}

char *SunSystem::getenv(char *name)
{
    return ::getenv(name);
}

void SunSystem::Setenv(char *name, char *value)
{
#ifdef sony
    System::Setenv(name, value);
#else
    ::putenv(strprintf("%s=%s", name, value));
#endif
}

int SunSystem::Call(CallFunc cf, void *p1, void *p2, void *p3, void *p4)
{
    inCall= TRUE;
    int code= sunsystem_Call(cf, p1, p2, p3, p4);
    inCall= FALSE;
    return code;
}

char *SunSystem::GetError()
{
    if (errno < 0 || errno >= sys_nerr)
	return form("errno out of range %d", errno);
    return sys_errlist[errno];
}

//---- expand the metacharacters as in the shell ------------------------

bool SunSystem::ExpandPathName(char *patbuf, int buflen)
{
    errorstr= expandpathname(patbuf, buflen);
    return (bool) (errorstr != 0);
}

void SunSystem::wait(u_int duration)
{
    sunwindow_select(0, 0, 0, duration);
}

//---- dynamic loading and linking ---------------------------------------------

#ifdef __GNUG__
static void *Load2(char *progname, char *name, char *suf)
#else
static void *Load2(char *progname, char *name, char*)
#endif
{
    char *eentry;

    DynLinkInit(progname, gEtDir);
#ifdef __GNUG__
    eentry= form("__%s__ctor%s", name, suf);
#else
    eentry= form("___ct__%d%sFv", strlen(name), name);
#endif
    if (DynLookup(eentry))
       return (Object*) DynCall(eentry)(0, 0, 0, 0);
    DynLoad(name);
    return (Object*) DynCall(eentry)(0, 0, 0, 0);
}

Object *SunSystem::Load(char *progname, char *name)
{
    return (Object*) Load2(progname, name, "F_");
}

//---- SunDirectory ------------------------------------------------------------

SunDirectory::SunDirectory(char *name) : Directory(name)
{
    dirp= opendir(name);
}

SunDirectory::~SunDirectory()
{
    if (dirp)
	closedir(dirp);
}

char *SunDirectory::operator()()
{
    if (dirp)
	return getdirentry(dirp);
    return 0;
}
