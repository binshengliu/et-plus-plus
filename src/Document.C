#include "ET++.h"

#include "FileDialog.h"
#include "System.h"
#include "Icon.h"
#include "ObjList.h"
#include "ObjectTable.h"
#include "ClassManager.h"

static FileDialog *loadDialog, *saveDialog, *importDialog;

static u_short FileBits[]= {
#   include "images/file.im"
};

//---- Document ----------------------------------------------------------------

AbstractMetaImpl(Document, (TP(application), T(changeCount), T(uniqueId),
	TP(docName), TP(docType), TP(lastCmd), TP(menu), TB(isUntitled), 
	TB(isOpen), TB(isConverted), TP(loadDir), TP(window), TP(icon),
	TP(windows), TB(makeBackup), 0));


Document::Document(const char *dt)
{
    changeCount= 0;
    lastCmd= gNoChanges;
    menu= new Menu("edit");
    icon= 0;
    window= 0;
    windows= new ObjList;
    isOpen= TRUE;
    application= 0;
    docName= 0;
    loadDir= 0;
    docType= dt;
    isUntitled= TRUE;
    isConverted= FALSE;
    makeBackup= FALSE;
    uniqueId= 0;
}

Document::~Document()
{
    if (lastCmd && lastCmd != gResetUndo)
	Error("~Document", "lastCmd != gResetUndo");
    SafeDelete(lastCmd);
    SafeDelete(menu);
    SafeDelete(docName);
    SafeDelete(loadDir);
    SafeDelete(icon);
    SafeDelete(windows);
    SafeDelete(window);
}

FileDialog *Document::MakeFileDialog(FileDialogType)
{
    return application->MakeFileDialog();
}

void Document::SetName(char *name)
{
    if ((docName == 0) || (strcmp(name, docName) != 0)) {
	strreplace(&docName, name);
	if (window)
	    SetWindowTitle(docName);
    }
}

void Document::SetWindowTitle(char *docName)
{
    window->SetTitle(docName);
}

EvtHandler *Document::GetNextHandler()
{
    return application;
}

void Document::SetApplication(Application *app)
{
    application= app;
}

void Document::OpenWindows()
{
    Point lastDocPos= application->GetNewDocumentPos();
    if (window= DoMakeWindows())
	windows->AddFirst(window);
    if (window= Guard(windows->First(), Window)) {
	DoMakeViews();
	char *name= window->GetTitle();
	if (name == 0 || *name == '\0')
	    window->SetTitle(docName, FALSE);
	window->OpenAt(lastDocPos);
    }
}

Window *Document::DoMakeWindows()
{
    return 0;
}

void Document::AddWindow(BlankWin *win)
{
    windows->Add(win);
}

void Document::DoMakeViews()
{
    // Warning("DoMakeViews", "method is obsolete, use DoMakeWindows!!"
}

void Document::Toggle()
{
    if (window) {
	BlankWin *win;
	Iter next(windows);
	bool isopen= TRUE;
	
	if (icon == 0) {
	    icon= DoMakeIcon(GetName());
	    icon->OpenAt(window->GetRect().origin);
	} else
	    icon->Open(isopen= !icon->IsOpen());

	for (int i= 0; win= (BlankWin*) next(); i++) {
	    if (isopen) {
		win->SetFlag(eBWinWasOpen, win->IsOpen());
		win->Close();
	    } else {
		if (win->TestFlag(eBWinWasOpen))
		    win->Open();
	    }
	}
	if (isopen)
	    icon->UpdateIconLabel(GetName());
    }
}

Icon *Document::DoMakeIcon(char *name)
{
    if (gFileIcon == 0)
	gFileIcon= new Bitmap(16, FileBits);
    return
	new Icon(this,
	    new BorderItem(
		new Cluster(cIdNone, eVObjHCenter, gPoint2,
		    new ImageItem(gFileIcon, 13),
		    new TextItem(cIdIconLabel, name, new_Font(eFontTimes, 9)),
		    0
		), gPoint2, 2, eVObjHCenter));
}

//---- Menus --------------------------------------------------------------------

Menu *Document::GetMenu()
{
    return menu;
}

void Document::DoCreateMenu(Menu *menu)
{
    Menu *file= new Menu("file");

    EvtHandler::DoCreateMenu(menu);

    file->AppendItems(
		     "load ...",                 cLOAD,
		     "-",
		     "save",                     cSAVE,
		     "save as ...",              cSAVEAS,
		     "revert",                   cREVERT,
		     "close",                    cCLOSE,
		     "-",
		     "application window",       cSHOWAPPLWIN,
		     0);
    
    if (CanImportDocument(0))
	file->InsertItemAfter(cLOAD, "import ...", cIMPORT);
	
    menu->AppendItems("undo",   cUNDO,
		      "-",
		      "cut",    cCUT,
		      "copy",   cCOPY,
		      "paste",  cPASTE,
		      "-",
		     0);

    menu->AppendMenu(file, cFILEMENU);
}

