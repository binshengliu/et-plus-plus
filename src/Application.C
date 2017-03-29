//$Application,ApplDialog,ApplIntHandler$

#include "ET++.h"

#include "Storage.h"
#include "ObjList.h"
#include "IdDictionary.h"
#include "WindowSystem.h"
#include "FileDialog.h"
#include "ObjectTable.h"
#include "ClassManager.h"
#include "ClipBoard.h"
#include "ProgEnv.h"
#include "FixedStorage.h"
#include "Buttons.h"
#include "BorderItems.h"

extern char **environ;

extern bool gInMain;

bool        gBatch= TRUE;
Application *gApplication= 0;
ClipBoard   *gClipBoard;
char        *gProgname;
int         gArgc;
char        **gArgv;
bool        gMemStatistics;

static int untitledCnt= 0;

extern bool forcemono;

//---- ApplIntHandler --------------------------------------------------------------

class ApplIntHandler : public SysEvtHandler {
public:
    ApplIntHandler() : SysEvtHandler(eSigInterrupt)
	{ }
    void Notify(SysEventCodes, int);
};

void ApplIntHandler::Notify(SysEventCodes, int)
{
    if (gApplication)
	gApplication->Inspect();
}

static void ApplErrorHandler(int level, bool, char *location, char *msg)
{
    gApplication->DoOnError(level, location, msg);
} 

//---- initial Application Dialog ----------------------------------------------

MetaImpl(ApplDialog, (TP(appl), 0));

ApplDialog::ApplDialog(Application *eh, char *title)
				: Dialog(title, eBWinDefault+eBWinFixed, eh)
{   
    appl= eh; 
}

VObject *ApplDialog::DoCreateDialog() 
{
    return appl->DoCreateDialog();
}

void ApplDialog::Control(int id, int p, void *d)
{   
    EvtHandler::Control(id, p, d);    
}

Point ApplDialog::GetInitialPos()
{
    return gPoint0;
} 

//---- Application -------------------------------------------------------------


AbstractMetaImpl(Application, (T(argc), TP(documents), TP(menu),
			TP(applDialog), T(lastDocPos), TB(printHierarchy),
			TP(version), TVP(argv, argc), TP(mainDocumentType),
			TP(gClassManager), TP(gObservers), TP(gWindow), T(gScreenRect),
			TPP(gEnviron), TB(gDebug), TB(gBatch), TB(inited), 0));

Application::Application(int ac, char **av, const char *dt)
{
    gInMain= TRUE;
    documents= 0;
    menu= 0;
    applDialog= 0;
    clipboard= 0;
    inited= FALSE;
    version= "Version 2.3, 12/1/90, \251IFI & UBS-UBILAB";

    if (gApplication)
	Error("Application", "only one Application !!");
    else {
	UpdateGlobals();
	ObjectTable::AddRoot(this);
	SetErrorHandler(ApplErrorHandler);
	gArgc= argc= ac;
	gArgv= argv= av;
	gProgname= argv[0];
	printHierarchy= FALSE;
	lastDocPos= Point(150, 100);
	documents= new ObjList;
	applDialog= 0;
	menu= 0;
	mainDocumentType= dt;
    }
}

Application::~Application()
{
    SetErrorHandler(DefaultErrorHandler);
    SafeDelete(documents);
    SafeDelete(applDialog);
    SafeDelete(menu);

    if (gMemStatistics) {
	PrintStorageStatistics();
	MemPools::PrintStatistics();
    }
}

void Application::ApplInit()
{
    if (! inited) {
	ParseCommandLine(argc, argv);
	ETInit();
	if (printHierarchy)
	    gClassManager->DisplayOn(cout);
	inited= TRUE;
    }
}

EvtHandler *Application::GetNextHandler()
{
    return 0;
}

FileDialog *Application::MakeFileDialog()
{
    return new FileDialog;
}

