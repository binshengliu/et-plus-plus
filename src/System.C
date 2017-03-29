//$System,SysEvtHandler,AbortHandler$
#include "System.h"
#include "ObjList.h"
#include "FileType.h"
#include "ObjectTable.h"
#include "String.h"
#include "Error.h" 
#include "Class.h"
#include "Directory.h"  
#include "OrdColl.h"

#include <stdio.h>
#include <osfcn.h>

char    gBackspace;
char    gRubout;
char    *gEtDir;
System  *gSystem;

extern char **environ;

//---- SysEvtHandler -------------------------------------------------

AbstractMetaImpl(SysEvtHandler, (T(resourceId), TP(owner), 0));

SysEvtHandler::SysEvtHandler(int id)
{
    owner= 0;
    resourceId= id;
}

SysEvtHandler::~SysEvtHandler()
{
}

int SysEvtHandler::GetResourceId()
{
    return resourceId;
}

void SysEvtHandler::SetResourceId(int id)
{
    resourceId= id;
}

void SysEvtHandler::Notify(SysEventCodes, int)
{
}

bool SysEvtHandler::HasInterest()
{
    return ! ShouldRemove();
}

bool SysEvtHandler::ShouldRemove()
{
    return owner == 0;
}

bool SysEvtHandler::IsEqual(Object *op)
{
    return resourceId == ((SysEvtHandler*)op)->resourceId;
}

void SysEvtHandler::Remove()
{
    if (owner) {
	owner->RemovePtr(this);
	owner= 0;
	gSystem->AddCleanupObject(this);
    } else if (resourceId >= 0)
	cerr << "SysEvtHandler::Remove() called more than once\n";
}

//---- System ------------------------------------------------------------------

System::System(char *name)
{
    osid= name;
}

bool System::Init()
{
    if ((gEtDir= getenv("ET_DIR")) == 0)
	gEtDir= "/local/et";

    ObjectTable::AddRoots(
	fileInputHandler    = new OrdCollection,
	fileOutputHandler   = new OrdCollection,
	zombieHandler       = new OrdCollection,
	signalHandler       = new OrdCollection,
	asyncSignalHandler  = new OrdCollection,
	timeoutHandler      = new OrdCollection,
	cleanupList         = new OrdCollection,
	0
    );
    
    GetTtyChars(gBackspace, gRubout);

    return FALSE;
}

static void FreeDelete(Collection *ol)
{
    if (ol) {
	// ol->FreeAll();
	SafeDelete(ol);
    }
}

System::~System()
{
    FreeDelete(fileInputHandler);
    FreeDelete(fileOutputHandler);
    FreeDelete(zombieHandler);
    FreeDelete(signalHandler);
    FreeDelete(asyncSignalHandler);
    FreeDelete(timeoutHandler);
    FreeDelete(cleanupList);
}

void System::GetTtyChars(char &backspace, char &rubout)
{
    backspace= '\b';
    rubout= 0x7f;
}

void System::exit(int code, bool mode)
{    
    if (mode)
	::exit(code);
    ::_exit(code);
}

void System::Control()
{
    extern bool gWinInit;
    gWinInit= TRUE;
    done= FALSE;
    while (! done) {
	InnerLoop();
	if (anyremoved)
	    Remove();
    }
}

void System::InnerLoop()
{
    AbstractMethod("InnerLoop");
}

Directory *System::MakeDirectory(char *name)
{
    return new Directory(name);
}

FileType *System::GetFileType(char *path, bool shallow)
{
    return new FileType(path, shallow);
}

bool System::AccessPathName(char*, int)
{
    return FALSE;
}

bool System::ExpandPathName(char*, int)
{
    return FALSE;
}
    
bool System::ChangeDirectory(char *)
{
    return FALSE;
}
    
char *System::WorkingDirectory()
{
    return 0;
}

char *System::HomeDirectory()
{
    return 0;
}

void System::Rename(char *from, char *to)
{
    unlink(to);
    if (link(from, to) != -1) 
	unlink(from);
}

void System::wait(unsigned int)
{
}

bool System::CanRead(int, int)
{
    return FALSE;
}

void System::Remove()
{
    cleanupList->FreeAll();
    cleanupList->Empty(cCollectionInitCap);
    anyremoved= FALSE;
}

SysEvtHandler *System::AddFileInputHandler(SysEvtHandler *re)
{
    SysEvtHandler *old= (SysEvtHandler*) fileInputHandler->Remove(re);
    fileInputHandler->Add(re);
    re->SetOwner(fileInputHandler);
    return old;
}

SysEvtHandler *System::AddFileOutputHandler(SysEvtHandler *re)
{
    SysEvtHandler *old= (SysEvtHandler*) fileOutputHandler->Remove(re);
    fileOutputHandler->Add(re);
    re->SetOwner(fileOutputHandler);
    return old;
}

SysEvtHandler *System::AddZombieHandler(SysEvtHandler *re)
{
    zombieHandler->Add(re);
    re->SetOwner(zombieHandler);
    return re;
}

SysEvtHandler *System::AddSignalHandler(SysEvtHandler *re, bool sync)
{
    Collection *shl= sync ? signalHandler : asyncSignalHandler;
    SysEvtHandler *fsh= (SysEvtHandler*) shl->Remove(re);
    shl->Add(re);
    re->SetOwner(shl);
    return fsh;
}

SysEvtHandler *System::AddTimeoutHandler(SysEvtHandler *re)
{
    timeoutHandler->Add(re);
    re->SetOwner(fileInputHandler);
    return re;
}
    
SysEvtHandler *System::RemoveTimeoutHandler(SysEvtHandler *re)
{
    return (SysEvtHandler *)timeoutHandler->RemovePtr(re);
}

void System::AddCleanupObject(Object *op)
{
    cleanupList->Add(op);
    if (gSystem)
	gSystem->anyremoved= TRUE;
}

void System::abort(int)
{
    ::abort();
}

PttyConnection* System::MakePttyConnection(char *, char **)
{
    return 0;
}

//---- environment manipulation ------------------------------------------------

char *System::getenv(char*)
{
    return 0;
}

void System::Setenv(char *, char *)
{
    Warning("Setenv", "not overridden");
}

void System::Unsetenv(char *name)
{
    register len;
    register char **ap, **newv;

    len= strlen(name);
    for (newv= ap= environ; *newv= *ap;  ap++) {
	if (strncmp(*ap, name, len) == 0  &&  (*ap)[len] == '=') {
	    /* Memory leak bug: we cannot free(*ap) here, because we don't know
	     * whether *ap was created with putenv(). */
	     ;
	} else
	    newv++;
    }
}

int System::Call(CallFunc cf, void *p1, void *p2, void *p3, void *p4)
{
    (*cf)(p1, p2, p3, p4);
    return 0;
}

Object *System::Load(char*, char*)
{
    return 0;
}

char *System::GetError()
{
    return form("errno: %d", errno);
}

