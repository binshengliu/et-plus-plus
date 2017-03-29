//$myTreeView,myDocument,TreeApp,PaletteView,myFileDialog$

#include "ET++.h"

#include "OrdColl.h"
#include "Dialog.h"
#include "DialogItems.h"
#include "TreeView.h"
#include "FileDialog.h"
#include "FileType.h"
#include "ProgEnv.h"
#include "ClassManager.h"

static u_short Bits1[]= {
#   include "images/1.im"
};

static u_short Bits2[]= {
#   include "images/2.im"
};

static u_short Bits3[]= {
#   include "images/3.im"
};

static u_short Bits5[]= {
#   include "images/5.im"
};

static u_short Bits6[]= {
#   include "images/6.im"
};

static u_short Bits7[]= {
#   include "images/7.im"
};

static const Point cIconSize1(40, 28);
static char *docTypeTree= "TREE";

//---- myFileDialog ------------------------------------------------------------

static const int cIdSaveOptions= cIdFirstUser + 100;

class myFileDialog: public FileDialog {
    int saveOption;
    bool subtreeonly;
public:
    MetaDef(myFileDialog);
    myFileDialog()
	{ saveOption= 0; subtreeonly= FALSE; }
    VObject *Hook(FileDialogFlags);
    void Control(int id, int, void *v);
    int GetSaveOption();
};

MetaImpl(myFileDialog, (T(saveOption), TB(subtreeonly), 0));

VObject *myFileDialog::Hook(FileDialogFlags fd)
{
    if (fd == eFDRead)
	return new BorderItem ("Options",
		new ManyOfCluster(cIdSaveOptions, eVObjHLeft, 5,
					    "As Ascii", "Subtree only", 0),
		Point(6,4)
	    );
    return FileDialog::Hook(fd);
}

void myFileDialog::Control(int id, int part, void *v)
{
    if (id == cIdSaveOptions) {
	switch(part) {
	case cIdSaveOptions:
	    saveOption= int(v);
	    break;
	case cIdSaveOptions+1:
	    subtreeonly= int(v);
	    break;
	}
    }
    else 
	FileDialog::Control(id, part, v);
}

int myFileDialog::GetSaveOption()
{
    return saveOption;
}

//---- TreeView ----------------------------------------------------------------

class myTreeView: public TreeView {
public:
    MetaDef(myTreeView);
    myTreeView(Document *dp) : TreeView(dp)
	{ }
    Command *NodeSelected(VObject *, int);
};

//---- PaletteView -------------------------------------------------------------

class PaletteView: public DialogView {
    class TreeView *treeView;
    VObject *t1, *t2;
public:
    MetaDef(PaletteView);
    PaletteView(Document *dp, TreeView *t);
    VObject *DoCreateDialog();
    void Control(int id, int part, void *val);
};

//---- PaletteView -------------------------------------------------------------

static const int cIdLayout= cIdFirstUser + 200,
		 cIdConn  = cIdFirstUser + 300;

MetaImpl(PaletteView, (TP(treeView), TP(t1), TP(t2), 0));

PaletteView::PaletteView(Document *dp, TreeView *t) : DialogView(dp)
{
    treeView= t;
}

VObject *PaletteView::DoCreateDialog()
{
    static Bitmap *B1, *B2, *B3, *B5, *B6, *B7;

    if (B1 == 0) {
	B1= new Bitmap(cIconSize1, Bits1);
	B2= new Bitmap(cIconSize1, Bits2);
	B3= new Bitmap(cIconSize1, Bits3);
	B5= new Bitmap(cIconSize1, Bits5);
	B6= new Bitmap(cIconSize1, Bits6);
	B7= new Bitmap(cIconSize1, Bits7);
	
    }
    return new BorderItem(
	new Cluster(cIdNone, eVObjHLeft, Point(10),
	    new BorderItem("Lines",
		new OneOfCluster(cIdNone, eVObjHLeft, Point(4),
		    new LabeledButton(cIdLayout+eTCPerpendicular, 
			new RadioButton, 
			new ImageItem(B2), gPoint10, eVObjVCenter),
		    t1= new LabeledButton(cIdLayout+eTCDiagonal, 
			new RadioButton, 
			new ImageItem(B1), gPoint10, eVObjVCenter),
		    t2= new LabeledButton(cIdLayout+eTCDiagonal2, 
			new RadioButton, 
			new ImageItem(B3), gPoint10, eVObjVCenter),
		    new LabeledButton(cIdLayout+eTCNone, "none", TRUE),
		    0
		)
	    ),
	    new BorderItem("Layout",
		new OneOfCluster(cIdNone, eVObjHLeft, Point(4),
		    new LabeledButton(cIdConn+eTLLeftRight, 
			new RadioButton, 
			new ImageItem(B7), gPoint10, eVObjVCenter),
		    new LabeledButton(cIdConn+eTLIndented,
			new RadioButton,
			new ImageItem(B5), gPoint10, eVObjVCenter),
		    new LabeledButton(cIdConn+eTLTopDown, 
			new RadioButton, 
			new ImageItem(B6), gPoint10, eVObjVCenter),
		    0
		)
	    ),
	    0
	),
	Point(5, 3),
	-1
    );
}

void PaletteView::Control(int, int part, void*)
{
    if (treeView == 0)
	return;
    switch (part) {
    case cIdLayout+eTCNone:
    case cIdLayout+eTCDiagonal:
    case cIdLayout+eTCPerpendicular:
    case cIdLayout+eTCDiagonal2:
	treeView->SetConnType(TreeConnection(part-cIdLayout));
	break;

    case cIdConn+eTLIndented:
	t1->Enable(FALSE);
	t2->Enable(FALSE);
	treeView->SetLayout(TreeLayout(part-cIdConn));
	break;

    case cIdConn+eTLTopDown:
    case cIdConn+eTLLeftRight:
	t1->Enable(TRUE);
	t2->Enable(TRUE);
	treeView->SetLayout(TreeLayout(part-cIdConn));
	break;
    }
}

