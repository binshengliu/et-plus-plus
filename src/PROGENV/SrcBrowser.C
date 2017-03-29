//$SourceBrowser,SrcBrowserPPrinter,MethodMark,SrcBrowserTextView$
  
#include "CodeTextView.h"
#include "StyledText.h"
#include "OrdColl.h"
#include "WindowSystem.h"
#include "PathLookup.h"
#include "GotoDialog.h"
#include "ClassManager.h"
#include "Alert_e.h"
#include "Menu.h"
#include "Application.h"
#include "Scroller.h"
#include "Expander.h"
#include "Clipper.h"
#include "Window.h"

#include "SrcBrowser.h"
#include "EtProgEnv.h"
#include "ClassList.h"
#include "MethodB.h"
#include "EtPeCmdNo.h"
#include "EtPeDoc.h"

//---- SourceBrowser -------------------------------------------------------------

MetaImpl(SourceBrowser, (TP(textview), TP(listview), TP(text), TP(ccl), 
		       TP(log), TB(isCCode), 0));

static int srcCount= 0;

SourceBrowser::SourceBrowser() : Document(cDocTypeAscii)
{
    Font *fd= gFixedFont;
    if (Getenv("ET_NO_STYLEDCODE") == 0)
	text= new StyledText(256, fd);
    else
	text= new GapText(256, fd);
    if (Getenv("ET_NOBACKUP") == 0)
	EnableBackups(TRUE);
    text->SetTabWidth(fd->Width(' ')*8);
    ccl= 0;
    definition= FALSE;
    log= new OrdCollection;
    isCCode= FALSE;
    base= srcCount++ == 0;
}

SourceBrowser::~SourceBrowser()
{
    SafeDelete(textview);
    SafeDelete(listview);
    SafeDelete(text);
    SafeDelete(log);
}

Window *SourceBrowser::DoMakeWindows()
{   
    Menu *m= new Menu("Classes");
    m->AppendItems(
		      "other",                  cEDITOTHER,
		      "superclass",             cEDITSUPER,
		      "spawn",                  cSPAWN,
		      "-",
		      "inspect some instance",  cINSPECTSOME,
		      "-",
		      "show in hierarchy",      cSHOWHIERARCHY,
		      "show inheritance path",  cSHOWINHPATH,
		      "-",
		      "previous class",         cGOBACK,
		      "empty classes",          cEMPTYCLASSES,
		      0);

    textview= new SrcBrowserTextView(this, Rectangle(Point(2000,cFit)), text);
    listview= new ClassListView(this);
    listview->SetMenu(m); 
    methods= new MethodBrowser(this);
    implementors= new Implementors(this);
    VObject *scr= new Scroller(listview, Point(-1, 100));
    scr->SetFlag(eVObjHFixed | eVObjVFixed);
    implTitle= new TextItem("", gFixedFont->WithFace(eFaceItalic), Point(2,1));
    VObject *v= new Expander(cIdNone, eHor, gPoint2,
	    scr,
	    new Expander(cIdNone, eVert, gPoint2,
		new Scroller(methods),
	    0),
	    new Expander(cIdNone, eVert, gPoint2, 
		new Clipper(implTitle, Point(0,-1)), 
		new Scroller(implementors),
	    0),
	0); 
	
    return new Window(this, Point(600, 500), eWinCanClose,
	new Expander(cIdNone, eVert, gPoint2,
	    v,
	    new Scroller(textview,Point(550, 300)),
	0));
}

bool SourceBrowser::Open()
{
    int rcode= Document::Open();
    if (rcode) {
	ccl= 0;
	listview->SelectClass(ccl);
	methods->ShowMethodsOf(ccl);
	Send(cIdCLSelectClass, 0, ccl);
    }
    return rcode;
}

void SourceBrowser::SetWindowTitle(char *name)
{
    if (base)
	GetWindow()->SetTitle(form("*** %s ***", name));
    else
	Document::SetWindowTitle(name);
}