void Document::DoSetupMenu(Menu *menu)
{
    EvtHandler::DoSetupMenu(menu);
    if (lastCmd != gNoChanges) {
	menu->ReplaceItem(cUNDO, lastCmd->GetUndoName());
	if (lastCmd->TestFlag(eCmdCanUndo))
	    menu->EnableItem(cUNDO);
    }
    if (Modified()) {
	menu->EnableItem(cSAVE);
	if (!isUntitled)
	    menu->EnableItem(cREVERT);
    }
    menu->EnableItems(cCLOSE, cLOAD, cSAVEAS, cSHOWAPPLWIN, cFILEMENU, cIMPORT, 0);
}

Command *Document::DoMenuCommand(int cmd)
{
    switch(cmd) {
    case cUNDO:
	Undo();
	break;

    case cREDO:
	Redo();
	break;

    case cSAVE:
	Save();
	break;

    case cSAVEAS:
	SaveAs();
	break;

    case cLOAD:
	Open();
	break;

    case cIMPORT:
	return Import();
	
    case cCLOSE:
	Close();
	break;

    case cREVERT:
	Revert();
	break;

    case cCOLLAPSE:
	Toggle();
	break;
	
    default:
	return EvtHandler::DoMenuCommand(cmd);
    }
    if (window)
	window->UpdateEvent();
    return gNoChanges;
}
 
void Document::Control(int id, int part, void *vp)
{
    if (id == cIdWinDestroyed) 
	windows->RemovePtr((Object*)vp);
    else if (id == cIdCloseBox) 
	Toggle();
    EvtHandler::Control(id, part, vp);
}

void Document::PerformCommand(Command* cmd)
{
    // don't do anything on gNoChanges!!
    if (cmd && (cmd != gNoChanges) && (cmd->GetId() || cmd == gResetUndo)) {
	if (lastCmd)
	    lastCmd->Finish(cmd);
	if (cmd != gResetUndo)
	    SetChangeCount(changeCount+= cmd->Do());
	if (lastCmd)
	    lastCmd= cmd;
	else
	    cmd->Finish(0);
    }
    if (window)
	window->UpdateEvent();
}

void Document::Undo()
{
    if (lastCmd)
	SetChangeCount(changeCount+= lastCmd->Undo());
}

void Document::Redo()
{
}

bool Document::Modified()
{
    return changeCount > 0; 
}

bool Document::AlertChanges()
{
    return Modified();
}

void Document::SetChangeCount(int c)
{
    changeCount= c;
}

void Document::InitChangeCount()
{
    changeCount= 0;
}

//---- Menu Commands -----------------------------------------------------------

bool Document::Open()
{
    if (AlertChanges()) {
	switch(ShowAlert(eAlertCaution, "Save changes to @B%s@P ?", 
							   BaseName(docName))) {
	case cIdYes:
	    if (! Save())
		return FALSE;
	    break;
	case cIdNo:
	    break;
	case cIdCancel:
	    return FALSE;
	}
    }

    PerformCommand(gResetUndo);

    if (loadDialog == 0)
	ObjectTable::AddRoot(loadDialog= MakeFileDialog(eFDTypeRead));

    if (loadDialog->ShowInWindow(eFDRead, window, this) == cIdOk) {
	char *filename= loadDialog->FileName();
	FileType *ft= loadDialog->GetDocType();
	if (CanLoadDocument(ft))
	    Load(filename, TRUE, ft);
	else
	    application->OpenDocument(filename);
	return TRUE;
    }
    return FALSE;
}

bool Document::Close()   // return TRUE if OK
{
    bool closedoc= TRUE;

    if (! isOpen)
	return closedoc;

    if (AlertChanges()) {
	switch (ShowAlert(eAlertCaution, "Save changes to @B%s@P ?", 
							    BaseName(docName))) {
	case cIdNo:
	    closedoc= TRUE;
	    break;
	case cIdYes:
	    closedoc= Save();
	    break;
	case cIdCancel:
	    closedoc= FALSE;
	    break;
	}
    }

    if (closedoc) {
	PerformCommand(gResetUndo);
	windows->ForEach(BlankWin,Open)(FALSE);
	if (application)
	    application->RemoveDocument(this);
	isOpen= FALSE;
    }
    return closedoc;
}

bool Document::Save()
{
    if (AlertChanges() && (isUntitled || isConverted))
	return SaveAs();

    if (! Modified()) {
	ShowAlert(eAlertNote, "No changes since last save");
	return TRUE;
    }
    MakeBackup(loadDir, docName);
    char *name= form("%s/%s", loadDir, docName);
    PerformCommand(gResetUndo);     //  Added by SMM, Kewill, 28/2/90 ???
    if (docName[0] == '/') 
	Store(docName, 0);
    else 
	Store(name, 0);
	
    // update id
    uniqueId= FType(name).UniqueId();
    
    return TRUE;
}
    