void Application::ParseCommandLine(int argc, char **argv)
{
    int n, i= 1;
    
    while (i < argc) {
	if (argv[i][0] == '-') {
	    n= DoParseOptions(argv[i], argv[i+1]);
	    if (n >= 0)
		i+= n;
	    else
		i++;
	} else {
	    ETInit();
	    OpenDocument(argv[i]);
	    i++;
	}
    }
}

int Application::DoParseOptions(char *arg0, char *arg1)
{
    int x= lastDocPos.x, y= lastDocPos.y;
    
    if (arg0[1] != 'E')
	return 0;

    switch(arg0[2]) {
    case 'd':   // debug
	::gDebug= !::gDebug;
	return 1;
    case 'b':   // double buffer
	::gBatch= !::gBatch;
	return 1;
    case 'c':   // toggle color monochrome
	forcemono= TRUE;
	return 1;
    case 'h':   // print hierarchie to cerr
	printHierarchy= !printHierarchy;
	return 1;
    case 'm':
	::gMemStatistics= !::gMemStatistics;
	return 1;
    case 'e':
	EditSource(TRUE);
	return 1;
    case 'E':
	gProgEnv->Start();
	return 1;
    case 'w':
	SetIgnoreLevel(0);
	return 1;
    case 'i':
	Inspect();
	return 1;
    case 'p':
	sscanf(arg1, "%d,%d", &x, &y);
	lastDocPos= Point(x,y);
	return 2;
    }
    return 0;
}

char *Application::ProgramName()
{
    char *p= rindex(gProgname, '/');
    if (p)
	return p+1;
    return gProgname;
}

Point Application::GetNewDocumentPos()
{
    Point p= lastDocPos;
    lastDocPos+= Point(40, 30);
    return p;
}

void Application::RemoveDocument(Document *dp)
{
    if (documents)
	documents->Remove(dp);
    gSystem->AddCleanupObject(dp);
}

Menu *Application::GetMenu()
{
    if (menu == 0)
	menu= new Menu(argv[0]);
    return menu;
}

void Application::About()
{
    ShowAlert(eAlertSun, "%s %s", argv[0], version);
}

void Application::HandleApplicationCommands(int cmd)
{
    switch(cmd) {

    case cNEW:
	NewDocument(mainDocumentType);
	break;

    case cQUIT:
	Quit();
	break;

    case cABOUT:
	About();
	break;

    case cSHOWAPPLWIN:
	ShowApplicationWindow();
	break;

    case cOPEN:
	Open();
	break;
	
    default:
	if (cmd >= cDEBUGFIRST && cmd <= cDEBUGLAST)
	    Debug(cmd-cDEBUGFIRST+1);
    }
}

Command *Application::DoMenuCommand(int cmd)
{  
    HandleApplicationCommands(cmd);          
    return gNoChanges;
}

void Application::Control(int id, int, void*)
{ 
    HandleApplicationCommands(id);
}

void Application::Quit()
{
    if (CloseAllDocuments())
	gSystem->ExitControl();
}

void Application::Debug(int)
{
    gDebug= (::gDebug= ! ::gDebug);
}

void Application::Open()
{
    if (fileDialog == 0)
	fileDialog= MakeFileDialog();
    if (fileDialog->ShowInWindow(eFDRead, applDialog->GetWindow(), this) == cIdOk)
	OpenDocument(fileDialog->FileName());
}

void Application::AddDocument(Document *dp)
{
    documents->Add(dp);
    dp->SetApplication(this);
}

Document *Application::FindDocument(int id)
{
    Document *dp;
    Iter next(documents);

    while (dp= (Document*) next())
	if (dp->UniqueId() == id)
	    return dp;
    return 0;
}

Document *Application::DoMakeDocuments(const char*)
{
    AbstractMethod("DoMakeDocuments");
    return 0;
}

bool Application::CloseAllDocuments()
{
    Document *dp;
    Iter next(documents);

    while (dp= (Document*) next()) // try to close all documents
	if (! dp->Close())
	    return FALSE;
    return TRUE;
}

