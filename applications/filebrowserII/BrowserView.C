//$BrowserView,BrowserShellTView,FileBrowserPPrinter,FileBrowserTextView$

#include "BrowserView.h"

#include "FileType.h"
#include "Alert_e.h"
#include "DialogItems.h"
#include "System.h"
#include "SortedOList.h"
#include "OrdColl.h"
#include "Directory.h"
#include "Expander.h"
#include "Scroller.h"
#include "BrowserCmdNo.h"
#include "ChangeDirDiag.h"
#include "Menu.h"

//---- BrowserView ------------------------------------------------------------

MetaImpl(BrowserView, (TP(path), TP(directories), TP(fileLists), T(left),
	    TP(changeDir), T(nShown), 0));

BrowserView::BrowserView(EvtHandler *ep, int n) : DialogView(ep)
{ 
    menu= 0;
    nShown= n; 
    left= 0; 
    path= new OrdCollection;
    directories= new OrdCollection;
    changeDir= 0;
}

BrowserView::~BrowserView()
{
    path->FreeAll();
    SafeDelete(path);
    directories->FreeAll();
    SafeDelete(directories);
}

VObject *BrowserView::DoCreateDialog()
{
    fileLists= new Expander(cIdNone, eHor, gPoint4, (Collection*)0);
    
    for (int i= 0; i < nShown; i++) 
	fileLists->Add(new FileList(cIdFirstList+i));

    shiftLeft = new ActionButton(cIdShiftLeft, "<<");
    shiftRight= new ActionButton(cIdShiftRight, ">>");

    // initialize left most file list
    ShowDirectory(-1, "."); 

    return new Expander(cIdNone, eVert, gPoint8,
	fileLists,
	new Expander(cIdNone, eHor, gPoint10,
	    shiftLeft, 
	    new ActionButton (cIdChDir, "change directory..."), 
	    shiftRight, 
	    0
	),
	0
    );
}

void BrowserView::Control(int id, int part, void *)
{
    switch (id) {
    
    case cIdShiftLeft:
	ShiftFileLists(left-1);
	break;
	
    case cIdShiftRight:
	ShiftFileLists(left+1);
	break;
	
    case cIdChDir:              
	if (changeDir == 0)
	    changeDir= new ChangeDirDiag(this);
	changeDir->Show();

    default: 
	if (id >= cIdFirstList && id < cIdFirstList+nShown) {
	    int at= id-cIdFirstList;
	    if (left + at >= path->Size())
		return;
	    switch (part) {
	    case cIdShowShell:
		Shell(at, PathNameAt(left+at));
		break;
	    default:
		LoadFile(at, FileListAt(at));
	    }
	}
    } 
}

void BrowserView::Shell(int, char *path, char *cmd)
{
    View::Control(cIdShowShell, 0, path);
    if (cmd)
	View::Control(cIdShellCmd, 0, form("%s\n", cmd));
}

void BrowserView::LoadFile(int at, FileList *fl) 
{
    AbsoluteFileItem *fi= fl->GetSelectedItem();
    
    //GrShowWaitCursor cw;
    
    if (fi->IsDirectory()) 
	ShowDirectory(at, fi->Name());
	
    else {
	fi->GetType()->DeepenShallowType();
	if (fi->GetType()->IsAscii())
	    // notify next event handler
	    View::Control(cIdLoadFile, 0, fi); 
    } 
}

//---- directory handling --------------------------------------------------

Collection *BrowserView::ReadDirectory(char *path)
{
    Directory *dir= gSystem->MakeDirectory(".");
    char *entry;
    Collection *col= new SortedObjList;
    
    while(entry= (*dir)())
	// do not show "." and ".."
	if (strcmp(entry, ".") != 0 && strcmp(entry, "..")) 
	    col->Add(new AbsoluteFileItem(entry, path));
    return col;    
}

