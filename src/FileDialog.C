//$FileDialog, FileItem$

#include <stdio.h>

#include "ET++.h"
 
#include "FileDialog.h"
#include "PopupItem.h"
#include "EditTextItem.h"
#include "BorderItems.h"
#include "Buttons.h"
#include "ScrollBar.h"
#include "CheapText.h"
#include "System.h"
#include "CollView.h"
#include "OrdColl.h"
#include "Directory.h"

char *cPathSeparator= "/",
     *cPathRoot     = "/";
	  
const int cPathBuf      =   200,
	  cItemMinWidth =   250,
	  cNumItems     =   7;

const Point cIconSize(16);

//---- Path -----------------------------------------------------------------

Path::Path()
{
    path= 0;
    Reset();
}

Path::~Path()
{
    SafeDelete(path);
}

const char *Path::GetPath()
{
    return path;
}

void Path::Reset()
{
    if (gSystem)
	strreplace(&path, gSystem->WorkingDirectory());
    p= path;
}

void Path::Start()
{
    p= path;
}

char *Path::operator()()
{  
    char buf[300];
     
    if (*p == 0) {
	p= path;
	return 0;
    }
    p++;
    char *c= buf;
    while (*p && *p != '/')
	*c++= *p++;
    *c= '\0';
    return form("%s", buf);
}

int Path::Components()
{
    char *cp= path+1;
    int n= 1;
    if (*cp == 0)
	return n;
    while (*cp) 
	if (*cp++ == '/')
	    n++;
    return n+1;
}

char *Path::At(int at)
{
    char buf[1000];
    
    int i= 0;
    char *c= path+1, res= 0;
    
    for (; *c && i < at; c++)
	if (*c == '/')
	    i++;
    strncpy(buf, path, c - path);
    buf[c - path]= '\0';
    return form("%s", buf);
}

//---- FileDialog --------------------------------------------------------------

MetaImpl(FileDialog, (TP(scroller), TP(eti), TP(title), TP(pathname),
				    TE(flags), TP(collview), TP(fileList), 0));

FileDialog::FileDialog(char *ti) : Dialog(ti, eBWinBlock)
{
    title= new TextItem(ti);
    doc= 0;
    fileList= 0;
    doctype= 0;
    initDir= 0;
    pathname= new char[cPathBuf];
}

FileDialog::~FileDialog()
{
    SafeDelete(pathname);
    SafeDelete(doctype);
    SafeDelete(initDir);
}

int FileDialog::ShowInWindow(FileDialogFlags f, Clipper *fp, EvtHandler *eh, char *p)
{
    flags= f;
    doc= eh;

    if (p == 0) {
	if (f == eFDRead)
	    p= "Open File Named:";
	else if (f == eFDWrite)
	    p= "Save in File Named:";
	else
	    p= "Import File Named:";
    }
    
    title->SetString(p);
    // check whether working directory is still correct
    strreplace(&initDir, gSystem->WorkingDirectory());
    if (fileList && strcmp(initDir, path.GetPath()) != 0) {
	path.Reset();
	UpdatePath();
	UpdateList();
    }
    return Dialog::ShowOnWindow(fp);
}

void FileDialog::UpdateList()
{
    Directory *dir= gSystem->MakeDirectory(".");
    char *name;
    VObject *vop;
    int i;
    
    fileList= new OrdCollection;
    
    for (i= 0; name= (*dir)(); i++)
	if (strcmp(name, ".") && (vop= MakeFileItem(name)))
		fileList->Add(vop);
    
    fileList->Sort();

    if (collview == 0) {
	collview= new CollectionView(this, fileList, eCVDontStuckToBorder);
	collview->SetMinExtent(Point(cItemMinWidth, 0));
	collview->SetId(cIdList);
	collview->SetContainer(this);
    } else
	collview->SetCollection(fileList);
    if (eti)
	eti->SetString((byte*)"");
}

VObject *FileDialog::MakeFileItem(char *name)
{
    return new FileItem(name);
}

VObject *FileDialog::Hook(FileDialogFlags)
{
    return 0;
}

int FileDialog::GetSaveOption()
{
    return 0;
}

int FileDialog::ItemHeight()
{
    VObject *vop= (VObject*)fileList->At(0);
    if (vop)
	return vop->GetMinSize().extent.y;
    return 16;
}