void Application::OpenDocument(char *name)
{
    Document *dp;
    FType ft(name);

    if (!CanOpenDocument(ft.FileType())) {
	ShowAlert(eAlertNote, "cannot handle document @I%s@P (%s)\n", name, ft.Type());
	return;
    }
    dp= DoMakeDocuments(ft.Type()); // let user make his documents
    if (dp) {
	AddDocument(dp);
	dp->OpenWindows();
	dp->Load(name, TRUE, ft.FileType());
    }
}

void Application::NewDocument(const char *type)
{
    Document *dp;

    dp= DoMakeDocuments(type); // let user make his documents
    if (dp == 0) {
	Error("NewDocument", "DoMakeDocuments returns 0");
	return;
    }
    AddDocument(dp);
    dp->SetName(form("untitled.%d", untitledCnt++));
    dp->OpenWindows();
}

bool Application::CanOpenDocument(FileType *ft)
{
    if (strismember(mainDocumentType, cDocTypeUndef, ft->Type(), 0))
	return TRUE;
    if (strcmp(mainDocumentType, cDocTypeAscii) == 0 && ft->IsAscii())
	return TRUE;
    return FALSE;
}

int Application::Run()
{
    ApplInit();
    
    applDialog= new ApplDialog(this, gProgname);
    gSystem->AddSignalHandler(new ApplIntHandler);
    gClassManager->SetDynLoadHook(this, (OObjMemberFunc)&Application::DynLoad);

    gClipBoard= clipboard= gWindowSystem->MakeClipboard();
    
    OpenApplicationDialog(GetNewDocumentPos());
    
    gSystem->Control();
    
    SafeDelete(clipboard);
    gClipBoard= 0;

    applDialog->Close();
    
    return 0;
}

void Application::OpenApplicationDialog(Point p)
{
    applDialog->ShowAt(0, p);
}

int Application::DynLoad(char *name)
{
    Object *op= gSystem->Load(gProgname, name);
    if (op == 0) {
	Error("DynLoad", "cannot load class");
	return 0;
    }
    return DynLoadHook(op);
}

int Application::DynLoadHook(Object*)
{
    return 1;
}

VObject *Application::DoCreateDialog() 
{ 
    return new BorderItem(
		new Expander(cIdNone, eHor, 5,
		    new ActionButton(cNEW,  "new"),
		    new ActionButton(cOPEN, "open"),
		    new ActionButton(cQUIT, "quit"),
		    0
		),
		Point(5)
	    );
}

void Application::ShowApplicationWindow()
{
    if (applDialog)
	applDialog->GetWindow()->Open();    
}

void Application::InspectorId(char *buf, int sz)
{
    strn0cpy(buf, argv[0], sz);
}

void Application::Parts(Collection* col)
{
    EvtHandler::Parts(col);
    col->AddVector(documents, applDialog, 0);
}

void Application::UpdateGlobals()
{
    extern IdDictionary *ObjectGetObservers();
    
    gApplication= this;
    gClassManager= ::gClassManager;
    gWindow= ::gWindow;
    gDebug= ::gDebug;
    gBatch= ::gBatch;
    gScreenRect= ::gScreenRect;
    gObservers= ObjectGetObservers();
    gEnviron= environ;
}

void Application::DoOnError(int level, char *location, char *msg)
{
    int ignorelevel= GetIgnoreLevel();
    static bool inError= FALSE;
    
    if (level < ignorelevel || inError)
	return;
    inError= TRUE;
    
    char *type= "Warning";
    if (level >= cFatal)
	type= "Fatal";
    else if (level >= cSysError)
	type= "SysError";
    else if (level >= cError)
	type= "Error";
    
    cerr.form("%s: %s in <%s>: %s\n", ProgramName(), type, location, msg);
    if (level >= cError) {
	switch (ShowAlert(eAlertError, "%s: %s\nin @B%s@B: %s", ProgramName(), type, location, msg)) {
	case cIdIgnore:
	    break;
	    
	case cIdAbort:
	    Abort();
	    
	case cIdInspect:
	    Inspect();
	    break;
	}
    }
    inError= FALSE;
}

