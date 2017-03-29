//$EtPeDoc$ 
 
#include "EtPeDoc.h" 
#include "ProgEnv.h"
#include "ObjectTable.h"
 
#include "Window.h"
#include "Splitter.h"
#include "Icon.h"
#include "Buttons.h"
#include "Cluster.h"

#include "Application.h"
#include "Inspector.h"
#include "ClassTree.h"
#include "ClassManager.h"
#include "OStruBrowser.h"
#include "SrcBrowser.h"
#include "FlatInhView.h"
#include "EtPeCmdNo.h"
#include "MethodAccessor.h"

#include "Reference.h"
#include "PathLookup.h"
#include "ObjList.h"
#include "OrdColl.h"
#include "System.h"
#include "String.h"

static u_short IconBits[]= {
#   include "../images/et++pe_icon.image"
};

static u_short SrcBits[]= {
#   include "../images/et++pe_src.image"
};

static u_short InspBits[]= {
#   include "../images/et++pe_insp.image"
};

static u_short OStruBits[]= {
#   include "../images/et++pe_ostru.image"
};

static u_short HierarchyBits[]= {
#   include "../images/et++pe_hier.image"
};

static Bitmap *IconBm, *SrcBm, *InspBm, *OStruBm, *HierarchyBm;

EtPeDoc *gEtPeDoc= 0;

//---- EtPeDoc ------------------------------------------------------------

MetaImpl0(EtPeDoc);

EtPeDoc::EtPeDoc()
{
    srcCount= hierCount= ostruCount= inspCount= fihCount= 0;
    browser= 0;
    universe= 0;
    if (Getenv("ET_NO_MAPFILES")) 
	methodAccessor= new PeScanMethodAccessor();
    else
	methodAccessor= new PeMapMethodAccessor();
}

EtPeDoc::~EtPeDoc()
{
    Window *wp;
    if (browser)
	browser->RemoveObserver(this);
    browser= 0;
    SafeDelete(universe);
    SafeDelete(methodAccessor);
    Iter next(windows);
    while (wp= (Window*)next()) 
	if (wp != window) 
	    SafeDelete(wp);
}
 
Window *EtPeDoc::DoMakeWindows()
{
    if (SrcBm == 0) {
	// ObjectTable::AddRoots(
	    SrcBm= new Bitmap(64, SrcBits);
	    InspBm= new Bitmap(64, InspBits);
	    OStruBm= new Bitmap(64, OStruBits);
	    HierarchyBm= new Bitmap(64, HierarchyBits);
	//    0
	//);
    }
	    
    VObject *buttons1=
	new Cluster(1, eVObjVBase,
	    5,                      // gap between buttons
	    new Button(cIdNewInsp,      new ImageItem(InspBm)),
	    new Button(cIdNewOBrowser,  new ImageItem(OStruBm)),
	    new Button(cIdNewBrowser,   new ImageItem(SrcBm)),
	    new Button(cIdNewHier,      new ImageItem(HierarchyBm)),
	    0
	);
    VObject *all=
	new Cluster(cIdNone, eVObjHCenter,
	    10,
	    buttons1,
	    0
	);
    return new Window(this, Point(200, 20), eWinDefault, all, "ET++PE");
}
   
bool EtPeDoc::Modified()
{
    return FALSE;
}

