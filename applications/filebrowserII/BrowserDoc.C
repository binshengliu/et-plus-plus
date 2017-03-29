//$BrowserDocument$
#include "ET++.h"
#include "BrowserView.h"
#include "BrowserItems.h"
#include "BrowserDoc.h"
#include "Preferences_e.h"

#include "GotoDialog.h"
#include "BrowserCmdNo.h"
#include "StyledText.h"
#include "ShellTView.h"
#include "FileDialog.h"
#include "Icon.h"
#include "Alert.h"

#include "ExtScroller.h"
#include "SharedDocObjects.h"

#include "System.h"
#include "WindowSystem.h"

static u_short CautionBits[]= {
#   include "images/caution.image"
};

const int cNumFilelists= 3;

static char *errFormats[]= {
	/* cc */            "\"%[^\"]\", line%d",
	/* grep -n, cpp*/   "%[^:]:%d:",
	0
};

static Alert *sharedAlert= 0;

//---- BrowserDocument --------------------------------------------------

MetaImpl(BrowserDocument, (TP(browserView), TP(textView), TP(text), 
			  T(nFileLists), TP(shell), TP(shWin), T(isCCode), 
			  TP(shText), TP(shScroller), TP(functionButton), 0));

BrowserDocument::BrowserDocument(int n) : Document(cDocTypeAscii)
{   
    // my external type is cDocTypeAscii, e.g. ordinary ascii files  
    if (n == -1) 
	n= Preferences::FileLists();
    nFileLists= range(1, 6, n);
    shell= 0;
    shWin= 0;
    shText= 0;
    shScroller= 0;
    isCCode= FALSE;
    icon= 0;
    iconText= 0;
    functionButton= 0;
    
    text= MakeText();
    EnableBackups(Preferences::MakeBak());
}

BrowserDocument::~BrowserDocument()
{
    SafeDelete(textView);
    SharedDocObjects::Delete(text, this);        
    SafeDelete(shText);
}

Window *BrowserDocument::DoMakeWindows()
{
    browserView= new BrowserView(this, nFileLists);
    textView= new FileBrowserTextView(this, Rectangle(Point(2000,cFit)), text);
   
    VObject *scroller= new ExtScroller(textView->GetFunctionMenu(), textView, Point(500, 350));
    functionButton= scroller->FindItem(cIdFuncButton);
    functionButton->Disable();
    
    return
	new Window(this, gPoint0, eWinDefault, 
	    new Expander(cIdNone, eVert, gPoint2,
		new BorderItem(browserView, gPoint8, 1, eVObjHCenter),
		scroller,
	    0),
	    "FileBrowser"
	);
}

Icon *BrowserDocument::DoMakeIcon(char *)
{
    iconText= new TextItem(cIdIconLabel, "", new_Font (eFontGeneva, 10));
    icon= new Icon(this, new BorderItem (iconText, gPoint0, 2));
    return icon;
}

void BrowserDocument::DoWrite(ostream &s, int)
{
    text->PrintOnAsPureText(s);
}

void BrowserDocument::DoRead(istream &s, FileType *ft)
{
    Document *dp= IsAlreadyOpen(ft);
    if (dp && UniqueId() == ft->UniqueId()) { //---- revert command
	text->ReadFromAsPureText(s, ft->SizeHint());
	ShowReverted(text, ft);
	return;
    } 
    SharedDocObjects::DeleteDelayed(text, this);
    if (!dp) {        //---- new unshared copy
	text= MakeText();
	text->ReadFromAsPureText(s, ft->SizeHint());
    } else {          //---- shared copy
	TextView *tv= Guard(dp, BrowserDocument)->GetTextView();
	text= tv->GetText();
	SharedDocObjects::AddRef(text, this);
	SetSharedWindowTitle();
    }
    InstallText(text, ft->IsCCode());
}

void BrowserDocument::ShowReverted(Text *t, FileType *ft)
{
    Iter next(SharedDocObjects::MakeDocumentIter(t));
    BrowserDocument *dp;
    while (dp= (BrowserDocument*)next()) 
	dp->InstallText(t, ft->IsCCode());
}

void BrowserDocument::InstallText(Text *t, bool isC)
{
    textView->SetText(t);
    
    if (isCCode= isC)
	textView->FormatCode();
    else {
	textView->SetDefaultStyle();
	textView->ResetFunctionMenu();
    }
    textView->SetSelection(0, 0);
    textView->RevealSelection();
    Menu *m= textView->GetFunctionMenu();
    functionButton->Enable(isCCode && (m->GetCollection()->Size() > 1));
}

