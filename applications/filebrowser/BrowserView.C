//$BrowserView$

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

//---- dialog ids -------------------------------------------------------------

const int cIdShiftLeft  =   cIdFirstUser + 0,
	  cIdShiftRight =   cIdFirstUser + 1,
	  cIdFirstList  =   cIdFirstUser + 10;

//---- BrowserView ------------------------------------------------------------

MetaImpl(BrowserView, (TP(path), TP(directories), TP(fileLists), T(left),
	    T(nShown), 0));

BrowserView::BrowserView(EvtHandler *ep, int n) : DialogView(ep)
{ 
    nShown= n; 
    left= 0; 
    path= new OrdCollection;
    directories= new OrdCollection;
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
	    shiftRight, 
	    0
	),
	0
    );
}

void BrowserView::Control(int id, int, void *)
{
    switch (id) {
    
    case cIdShiftLeft:
	ShiftFileLists(left-1);
	break;
	
    case cIdShiftRight:
	ShiftFileLists(left+1);
	break;
	
    default:
	if (id >= cIdFirstList && id < cIdFirstList+nShown) {
	    int at= id-cIdFirstList;
	    FileList *fl= FileListAt(at);
	    AbsoluteFileItem *fi= fl->GetSelectedItem();
	    
	    GrShowWaitCursor cw;
	    
	    if (fi->IsDirectory()) 
		ShowDirectory(at, fi->Name());
		
	    else if (fi->GetType()->IsAscii())
		// notify next event handler
		View::Control(cIdLoadFile, 0, fi);                
	}
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