void EtPeDoc::Control(int id, int part, void *val)
{
    switch (id) {
    
    case cIdNewInsp:
	InspectorShow(gApplication, FALSE);
	break;
	
    case cIdInspector:
	InspectorShow((Object*)val, FALSE);
	break;
	
    case cIdSpawnInsp:
	InspectorSpawn((Ref *)val, FALSE);
	break;

    case cIdNewHier: 
	HierarchyViewerShow(gClassManager->Find("Object"));
	break;
	      
    case cIdSpawnHier:
	HierarchyViewerSpawn((Class*)val);
	break;
	
    case cIdHierarchy:
	HierarchyViewerShow((Class*)val);
	break;
	
    case cIdNewOBrowser: 
	ObjectBrowserShow(gApplication);
	break;
	      
    case cIdSpawnOBrowser:
	ObjectBrowserSpawn((Object*)val);
	break;
	
    case cIdOBrowser:
	ObjectBrowserShow((Object*)val);
	break;
	
    case cIdNewBrowser:
	SrcBrowserShow(gApplication->IsA(), TRUE);
	break;
	
    case cIdBrowser:
	SrcBrowserShow((Class*)val, TRUE);
	break;
	
    case cIdSpawnFIH:
	FlatInheritanceSpawn((Class*)val);
	break;
	
    case cIdShowFIH:
	FlatInheritanceShow((Class*)val);
	break;
	
    default: 
	Document::Control(id, part, val);
    } 
}

void EtPeDoc::DoObserve(int, int what, void *, Object* op)
{
    if (what == cPartSenderDied && op == browser) 
	browser= 0;
}

void EtPeDoc::InspectorSpawn(Ref *ref, bool block)
{    
    WindowFlags fl= (WindowFlags) (block ? eWinDefault+eBWinBlock : eWinDefault);
    fl= (WindowFlags) (inspCount ? fl+eWinDestroy : fl+eWinCanClose);
    
    Inspector *ip= new Inspector();
    Window *w= new Window(this, gPoint0, 
			  fl,
			  ip,
			  MakeTitle(inspCount, "Inspector"));
    AddWindow(w);
    w->OpenAt(gApplication->GetNewDocumentPos());
    ip->Push(ref);
    if (inspCount++ == 0) {
	inspWin= w;
	inspector= ip;
    }
}

void EtPeDoc::InspectorShow(Object *op, bool block)
{
    if (inspCount == 0)
	InspectorSpawn(new Ref(*op), block);
    else {
	inspWin->Open();
	inspector->Push(new Ref(*op));
    }
}

void EtPeDoc::HierarchyViewerSpawn(Class *root)
{
    HierarchyBrowser *hb= new HierarchyBrowser(root);
    Window *w= new Window(this, 
			  Point(600,600), 
			  hierCount ? eWinDestroy : eWinCanClose, 
			  hb, 
			  MakeTitle(hierCount, "Class Hierarchy"));
    w->OpenAt(gApplication->GetNewDocumentPos());
    AddWindow(w);
    if (hierCount++ == 0) {
	hierWin= w;
	hierBrowser= hb;
    }
}

void EtPeDoc::HierarchyViewerShow(Class *selection)
{
    if (hierCount == 0)
	HierarchyViewerSpawn(gClassManager->Find("Object"));
    else
	hierWin->Open();
    hierBrowser->ShowClass(selection); 
}

void EtPeDoc::ObjectBrowserSpawn(Object *op)
{
    ObjectBrowser *ob= new ObjectBrowser();
    Window *w= new Window(this, 
			  Point(600,600), 
			  ostruCount ? eWinDestroy : eWinCanClose, 
			  ob, 
			  MakeTitle(ostruCount, "Object Structure Browser"));
    w->OpenAt(gApplication->GetNewDocumentPos());
    AddWindow(w);
    if (ostruCount++ == 0) {
	objWin= w;
	objBrowser= ob;
    }
    ob->ShowObject(op);  
}

void EtPeDoc::ObjectBrowserShow(Object *op)
{
    if (ostruCount == 0)
	ObjectBrowserSpawn(op);
    else {
	objWin->Open();
	objBrowser->ShowObject(op);
    }
}

void EtPeDoc::SrcBrowserShow(Class *cp, bool decl, int at)
{
    if (browser == 0) {
	browser= new SourceBrowser;
	browser->AddObserver(this);
	gApplication->AddDocument(browser);
	browser->OpenWindows();
    } else {
	Window *w= browser->GetWindow();
	if (w)
	    w->Open();
    }
    browser->SetClass(cp, decl, at == -1);
    if (at != -1) 
	browser->RevealAndSelectLine(at);
}