VObject *FileDialog::DoCreateDialog()
{
    pathMenu= new Menu("", FALSE, 1, 0, FALSE);
    pathMenu->SetFlag(eMenuNoScroll);
    pathPopup= new PopupItem(cIdPath, path.Components(), "CWD:", pathMenu);

    UpdatePath();
    UpdateList();
    
    VObject *actions=
	new Expander(cIdNone, eHor, 20, 
	    new ActionButton(cIdOk, "Ok", TRUE),
	    new ActionButton (cIdCancel, "Cancel"),
	    new ActionButton(cIdUpdate, "Update"),
	    Hook(eFDRead),
	    0
	);
    
    VObject *name=
	new Expander(cIdNone, eVert, 2,
	    title,
	    new BorderItem(eti= new EditTextItem(cIdName)),
	    0
	);
    
	
    scroller= new Scroller(collview, Point(cItemMinWidth, ItemHeight()*cNumItems), cIdList);

    // overall layout
    return
	new BorderItem(
	    new Expander(cIdNone, eVert, 20,
		pathPopup,
		scroller,
		name,
		actions,
		0
	    ),
	    20, 0
	);
}

void FileDialog::DoSetup()
{
    EnableItem(cIdOk, eti->GetTextSize() > 0);
    pathPopup->Enable(path.Components() > 1, TRUE);
}

Command *FileDialog::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    if (t.IsKey() || t.IsCursorKey()) {
	Command *cmd= scroller->Input(lp, t, vf);
	if (cmd)
	    return cmd;
    }
    return Dialog::DispatchEvents(lp, t, vf);
}

void FileDialog::Control(int id, int p, void *v)
{
    VObject *gop;
    
    switch (id) {
    
    case cIdUpdate:
	UpdateList();
	return;
    
    case cIdList:
	switch (p) {
	case cPartCollSelect:
	    gop= (VObject*) collview->GetCollection()->At( (int) v );
	    char *fname= gop->AsString();
	    
	    if (fname && strlen(fname) > 0) {
		eti->SetString((byte*)gop->AsString());
		eti->SetSelection();
		DoSetup();
	    }
	    break;
	case cPartCollDoubleSelect:
	    if (OpenOrChangeDir()) 
		Dialog::Control(cIdOk, cPartAction, v);
	    break;
	}
	return;
	
    case cIdName:
	if (p == cPartChangedText && v == eti)
	    DoSetup();
	break;
	
    case cIdOk:
	if (!OpenOrChangeDir())
	    return;
	break;
	
    case cIdCancel:
	gSystem->ChangeDirectory(initDir); 
	break;
    
    case cIdPath:
	eti->SetString((byte*)path.At(p-cIdComponent));
	eti->SetSelection();
	OpenOrChangeDir();
	return;

    default:
	break;
    }
    Dialog::Control(id, p, v);
}

bool FileDialog::OpenOrChangeDir()
{
    Text *t;
    
    t= eti->GetText();
    t->CopyInStr((byte*)pathname, cPathBuf, 0, t->Size());

    if (gSystem->ExpandPathName(pathname, cPathBuf-1)) {
	ShowAlert(eAlertNote, "%s (%s)", gSystem->GetErrorStr(), pathname);
	return FALSE;
    }
    SafeDelete(doctype);
    doctype= gSystem->GetFileType(pathname);
    if (ChangeDirectory())
	return FALSE;
    if (flags == eFDWrite) {
	if (NotWritable(pathname))
	    return FALSE;
    } else {
	if (NotReadable(pathname))
	    return FALSE;
	if (WrongType()) {
	    ShowAlert(eAlertNote, "file @B%s@P has wrong type (%s)", pathname,
							  doctype->Type());
	    return FALSE;
	}
    }
    return TRUE;
}

bool FileDialog::NotReadable(char *name)
{
    if (gSystem->AccessPathName(name, 4)) {
	ShowAlert(eAlertNote, "Can't open document @B%s@P for reading\n%s", name,
							gSystem->GetErrorStr());
	return TRUE;
    }
    return FALSE;
}

bool FileDialog::NotWritable(char *name)
{
    if (gSystem->AccessPathName(name, 0)) {
	char *p= rindex(name, '/'), buf[1000];
	if (!p)
	    strcpy(buf, ".");
	else 
	    strncpy(buf, name, p-name);
	if (gSystem->AccessPathName(buf, 2)) {
	    ShowAlert(eAlertNote, "Document @B%s@P is not writable\n%s", name,
							gSystem->GetErrorStr());
	    return TRUE;
	}
	return FALSE;
    }
    if (ShowAlert(eAlertCaution, "file @B%s@P exists\noverwrite ?", name) != cIdYes)
	return TRUE;
    if (gSystem->AccessPathName(name, 2)) {
	ShowAlert(eAlertNote, "Document @B%s@P is not writable\n%s", name,
							gSystem->GetErrorStr());
	return TRUE;
    }
    return FALSE;
}