bool SourceBrowser::CanImportDocument(FileType *ft)
{
    if (ft == 0)
	return TRUE;
    return strismember(ft->Type(), cDocTypeAscii, 0);
}

Command *SourceBrowser::DoImport(istream& s, FileType *ft)
{
    GapText *t = new GapText(1024);
    Command *cmd;
    
    t->ReadFromAsPureText(s, ft->SizeHint());
    cmd= textview->InsertText(t);
    SafeDelete(t);
    return cmd;
}

void SourceBrowser::DoWrite(ostream &s, int)
{
    // ??? terminate last line with a '\n' ???
    if ((*text)[text->Size()-1] != '\n')
	text->Append('\n');
    text->PrintOnAsPureText(s);
}

void SourceBrowser::DoRead(istream &s, FileType *ft)
{
    text->ReadFromAsPureText(s, ft->SizeHint());
    if (isCCode= ft->IsCCode())
	textview->FormatCode();
    else
	textview->SetDefaultStyle();
    textview->SetText(text);
}

void SourceBrowser::RevealAndSelectLine(int l)
{
    LineMark *lm= textview->MarkAtLine(range(0, textview->NumberOfLines()-1, l-1));
    textview->SetSelection(lm->Pos(),lm->End());
    Rectangle r= textview->SelectionRect();
    textview->Scroll(cPartScrollAbs, Point(0,r.origin.y)+Point(0,2));
}
 
void SourceBrowser::Log(Class *cl)
{
    if (cl != ccl)
	log->Add(cl);
}

void SourceBrowser::Control(int id, int part, void *val)
{
    MethodReference *mr= (MethodReference *)val;
    
    switch (id) {
    case cIdCLChangedClass:
	SetClass((Class*)val, definition);
	break;
	
    case cIdChangedMethod:
	RevealMethod(mr);
	break;
	
    case cIdImplementors:
	{ 
	GrShowWaitCursor wc;
	implTitle->SetFString(TRUE, "implementors of: %s", mr->Str());
	implementors->ShowImplementorsOf(mr, FALSE);
	}
	break;
	
    case cIdInherited:
	implTitle->SetFString(TRUE, "inherited: %s", mr->Str());
	implementors->ShowInherited(mr);
	break;
	
    case cIdOverrides:
	implTitle->SetFString(TRUE, "overrides of: %s", mr->Str());
	implementors->ShowImplementorsOf(mr, TRUE);
	break;
	
    default:
	Document::Control(id, part, val);
    }
}

void SourceBrowser::SetClass(Class *cl, bool what, bool reveal)
{
    Log(cl);
    DoSetClass(cl, what, TRUE, reveal);
}

void SourceBrowser::RevealMethod(MethodReference *mr)
{
    SetClass(mr->GetClass(), FALSE, FALSE);
    textview->ShowMethod(mr);
    methods->SelectMethod(mr);
}

void SourceBrowser::DoSetClass(Class *cl, bool decl, bool unique, bool reveal)
{
    char fname[1000];
    bool sameClass= (cl == ccl && decl == definition);
    bool sameFile= FALSE;

    if (!sameClass) {
	if (!gEtPeDoc->FileOfClass(cl, fname, decl)) {
	    ShowAlert(eAlertNote, "Can't find file @B%s@P in ET_SRC_PATH\n",
					    BaseName(fname));
	    return;
	}
	FType ft(fname);
	if (ft.UniqueId() != UniqueId()) {
	    if (Modified()) {
		switch (ShowAlert(eAlertCaution, "Save changes to \"%s\" ?",
			GetName())){
		case cIdYes:
		    Save();
		    break;
		case cIdCancel:
		    listview->SelectClass(ccl);
		    Send(cIdCLSelectClass, 0, ccl);
		    return;
		}
	    }
	    PerformCommand(gResetUndo);
	    Load(fname, unique, ft.FileType());
	}
	else
	    sameFile= TRUE;
	definition= decl;
	ccl= cl;
	listview->SelectClass(ccl);
	Send(cIdCLSelectClass, 0, ccl);
	methods->ShowMethodsOf(cl);
    }
    if (reveal)
	SelectSourceLine(sameFile);        
}