void BrowserView::ShowDirectory(int pane, char *name)
{
    if (pane != -1)
	gSystem->ChangeDirectory(form("%s/%s", PathNameAt(left+pane),name));

    ByteArray *dir= new ByteArray(gSystem->WorkingDirectory());
    Collection *col= ReadDirectory((char*) dir->Str());
    
    // free path to the right of selected pane
    while (path->Size() > left+pane+1) {
	RemoveAndFreeLast(path);
	RemoveAndFreeLast(directories);
    }
    // empty file lists
    for (int i= pane+1; i < nShown; i++)
	FileListAt(i)->Empty();
    
    // shift file lists if at left most one
    if (pane == nShown-1) {
	ShiftFileLists(left+1);
	pane--;
    }
    
    // update path, path names
    path->Add(col);
    directories->Add(dir);
    FileListAt(pane+1)->SetContents(DirectoryNameAt(path->Size()-1),col);
    DoSetup();
}

void BrowserView::ShowParentDirectory()
{
    gSystem->ChangeDirectory(form("%s/..", PathNameAt(left)));
    ByteArray *dir= new ByteArray(gSystem->WorkingDirectory());
    Collection *col= ReadDirectory((char*) dir->Str());

    path->AddFirst(col);
    directories->AddFirst(dir);
}

void BrowserView::ShiftFileLists(int to)
{
    if (to < 0 && left == 0)
	ShowParentDirectory();
    left= range(0, path->Size()-1, to);

    for (int i= 0; i < nShown; i++) {
	FileList *fl= FileListAt(i);
	if (left + i < path->Size())
	    fl->SetContents(DirectoryNameAt(left+i), PathAt(left+i));
	else
	    fl->Empty();
    }
    DoSetup();
}

void BrowserView::DoSetup() 
{
    bool root= path->Size() && strcmp(DirectoryNameAt(left), "/") == 0;
    shiftLeft->Enable(!root, TRUE);
    shiftRight->Enable(left != path->Size()-1, TRUE);
    fileLists->GetList()->ForEach(FileList,SetupIcon)();
}

Menu *BrowserView::GetMenu()
{
    return menu;
}

//---- accessing ------------------------------------------------------------

char *BrowserView::PathNameAt(int i)
{ 
    return (char*) ((ByteArray*)directories->At(i))->Str(); 
}

char *BrowserView::DirectoryNameAt(int i) 
{
    char *p= PathNameAt(i);
    char *name= rindex(p, '/');
    if (name == 0 || strcmp(p, "/") == 0)
	return p;
    return name+1;    
}

void BrowserView::RemoveAndFreeLast(SeqCollection *col)
{
    Object *op= col->RemoveLast();
    if (op) {
	op->FreeAll();
	SafeDelete(op);
    }
}

//---- BrowserShellTView ----------------------------------------------------

MetaImpl(BrowserShellTView, (TP(menu), TP(receiver), 0));

BrowserShellTView::BrowserShellTView(EvtHandler *eh, Rectangle r, 
				    Text *t, char *name, char **args)
					    : ShellTextView(0, r, t, name, args)
{
    menu= new Menu("Typescript");
    receiver= eh;
}

void BrowserShellTView::DoSetupMenu(class Menu *m)
{
    ShellTextView::DoSetupMenu(m);
    m->EnableItem(cSHOWERR);
}

Command *BrowserShellTView::DoMenuCommand(int cmd)
{
    static byte buf[500];
    int start, end;
    
    if (cmd == cSHOWERR) {
	GetSelection(&start, &end);
	Mark *m= MarkAtLine(CharToLine(start));
	GetText()->CopyInStr(buf, sizeof(buf), m->Pos(), m->End());
	receiver->Control(cIdShowErr, 0, buf);
    }
    else
	return ShellTextView::DoMenuCommand(cmd);
    return gNoChanges;
}

void BrowserShellTView::DoCreateMenu(Menu *m)
{
    ShellTextView::DoCreateMenu(m);

    m->AppendItems("doit", cDOIT,
		   "find error", cSHOWERR,
		   0);
		   
    Menu *typescript= new Menu("typescript");

    typescript->AppendItems(
		      "auto reveal ",       cAUTOREVEAL,
		      "reconnect",          cRECONNECT,
		      "clear transcript ",  cCLEAR,
		      0);
    m->AppendMenu(typescript, cTYPESCRIPTMENU);
}