void Document::MakeBackup(char *loadDir, char *docName)
{
    if (makeBackup) {
	char *name, backup[1000];
	strcpy(backup, form("%s/%s%%", loadDir, docName)); 
	name= form("%s/%s", loadDir, docName);
	gSystem->Rename(name, backup);
    }
}

void Document::EnableBackups(bool onOff)
{
    makeBackup= onOff;
}

bool Document::SaveAs()
{
    if (saveDialog == 0)
	ObjectTable::AddRoot(saveDialog= MakeFileDialog(eFDTypeWrite));

    if (saveDialog->ShowInWindow(eFDWrite, window, this) == cIdOk) {
	PerformCommand(gResetUndo);

	char *filename= saveDialog->FileName();
	SetName(filename);
	Store(filename, saveDialog->GetSaveOption());
	FType ft(filename);
	uniqueId= ft.UniqueId();
	isUntitled= isConverted= FALSE;
	strreplace(&loadDir, gSystem->WorkingDirectory());
	return TRUE;
    }
    return FALSE;
}

void Document::Revert()
{
    if (Modified() &&
	ShowAlert(eAlertCaution, "Discard all changes of @B%s@P ?", 
						  BaseName(docName)) == cIdYes){
	    FType ft(docName);
	    Load(docName, FALSE, ft.FileType());
    }            
}

Command *Document::Import()
{
    Command *cmd= gNoChanges;
	
    if (importDialog == 0)
	ObjectTable::AddRoot(importDialog= MakeFileDialog(eFDTypeImport));

    if (importDialog->ShowInWindow(eFDImport, window, this) == cIdOk) {
	char *filename= importDialog->FileName();
	FileType *ft= importDialog->GetDocType();
	if (CanImportDocument(ft)) {
	    istream from(filename);
	    gClassManager->Reset();
	    cmd= DoImport(from, ft);
	    gClassManager->Reset();
	}
    }
    return cmd;
}

//---- Load/Store Documents -----------------------------------------------------

bool Document::CanLoadDocument(FileType *file)
{
    bool ok= strcmp(GetDocumentType(), file->Type()) == 0;
    if (!ok)
	if (strcmp(GetDocumentType(), cDocTypeAscii) == 0 && file->IsAscii())
	    ok= TRUE;
    return ok;
}

bool Document::CanImportDocument(FileType *)
{
    return FALSE;
}

bool Document::Load(char *name, bool unique, FileType *filetype)
{
    bool rcode= TRUE, asuntitled= FALSE;
    int uid;
    char *newname;
    Document *doc;
	    
    if ((uid= filetype->UniqueId()) == 0) {
	ShowAlert(eAlertNote, "something wrong with @B%s@P\n%s", name,
						    gSystem->GetErrorStr());
	return FALSE;
    }

    if (unique && (doc= application->FindDocument(uid))) {
	if (DoFileIsAlreadyOpen(doc, name))
	    newname= name;   // show document once more
	else
	    return FALSE;   
    } else
	newname= name;

    istream from(newname);
    SetName(newname);

    gClassManager->Reset();
    DoRead(from, filetype);
    if (gClassManager->Reset() == 0) {
	InitChangeCount();
	isUntitled= asuntitled;
	if ((strcmp(docType, cDocTypeAscii) == 0) && filetype->IsAscii())
	    isConverted= FALSE;
	else
	    isConverted= strcmp(filetype->Type(), docType) != 0;
	if (lastCmd && lastCmd != gNoChanges)
	    delete lastCmd;
	lastCmd= gNoChanges;
    }
    strreplace(&loadDir, gSystem->WorkingDirectory());
    uniqueId= uid;
    return TRUE;
}

bool Document::DoFileIsAlreadyOpen(Document *shown, char *name)
{
    ShowAlert(eAlertNote, "Document @B%s@P is already open", name);
    ShowDocument(shown);
    return FALSE;
}

void Document::ShowDocument(Document *shown)
{
    Window *w= 0;
    if (shown != this) {
	if (w= shown->GetWindow())
	    if (!w->IsOpen())
		shown->Toggle();
	    else
		w->Open();
    }   
}

void Document::Store(char *name, int option)
{
    ostream to(name);

    gClassManager->Reset();
    gInPrintOn= TRUE;
    DoWrite(to, option);
    gInPrintOn= FALSE;
    gClassManager->Reset();

    SetChangeCount(0);
}

void Document::DoRead(istream& from, FileType *)
{
    char c;
    //overread magic cookie
    while (from.get(c))
	if (c == '\n')
	    break;
}

void Document::DoWrite(ostream& to, int)
{
    to << cMagic << docType SP << application->ProgramName() NL;
}

Command *Document::DoImport(istream& , FileType *)
{
    return gNoChanges;
}

void Document::InspectorId(char *buf, int bufSize)
{
    if (docName)
	strn0cpy(buf, docName, bufSize);
    else
	EvtHandler::InspectorId(buf, bufSize);        
}

void Document::Parts(Collection* col)
{
    EvtHandler::Parts(col);
    col->AddVector(windows, menu, 0);
}