void SourceBrowser::SelectSourceLine(bool)
{
    int line;
    if (definition)
	line= ccl->GetDeclFileLine();
    else 
	line= ccl->GetImplFileLine();

    RevealAndSelectLine(line);
}

void SourceBrowser::SetMode(bool what)
{
    SetClass(ccl, what);
}
    
void SourceBrowser::Spawn()
{    
    if (Modified() &&
	ShowAlert(eAlertCaution, "Save changes to \"%s\" ?", GetName()) == cIdYes)
	Save();
    SourceBrowser *browser= new SourceBrowser;
    gApplication->AddDocument(browser);
    browser->OpenWindows();
    browser->DoSetClass(ccl, definition, FALSE, TRUE);
}

void SourceBrowser::EditSelectedClass()
{
    Class *clp;
    char *classname;

    classname= FirstWordOfSelection();
    clp= gClassManager->Find(classname);
    if (clp)
	SetClass(clp,definition);
    else
	ShowAlert(eAlertNote, "No Metaclass found for \"%s\" ?", classname);
}
 
void SourceBrowser::ShowImplementors()
{
    char *method= FirstWordOfSelection();
    MethodReference mr(0, 0, method, FALSE);
    implementors->ShowImplementorsOf(&mr, FALSE);
}

void SourceBrowser::InspectSomeInstance()
{ 
    Object *op= ccl->SomeMember();
    if (!op)
	op= ccl->SomeInstance();
    if (op)
	op->Inspect();
    else
	ShowAlert(eAlertNote, "No instances found");
}

void SourceBrowser::DoCreateMenu(Menu *menu)
{
    Document::DoCreateMenu(menu);
    
    Menu *file= menu->FindMenuItem(cFILEMENU);
    Menu *util= new Menu("utilities");
    util->AppendItems(
		 "edit selected class",     cEDITSELECTED,
		 "go to line...",           cGOTOLINE,
		 "reformat",                cREFORMAT,
		0);
    menu->AppendMenu(util, cUTILMENU);
}

void SourceBrowser::DoSetupMenu(Menu *menu)
{
    menu->ToggleItem(cEDITOTHER, definition, "implementation", "definition");

    if (ccl)
	menu->EnableItems(cEDITOTHER, 
			  cSPAWN, 
			  cSHOWHIERARCHY,
			  cSHOWINHPATH, 
			  cINSPECTSOME,
			  0);
			  
    if (isCCode && text->IsKindOf(StyledText))
	menu->EnableItem(cREFORMAT);

    menu->EnableItems(cGOTOLINE, cEMPTYCLASSES, 0);
    
    if (ccl && ccl->Super())
	menu->EnableItem(cEDITSUPER);
    if (log->Size() > 1)
	menu->EnableItem(cGOBACK);
	 
    menu->ToggleItem(cEMPTYCLASSES, listview->HideEmptyClasses(), 
			 "show all classes", "hide classes with no instances");
    
    menu->ReplaceItem(cEDITSELECTED, "selected class/method");
    if (!textview->Caret()) {
	char *q= FirstWordOfSelection();
	if (strlen(q)) {
	    menu->ReplaceItem(cEDITSELECTED, form("show \"%s\"", q));
	    menu->EnableItem(cEDITSELECTED);
	}
    }
    Document::DoSetupMenu(menu);
}

char *SourceBrowser::FirstWordOfSelection()
{
    static byte buf[60];
    textview->SelectionAsString(buf, sizeof buf);
    for (byte *q= buf; *q; q++)
	if (Isinword(*q)) break;
    for (byte *p= q; *p; p++)
	if (!Isinword(*p)) break;
    *p= '\0';
    return (char*) q;
}