bool BrowserDocument::CanImportDocument(FileType *ft)
{
    if (ft == 0)
	return TRUE;
    return strismember(ft->Type(), cDocTypeAscii, 0);
}

Command *BrowserDocument::DoImport(istream& s, FileType *ft)
{
    GapText *t = new GapText(1024);
    Command *cmd;
    
    t->ReadFromAsPureText(s, ft->SizeHint());
    cmd= textView->InsertText(t);
    SafeDelete(t);
    return cmd;
}

bool BrowserDocument::DoFileIsAlreadyOpen(Document *shown, char *name)
{
    if (shown->UniqueId() == UniqueId())
	return FALSE;
    
    if (sharedAlert == 0)
	    sharedAlert= new Alert(eAlertCaution, (byte*) "Caution", 
			new Bitmap(Point(64, 52), CautionBits),
					"Open",    cIdYes,
					"Show",    cIdNo,
					"Cancel", cIdCancel, 0);
    switch(sharedAlert->Show("File @B%s@P is already open?", BaseName(name))) {
    case cIdYes:
	return TRUE;
    case cIdNo:
	ShowDocument(shown);
	return FALSE;
    case cIdCancel:
	return FALSE;
    }
}

bool BrowserDocument::AlertChanges()
{
    if (SharedDocObjects::RefCount(text) > 1)
	return FALSE;
    return Modified();
}

void BrowserDocument::SetChangeCount(int delta)
{
    if (SharedDocObjects::RefCount(text) == 1)
	Document::SetChangeCount(delta);
    else {
	Iter next(SharedDocObjects::MakeDocumentIter(text));
	Document *dp;
	while (dp= (Document*)next())
	    dp->Document::SetChangeCount(delta);
    }
}

void BrowserDocument::InitChangeCount()
{
    if (SharedDocObjects::RefCount(text) == 1)
	return Document::InitChangeCount();

    Iter next(SharedDocObjects::MakeDocumentIter(text));
    Document *dp;
    while (dp= (Document*)next())
	if (dp != this)
	    Document::SetChangeCount(dp->GetChangeCount());
}

void BrowserDocument::SetSharedWindowTitle()
{
    SetWindowTitle(form("%s (%d)", 
			   BaseName(GetName()), SharedDocObjects::RefId(text, this)+1));  
}

Text *BrowserDocument::MakeText()
{
    Text *tp;
    Font *fd= new_Font(gFixedFont->Fid(), Preferences::FontSize(), eFacePlain);
    if (Preferences::UseStyledText())
	tp= new StyledText(256, fd);
    else
	tp= new GapText(256, fd);
    tp->SetTabWidth(fd->Width(' ')*Preferences::TabWidth());
    SharedDocObjects::AddRef(tp, this);
    return tp;
}

Document *BrowserDocument::IsAlreadyOpen(FileType *ft)
{
    Document *dp= application->FindDocument(ft->UniqueId());
    if (dp == 0 || !dp->IsKindOf(BrowserDocument)) 
	return 0;
    return dp;
}

TextView *BrowserDocument::GetTextView()
{ 
    return textView; 
}
    
void BrowserDocument::Control(int id, int detail, void *data)
{
    //--- update tty size
    if (detail == cPartExtentChanged && data == (void*) shScroller) {
	shell->SetTtySize(shScroller->ContentRect(), gFixedFont);
	return;
    }  
    switch (id) {
    case cIdLoadFile:
	AbsoluteFileItem *fi= (AbsoluteFileItem*)data;
	LoadFile(fi->AbsoluteName(), fi->GetType()); 
	break;
	
    case cIdShowShell:
	ShowShell((char*)data);
	break;
	
    case cIdShowErr:
	ShowError((char*)data);
	break;
	
    case cIdShellCmd:
	ShellExecute((char*)data);
	break;
	
    case cIdFuncButton:
	if (detail == cPartSetupFunc)
	    textView->SetupFunctionMenu();
	else
	    textView->GotoFunction(detail-cPartGotoFunc);
	break;
	
    default:
	Document::Control(id, detail, data);
    }
}

void BrowserDocument::ShowError(char *errMsg)
{
    char file[400];
    int line;
    
    for (int i= 0; errFormats[i]; i++) {
	if (sscanf(errMsg, errFormats[i], file, &line) == 2) {
	    FType ft(file); 
	    if (UniqueId() != ft.UniqueId()) {     // file not already loaded ?
		if(gSystem->AccessPathName(file, 4) != 0) {
		    ShowAlert(eAlertNote, "Can't find @B%s@P in\n%s", file, 
						    gSystem->WorkingDirectory());
		    return;
		}
		if (!SavedChanges())
		    return;
		Load (file, TRUE, ft.FileType());
	    }
	    Mark *lm= textView->MarkAtLine(line-1);
	    textView->SetSelection(lm->Pos(),lm->End());
	    textView->RevealSelection();
	    GraphicDelay(500);
	    textView->SetSelection(lm->Pos(),lm->Pos());
	    return;
	}
    }
    ShowAlert(eAlertNote, "Error message no understood!");
}