//---- TreeView -----------------------------------------------------------------

MetaImpl0(myTreeView);

Command *myTreeView::NodeSelected(VObject *t, int)
{
    if (t == 0)
	return gNoChanges;
    if (t == 0 || !t->IsKindOf(Object))
	return gNoChanges;
    char *cp= t->AsString();
    if (cp == 0)
	return gNoChanges;
    Class *clp= gClassManager->Find(cp);
    if (clp)
	clp->Proto()->EditSource(TRUE);
    else
	ShowAlert(eAlertNote, "No Metaclass found for \"%s\" ?", cp);
    return gNoChanges;
}

//---- myDocument ---------------------------------------------------------------

class myDocument : public Document {
    DialogView *dialogView;
    TreeView *treeView;
public:
    MetaDef(myDocument);
    myDocument() : Document(docTypeTree)
	{ }
    ~myDocument();
    Window *DoMakeWindows(); 
    VObject *ReadTree(istream &is, int *level);
    VObject *BuildClassTree(Class *);
    VObject *ReadItem(istream &is, int *ip);
    VObject *MakeLeaf(int, char *text)
	{ return new TextItem(text); }
    FileDialog *MakeFileDialog(FileDialogType);
    void DoWrite(ostream &os, int);
    void DoRead(istream &is, FileType *);
    bool CanLoadDocument(FileType *);
};

MetaImpl(myDocument, (TP(dialogView), TP(treeView), 0));

myDocument::~myDocument()
{
    SafeDelete(dialogView);
    SafeDelete(treeView);
}

bool myDocument::CanLoadDocument(FileType *ft)
{
    return strismember(ft->Type(), docTypeTree, cDocTypeAscii, 0);
}

FileDialog *myDocument::MakeFileDialog(FileDialogType fd)
{
    if (fd == eFDTypeWrite)
	return new myFileDialog;
    return Document::MakeFileDialog(fd);
}

Window *myDocument::DoMakeWindows()
{
    treeView= new myTreeView(this);
    treeView->SetTree((TreeNode*) BuildClassTree(0));
    dialogView= new PaletteView(this, treeView);
    
    return new Window(this, Point(500), eWinDefault,
	new Expander(cIdNone, eHor, gPoint3,
	    new Clipper(dialogView),
	    new Scroller(treeView),
	0)
    );
}

void myDocument::DoWrite(ostream &os, int option)
{
    if (option != 0)
	treeView->Export(os);
    else {
	Document::DoWrite(os, option);
	treeView->PrintOn(os);
    }
}

void myDocument::DoRead(istream &is, FileType *ft)
{
    if (strcmp (ft->Type(), docTypeTree) == 0) {
	Document::DoRead(is, ft);
	treeView->ReadFrom(is);
    }
    else if (strcmp(ft->Type(), cDocTypeAscii) == 0) {
	int l= 0;
	treeView->SetTree((TreeNode*)ReadTree(is, &l));
    }
}

VObject *myDocument::ReadTree(istream &is, int *level)
{
    int l;
    VObject *g= ReadItem(is, &l);

    if (l > *level) {
	OrdCollection *list= new OrdCollection;
	list->Add(g);
	do {
	    VObject *newitem= ReadTree(is, &l);
	    if (!newitem->IsKindOf(TreeNode))
		newitem= new TreeNode(5, newitem, 0);
	    list->Add(newitem);
	} while (l == *level+1);
	g= new TreeNode(4, list);
    }
    *level= l;
    return g;
}

VObject *myDocument::ReadItem(istream &is, int *ip)
{
    static int first= 1;
    static int ind= 1;

    char c, buf[100], *cp= buf;
    int i= 0;

    for (;;) {
	is.get(c);
	if (c == '\n' || is.eof())
	    break;
	*cp++= c;
    }
    *cp= 0;
    while (!is.eof()) {
	is.get(c);
	if (!Isspace(c) || is.eof())
	    break;
	if (c == ' ')
	    i++;
	else
	    i+= 8;
    } 
    is.putback(c);
    if (i && first) {
	ind= i;
	first= 0;
    }
    *ip= i/ind;
    return MakeLeaf(i, buf);
}

VObject *myDocument::BuildClassTree(Class *cl)
{
    if (cl == 0) {
	Object *o= new Object;
	gClassManager->SetupSubclasses();
	cl= o->IsA();
	delete o;
    }
    Iterator *next= cl->SubclassIterator();
    VObject *g= MakeLeaf(3, cl->Name());
    OrdCollection *list= new OrdCollection;

    list->Add(g);
    while (cl= (Class*)(*next)()) {
	VObject *newitem= BuildClassTree(cl);
	if (!newitem->IsKindOf(TreeNode))
		newitem= new TreeNode(5, newitem, 0);
	list->Add(newitem);
    }
    g= new TreeNode(4, list);
    delete next;
    return g;
}

//---- tree --------------------------------------------------------------------

class TreeApp: public Application { 
public:
    MetaDef(TreeApp);
    TreeApp(int argc, char **argv) : Application(argc, argv, docTypeTree)
	{ ApplInit(); }
    Document *DoMakeDocuments(const char*)
	{ return new myDocument; }
    bool CanOpenDocument(class FileType *);
};

MetaImpl0(TreeApp);

bool TreeApp::CanOpenDocument(class FileType *ft)
{
    return strismember(ft->Type(), docTypeTree, cDocTypeAscii, 0);    
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    TreeApp atree(argc, argv);

    return atree.Run();
}