Command *SourceBrowser::DoMenuCommand(int cmd)
{
    char *q;
    
    switch (cmd) {

    case cEDITOTHER:
	SetMode(!definition);
	break;
    
    case cEDITSUPER:
	SetClass(ccl->Super(), definition);
	break;
	
    case cINSPECTSOME:
	InspectSomeInstance();
	break;
	
    case cEDITSELECTED:
	{ 
	GrShowWaitCursor wc;
	q= FirstWordOfSelection();
	if (gClassManager->Find(q))
	    EditSelectedClass();
	else
	    ShowImplementors();
	}
	break;
	
    case cEMPTYCLASSES:
	listview->ToggleHideEmpty();
	listview->SelectClass(ccl);
	break;
    
    case cGOBACK:
	log->RemoveAt(log->Size()-1);
	DoSetClass((Class*)log->Last(), definition, TRUE, TRUE);
	break;

    case cSPAWN:
	Spawn();
	break;
	
    case cSHOWHIERARCHY:
	gEtPeDoc->Control(cIdHierarchy, 0, ccl);
	break;
	
    case cSHOWINHPATH:
	gEtPeDoc->Control(cIdShowFIH, 0, ccl);
	break;

    case cGOTOLINE:
	GotoLine(textview);
	break;
	
    case cREFORMAT:
	textview->FormatCode();
	textview->ForceRedraw();
	return gNoChanges;
    default:
	break;
    }
    return Document::DoMenuCommand(cmd);
}
 
void SourceBrowser::Parts(Collection *col)
{
    EvtHandler::Parts(col);
}

//---- SrcBrowserTextView ---------------------------------------------------------

MetaImpl0(SrcBrowserTextView);

SrcBrowserTextView::SrcBrowserTextView(EvtHandler *eh, Rectangle r, Text *t)
					       : CodeTextView(eh, r, t)
{
}
						   
PrettyPrinter *SrcBrowserTextView::MakePrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps)
{
    return new SrcBrowserPPrinter(this, t, cs, fs, cds, ps);
}

void SrcBrowserTextView::ShowMethod(MethodReference *mr)
{
    MarkList *mlp= GetMarkList();
    Iter next(mlp);
    MethodMark *mp;
    bool found= FALSE;
    
    while (mp= (MethodMark*)next()) {
	if (strcmp(mp->className, mr->GetClass()->Name()) == 0 &&
				 strcmp(mp->method, (char*)mr->Str()) == 0) {
	    SetSelection(mp->Pos(),mp->End());
	    found= TRUE;
	    break;
	}
    }
    // no mark found, use position information stored in mr
    if (!found) {
	LineMark *lm= MarkAtLine(range(0, NumberOfLines()-1, mr->Line()-1));
	SetSelection(lm->Pos(),lm->End());
    }
    Rectangle r= SelectionRect();
    Scroll(cPartScrollAbs, Point(0,r.origin.y)+Point(0,-4));
}

//---- SrcBrowserPPrinter ---------------------------------------------------------

SrcBrowserPPrinter::SrcBrowserPPrinter(SrcBrowserTextView *tv, Text *t, Style *cs, 
	    Style *fs, Style *cds, Style *ps) : PrettyPrinter(t, cs, fs, cds, ps)
{
    tvp= tv;
    MarkList *mlp= tv->GetMarkList();
    mlp->FreeAll();
}

void SrcBrowserPPrinter::Function(int line, int start, int end, char *name, char *classname)
{
    PrettyPrinter::Function(line, start, end, name, classname);

    if (classname) 
	tvp->AddMark(new MethodMark(classname, name, start, end-start));
}

//---- MethodMark -------------------------------------------------------

MetaImpl(MethodMark, (TP(className), TP(method), 0));
    
MethodMark::MethodMark(char *c, char *m, int p, int l) : Mark(p, l)
{
    className= method= 0;
    strreplace(&className, c);
    strreplace(&method, m);
}

MethodMark::~MethodMark()
{
    SafeDelete(className);
    SafeDelete(method);
}