bool FileDialog::WrongType()
{
    if (doc) {
	if (doc->IsKindOf(Document)) {
	    if (flags == eFDImport)
		return ! ((Document*)doc)->CanImportDocument(doctype);
	    if (((Document*)doc)->CanLoadDocument(doctype))
		return FALSE;
	}
	return ! gApplication->CanOpenDocument(doctype);
    }
    return TRUE;
}

bool FileDialog::ChangeDirectory()
{
    if (strcmp(doctype->Type(), cDocTypeDirectory) == 0) {
	if (!gSystem->ChangeDirectory(pathname))
	    ShowAlert(eAlertNote, "Cannot change directory to @B%s@P", pathname);
	else {
	    path.Reset();
	    UpdatePath();
	    UpdateList();
	} 
	return TRUE;
    }
    return FALSE;
}

void FileDialog::UpdatePath()
{
    Collection *col= new OrdCollection;
    Font *rootFont= new_Font(gSysFont->Fid(), gSysFont->Size(),
				    (GrFace)(gSysFont->Face() | eFaceBold));
		
    int n= path.Components();
    if (cPathRoot)
	col->Add(new TextItem(cIdComponent+0, cPathRoot, rootFont, gPoint0));
    for (int i= 1; i < n; i++) {
	col->Add(new TextItem(cIdComponent+i, path(), gSysFont, gPoint0));
	TextItem *ti= new TextItem(cIdNone, cPathSeparator, gSysFont, gPoint0);
	ti->Disable();
	col->Add(ti);
    }
    pathMenu->SetCollection(col);
    pathPopup->SetSelectedItem(cIdComponent+n-1);
}

//---- bitmap images for file icons --------------------------------------

static u_short DirectoryBits[]= {
#   include "images/directory.im"
};
static u_short FileBits[]= {
#   include "images/file.im"
};
static u_short ExecBits[]= {
#   include "images/exec.im"
};
static u_short AsciiBits[]= {
#   include "images/ascii.im"
};

Bitmap *gDirectoryIcon, *gFileIcon, *gExecIcon, *gAsciiIcon;

//---- FileItem -----------------------------------------------------------

MetaImpl(FileItem, (TP(label), TP(icon), TB(shallow), 0));

FileItem::FileItem(char *f, bool s) :
		VObjectPair(icon= new ImageItem(
				    gFileIcon ? gFileIcon
					      : (gFileIcon= new Bitmap(16, FileBits)), 13), 
			    label= new TextItem(f, gSysFont, Point(4,0)))
{
    type= 0;
    shallow= s;
}

FileItem::~FileItem()
{ 
    SafeDelete(type); 
}

bool FileItem::IsDirectory()
{ 
    return type && (strcmp(type->Type(), cDocTypeDirectory) == 0); 
}

char *FileItem::LookupName()
{
    return Name();
}

void FileItem::Draw(Rectangle r)
{
    if (! type) {
	type= gSystem->GetFileType(LookupName(), shallow);
	UpdateIcon();
    }
    VObjectPair::Draw(r); 
}

void FileItem::UpdateIcon()
{
    Bitmap *bm= 0;
    
    if (IsDirectory()) {
	if (gDirectoryIcon == 0)
	    gDirectoryIcon= new Bitmap(16, DirectoryBits);
	bm= gDirectoryIcon;
    } else if (type->IsExecutable()) {
	if (gExecIcon == 0)
	    gExecIcon= new Bitmap(16, ExecBits);
	bm= gExecIcon;
    } else if (type->IsCCode()) {
	if (gAsciiIcon == 0)
	    gAsciiIcon= new Bitmap(16, AsciiBits);
	bm= gAsciiIcon;
    }
    if (bm)
	icon->SetBitmap(bm, FALSE);
}

char *FileItem::AsString()
{ 
    return Name(); 
}

//---- OpenFile ----------------------------------------------------------------

FILE *OpenFile(const char *name, const char *rw, const char *va_(cp), ...)
{
    const char *path;
    FILE *fp= 0;
    va_list ap;
    va_start(ap,va_(cp));
    
    for (int i= 0; ; i++) {
	if (i == 0)
	    path= va_(cp);
	else
	    if ((path= va_arg(ap, const char*)) == 0)
		break;
	if (fp= fopen((char*) form("%s/%s", path, name), (char*)rw))
	    break;
    }     
    if (fp == 0) {
	FileDialog *fileDialog= new FileDialog;
	if (fileDialog->ShowInWindow(eFDRead, gWindow, 0,
				    form("Please locate %s", name)) == cIdOk)
	    fp= fopen((char*) fileDialog->FileName(), (char*)rw);
    }

    va_end(ap);
    
    if (fp == 0)
	Fatal("::OpenFile", "can't locate %s", name);
    return fp;
}