Menu *BrowserShellTView::GetMenu()
{
    return menu;
}

//---- FileBrowserTextView ---------------------------------------------------------

MetaImpl(FileBrowserTextView, (TP(functionMenu), 0));

FileBrowserTextView::FileBrowserTextView(EvtHandler *eh, Rectangle r, Text *t)
					       : CodeTextView(eh, r, t)
{
    functionMenu= new Menu("Functions");
    //functionMenu->SetFlag(eMenuNoScroll);
}
						   
FileBrowserTextView::~FileBrowserTextView()
{
    SafeDelete(functionMenu);
}
    
PrettyPrinter *FileBrowserTextView::MakePrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps)
{
    return new FileBrowserPPrinter(this, t, cs, fs, cds, ps);
}

void FileBrowserTextView::ResetFunctionMenu()
{
    MarkList *mlp= GetMarkList();
    mlp->FreeAll();
}
 
Menu *FileBrowserTextView::GetFunctionMenu()
{
    return functionMenu;
}

void FileBrowserTextView::GotoFunction(int i)
{
    MarkList *mlp= GetMarkList();
    if (i != range(0, mlp->Size(), i))
	return;
    Mark *mp= (Mark*)mlp->At(i);
    if (mp) {
	SetSelection(mp->Pos(), mp->End());
	Rectangle r= SelectionRect();
	r.extent.y+= 200;
	RevealRect(r, r.extent);
    }
}

void FileBrowserTextView::SetupFunctionMenu()
{
    int from, to, at= -1;
    Collection *cp= functionMenu->GetCollection();
    GetSelection(&from, &to);
    Iter next(GetMarkIter());
    
    FunctionMark *fm;
    cp->ForEach(TextItem,SetFont)(gFixedFont);
    for (int i= 0; fm= (FunctionMark*)next(); i++) {
	if (fm->Pos() > from && at == -1)
	    at= i;
	if (fm->State() & (int)eStateDeleted)
	    functionMenu->DisableItem(cPartGotoFunc+i);
    }
    i= range(0, cp->Size()-1, at-1);
    TextItem *tp= Guard(cp->At(i), TextItem);
    tp->SetFont(gFixedFont->WithFace(eFaceBold));
}    

void FileBrowserTextView::UpdateFunctionMenu()
{
    OrdCollection *items= new OrdCollection(60);
    Iter next(GetMarkIter());
    FunctionMark *fm;
    Font *fp;
    VObject *trailer= new TextItem(cIdNone, "---", gFixedFont, Point(2,1));
    trailer->Disable();
    
    for (int i= 0; fm= (FunctionMark*)next(); i++) {
	fp= gFixedFont;
	//fp= gFixedFont->WithFace(eFaceItalic);
	items->Add(new TextItem(cPartGotoFunc+i, fm->GetName(), fp, Point(2,1)));
    }
    items->Add(trailer);
    functionMenu->SetCollection(items);
}

//---- FileBrowserPPrinter ---------------------------------------------------------

FileBrowserPPrinter::FileBrowserPPrinter(FileBrowserTextView *tv, Text *t, Style *cs, 
			Style *fs, Style *cds, Style *ps)
				    : PrettyPrinter(t, cs, fs, cds, ps)
{
    tvp= tv;
    MarkList *mlp= tv->GetMarkList();
    mlp->FreeAll();
}

void FileBrowserPPrinter::End()
{
    PrettyPrinter::End();
    tvp->UpdateFunctionMenu();
}

void FileBrowserPPrinter::Function(int line, int start, int end, char *name, char *classname)
{
    PrettyPrinter::Function(line, start, end, name, classname);

    char *cp;
    if (classname)
	cp= form("%s (%s)", name, classname);
    else
	cp= name;
    tvp->AddMark(new FunctionMark(cp, classname != 0, start, end-start));
}

void FileBrowserPPrinter::ClassDecl(int l, int start, int end, char *name)
{
    PrettyPrinter::ClassDecl(l, start, end, name);
    tvp->AddMark(new FunctionMark(form("class %s", name), FALSE, start, end-start));
}