void EtPeDoc::FlatInheritanceSpawn(Class *cl)
{
    FihView *fih= new FihView(new FihClassTable, this);
    Window *w= new Window(this, 
			  Point(600, 500), 
			  fihCount ? eWinDestroy : eWinCanClose, 
			  new Splitter(fih), 
			  MakeTitle(fihCount, "Flat Inheritance View"));
    w->OpenAt(gApplication->GetNewDocumentPos());
    AddWindow(w);
    if (fihCount++ == 0) {
	fihWin= w;
	fihBrowser= fih;
    }
    fih->ShowClass(cl); 
}

void EtPeDoc::FlatInheritanceShow(Class *cl)
{
    if (fihCount == 0)
	FlatInheritanceSpawn(cl);
    else
	fihWin->Open();
    fihBrowser->ShowClass(cl); 
    fihBrowser->UpdateEvent();
}

Icon *EtPeDoc::DoMakeIcon(char *)
{
    if (IconBm == 0)
	ObjectTable::AddRoot(IconBm= new Bitmap(Point(64), IconBits));
    return new Icon(this, new ImageItem(IconBm));
}

char *EtPeDoc::MakeTitle(int count, char *label)
{
    if (count == 0)
	return form("*** %s ***", label);
    return form("%s %d", label, count);
}

PathLookup *EtPeDoc::Universe()
{
    if (!universe) {
	universe= new PathLookup("ET_SRC_PATH");
	char *p= Getenv("ET_DIR");
	if (!p) 
	    p= "/local/et";
	universe->Add(form("%s/src", p));
	universe->Add(form("%s/src/PROGENV", p));
	universe->Add(form("%s/src/POSTSCRIPT", p));
    }  
    return universe;  
}

bool EtPeDoc::FileOfClass(Class *cl, char *fname, bool decl)
{
    const char *path, *base, *bp;
    fname[0] = '\0';
    if (decl)
	base= cl->GetDeclFileName();
    else
	base= cl->GetImplFileName();
    path= "" /* cl->GetCompDir() */;
    if (base[0] != '/') {
	strcpy(fname,path);
	strcat(fname,"/");
    }
    if (base[0] == '.' && base[1] == '/') {
	bp= &base[2];
	strcat (fname,bp);
    }
    else {
	bp= base;
	strcat (fname,base);
    }
    if(gSystem->AccessPathName(fname, 4) != 0) {
	PathLookup *pl= gEtPeDoc->Universe();
	if (!pl->Lookup(bp, fname)) 
	    return FALSE;
    }
    return TRUE;
}


void EtPeDoc::Parts(Collection* col)
{
    EvtHandler::Parts(col);
}

Collection *EtPeDoc::LoadMethodsOfClass(Class *cl)
{
    Collection *methods= cl->GetMethods();
    if (methods == 0 && methodAccessor) {
	// tom@izf.tno.nl
	if (!universe)
	    universe= Universe();
	methods= methodAccessor->FindMethods(cl, universe);
	if (methods && methods->IsKindOf(OrdCollection))
	    Guard(methods, OrdCollection)->Sort();
	cl->SetMethods(methods);
    }
    return methods;
}

bool EtPeDoc::SameMethods(char *m1, char *m2)
{
    return (strcmp(m1, m2) == 0); 
}

bool EtPeDoc::IsDestructor(char *m1)
{
    return m1[0] == '~';
}

//---- MethodReference --------------------------------------------------------

MetaImpl(MethodReference, (TP(clp), T(line), TB(isPublic), 0));

MethodReference::MethodReference(int l, Class *c, char *method, bool ip)
						      : ByteArray((byte*)method)
{
    clp= c;
    line= l;
    isPublic= ip;
}

bool MethodReference::IsEqual(Object *op)
{
    if (op == 0 || !op->IsKindOf(MethodReference))
	return FALSE;
    MethodReference *mrp= (MethodReference *)op;
    return (strcmp((char*)Str(), (char*)mrp->Str()) == 0) && (clp == mrp->clp);
}