void BrowserDocument::LoadFile(char *name, FileType *ft)
{
    if (!SavedChanges())
	return;
    else
	Load(name, TRUE, ft); 
}

bool BrowserDocument::SavedChanges()
{
    if (AlertChanges()) {
	switch (ShowAlert(eAlertCaution, "Save changes to @B%s@P ?",
							BaseName(GetName()))) {
	    case cIdYes:
		Save();
		break;

	    case cIdCancel:
		return FALSE;
	}
    }
    return TRUE;
}

void BrowserDocument::ShellExecute(char *cmd)
{
    if (shell)
	shell->Submit(cmd, strlen(cmd));
    shell->SetSelection(shText->Size(), shText->Size());
}

void BrowserDocument::ApplyPreferences()
{
    Font *nfp, *ofp= text->GetFont();
    nfp= new_Font(ofp->Fid(), Preferences::FontSize(), eFacePlain);
    int ow, w= nfp->Width(' ')*Preferences::TabWidth();
    int from, to;
    textView->GetSelection(&from, &to);
    
    ow= text->SetTabWidth(w);
    EnableBackups(Preferences::MakeBak());
    textView->SetAutoIndent(Preferences::AutoIndent() != 0);
    if (w != ow) 
	textView->ForceRedraw();
    if (nfp != ofp) {
	textView->SetFont(nfp);
	textView->FormatCode();
    }
    if (w != ow || nfp != ofp)
	textView->SetSelection(from, to);
}

void BrowserDocument::ExecuteMake()
{
    if (loadDir == 0)
	ShowShell(gSystem->WorkingDirectory());
    else {
	ShowShell(loadDir); 
	gSystem->ChangeDirectory(loadDir);
    }
    ShellExecute(form("%s\n", Preferences::MakeCmd()));
}

void BrowserDocument::ShowShell(char *data)
{
    static char *argv[] = { "/bin/csh", 0 };

    if (shWin != 0)
	shWin->Open();
    else {
	char *cmd= Getenv("SHELL");
	if (cmd)
	    argv[0]= cmd;
	Font *fd= gFixedFont;
	if (Getenv("ET_NO_STYLEDCODE"))
	    shText= new GapText(256, fd);
	else
	    shText= new StyledText(256, fd);
	shell= new BrowserShellTView(this, Rectangle(Point(1000,cFit)), shText, 
							       argv[0], argv);
	shScroller= new Scroller(shell);
	shScroller->SetFlag(eVObjLayoutCntl);
	shWin= new Window(0, Point(550, 240), eWinCanClose,
				shScroller,
				form("%s", shell->GetArgv()[0])
		    );
	shWin->OpenAt(gApplication->GetNewDocumentPos());
	AddWindow(shWin);
    }
    ShellExecute(form("cd %s\n", data));
    gSystem->ChangeDirectory(data);
}

//---- Menus --------------------------------------------------------------

void BrowserDocument::DoCreateMenu(Menu *menu)
{
    Document::DoCreateMenu(menu);
    
    Menu *file= menu->FindMenuItem(cFILEMENU);
    Menu *util= new Menu("utilities");
    util->AppendItems(
		 "go to line...",           cGOTOLINE,
		 "make >",                  cMAKE,
		 "reformat",                cREFORMAT,
		 "preferences...",          cPREFERENCES,
		0);
    menu->AppendMenu(util, cUTILMENU);
}

void BrowserDocument::DoSetupMenu(Menu *menu)
{
    if (isCCode && text->IsKindOf(StyledText))
	menu->EnableItem(cREFORMAT);
    menu->EnableItems(cGOTOLINE, cMAKE, cPREFERENCES, 0);
    Document::DoSetupMenu(menu);
}

Command *BrowserDocument::DoMenuCommand(int cmd)
{
    switch (cmd) {

    case cGOTOLINE:
	GotoLine(textView);
	break;
	
    case cMAKE:
	ExecuteMake();
	break;
	
    case cREFORMAT:
	textView->FormatCode();
	textView->ForceRedraw();
	return gNoChanges;

    case cPREFERENCES:
	if (Preferences::ShowDialog(textView) == cIdOk)
	    ApplyPreferences();
	return gNoChanges;
	
    default:
	break;
    }
    return Document::DoMenuCommand(cmd);
}
