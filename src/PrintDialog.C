//$PrintDialog$
#include "PrintDialog.h"

#include "CheapText.h"
#include "OrdColl.h"
#include "ObjectTable.h"
#include "Error.h"

#include "Window.h"
#include "PrintPort.h"
#include "Document.h"
#include "FileDialog.h"
#include "ObjList.h"
#include "CollView.h"
#include "Scroller.h"
#include "Printer.h"
#include "Alert_e.h"
#include "NumItem.h"
#include "BorderItems.h"
#include "Buttons.h"

#include "System.h"

const int cNameMinWidth = 150;
Printer *gPrinters[10];
PrintDialog *gPrintManager= 0;
bool gPrinting= FALSE;

extern void InitPrinters();

int ShowPrintDialog(VObject *v)
{
    if (gPrintManager == 0)
	gPrintManager= new PrintDialog;
    return gPrintManager->ShowPrintDialog(v);
}

//---- PrintDialog -------------------------------------------------------------

const int cIdSaveToFile =   cIdFirstUser + 2,
	  cIdOptions    =   cIdFirstUser + 3,
	  cIdFrom       =   cIdFirstUser + 4,
	  cIdTo         =   cIdFirstUser + 5,
	  cIdShowGrid   =   cIdFirstUser + 6,
	  cIdPrint      =   cIdFirstUser + 7,
	  cIdPrinters   =   cIdFirstUser + 100;

MetaImpl(PrintDialog, (TP(saveDialog), TP(optionDialog), TP(scroller),
    TP(collview), TP(printers), TP(current), TP(lastcurrent), TP(window),
    TP(vobject), TP(from), TP(to), TP(title), TB(showgrid),
    TB(lastshowgrid), T(lastfrom), T(lastto), T(lastprinter), 0));

PrintDialog::PrintDialog(char *t) : Dialog(t, eBWinBlock)
{
    title= t;
    showgrid= FALSE;

    InitPrinters();
    ObjectTable::AddRoot(this);
    printers= new OrdCollection;
    
    for (int i= 0; gPrinters[i]; i++)
	printers->Add(new TextItem(i, gPrinters[i]->GetName(), gSysFont, Point(4,0)));

    collview= new CollectionView(this, printers);
    collview->SetMinExtent(Point(cNameMinWidth+20, 0));
    collview->SetId(cIdPrinters);
    collview->SetContainer(this);
    current= gPrinters[0];
}

PrintDialog::~PrintDialog()
{
    if (printers) {
	printers->FreeAll();
	SafeDelete(printers);
    }
}

void PrintDialog::DoSetDefaults()
{
    from->SetValue(1);
    to->SetValue(999);
    collview->SetSelection(Rectangle(0,0,1,1));
    current= gPrinters[0];
}

void PrintDialog::DoSave()
{
    lastfrom= from->GetValue();
    lastto= to->GetValue();
    lastprinter= collview->GetSelection().origin.y;
    lastcurrent= current;
    lastshowgrid= showgrid;  // joep@tnosoes
}

void PrintDialog::DoRestore() 
{
    from->SetValue(lastfrom);
    to->SetValue(lastto);
    collview->SetSelection(Rectangle(0,lastprinter,1,1));    
    current= lastcurrent;
    showgrid= lastshowgrid;   // joep@tnosoes
    // ((RadioButton*)((LabeledButton*)optionDialog->GetInterior())->GetItem(0,0))->SetState(showgrid);
}

void PrintDialog::DoSetup()
{
    bool b= current != 0;
    EnableItem(cIdSaveToFile, b);
    EnableItem(cIdOptions, b && current->GetOptions());
    EnableItem(cIdPrint, b && current->CanPrint());
}

int PrintDialog::ShowPrintDialog(VObject *v)
{
    if (v) {
	window= v->GetWindow();
	oldvobject= vobject;
	vobject= v;
	if (v != oldvobject && oldvobject && showgrid)
	    oldvobject->ForceRedraw();
	return ShowOnWindow(window);
    }
    return -1;
}
 
VObject *PrintDialog::DoCreateDialog()
{
    //---- dialog parts ----
    
    scroller= new Scroller(collview, Point(cNameMinWidth, 16*4), cIdPrinters);

    // VObject *Options=
    optionDialog=
	    new BorderItem("Options", new LabeledButton(cIdShowGrid, "show pages"));

    VObject *Actions=
	new Cluster(cIdNone, (VObjAlign)(eVObjHLeft|eVObjHExpand), 8,
	    new ActionButton(cIdYes,        "Apply", TRUE),
	    new ActionButton(cIdPrint,      "Print"),
	    new ActionButton(cIdSaveToFile, "Save as ..."),
	    new ActionButton(cIdOptions,    "Options"),
	    new ActionButton(cIdCancel,     "Cancel"),
	    new ActionButton(cIdDefault,    "Default"),
	    0
	);
	
    VObject *Pages=
	new BorderItem("Pages",
	    new Cluster(cIdNone, (VObjAlign)(eVObjVBase|eVObjHExpand), 20,
		new TextItem("From:"),
		from= new NumItem(cIdFrom, 1, 1, 999, 3, 1),
		new TextItem("To:"),
		to= new NumItem(cIdTo, 999, 1, 999, 3, 1),
		0
	    )
	);
	
    //---- overall layout ----
    return
	new BorderItem(
	    new Cluster(cIdNone, eVObjVTop, 20,
		new Cluster(cIdNone, eVObjHLeft, 20,
		    scroller,
		    optionDialog,
		    Pages,
		    0
		),
		Actions,
		0
	    ),
	    20, 0
	);
}

void PrintDialog::Control(int id, int p, void *v)
{
    switch (id) {
    
    case cIdShowGrid:
	showgrid= !showgrid;
	if (vobject)
	    vobject->ForceRedraw();
	break;
    
    case cIdOptions:
	if (current && current->GetOptions()) {
	    if (current->ShowOnWindow(window) == cIdOk) {
		if (vobject)
		    vobject->ForceRedraw();
	    }
	}
	return;
    
    case cIdPrinters:
	if (p == cPartCollSelect) {
	    current= gPrinters[(int) v];
	    DoSetup();
	}
	break;
    
    case cIdSaveToFile:
	if (saveDialog == 0)
	    saveDialog= new FileDialog();
	if (saveDialog->ShowInWindow(eFDWrite, window) == cIdOk) {
	    Print(saveDialog->FileName());
	    Dialog::Control(cIdYes, p, v);
	}
	break;
	
    case cIdPrint:
	Print(0);
	break;
    }
    Dialog::Control(id, p, v);
}

void PrintDialog::ShowPageGrid(Rectangle, VObject *v)
{
    if (showgrid && !gPrinting && current && v) {
	int x, y;
	Point e= v->GetExtent(), ps= GetPageSize();

	GrSetPenSize(3);
	GrSetPenPattern(ePatGrey50);
	for (x= ps.x; x <= e.x; x+= ps.x)
	    GrLine(Point(x,0), Point(x,e.y));
	for (y= ps.y; y <= e.y; y+= ps.y)
	    GrLine(Point(0,y), Point(e.x,y));
	GrSetPenNormal();
   }
}

void PrintDialog::Print(char *name)
{
    PrintPort *newport= current->MakePrintPort(name);
    if (newport) {
	GrCursor oldcursor= GrSetWaitCursor();
	PortDesc currport= GrGetPort();
	PortDesc oldport= window->GetPortDesc();

	gPrinting= TRUE;
	GrSetPort(newport);
	window->SetPortDesc(newport);
	
	Point po, pe= GetPageSize();
	Rectangle vr= vobject->contentRect;
	int np= 1;
	
	for (po.y= 0; po.y <= vr.extent.y; po.y+= pe.y) {
	    for (po.x= 0; po.x <= vr.extent.x; po.x+= pe.x) {
		if (np >= from->GetValue() && np <= to->GetValue()) {
		    if (TestInterrupt("printing"))
			goto out;
		    newport->OpenPage(np);
			GrSetClip(pe, -po);
			GrSetPenNormal();
			GrSetPattern(ePatBlack);
			vobject->DrawAll(Rectangle(po, pe), FALSE);
			newport->GiveHint(eHintFlush, 0, 0);
		    newport->ClosePage();
		}
		np++;
	    }
	}
out:
	window->SetPortDesc(oldport);
	gPrinting= FALSE;
	
	GrSetPort(currport);
	GrSetCursor(oldcursor);
	SafeDelete(newport);
    }
}

Point PrintDialog::GetPageSize()
{
    return current->GetPageSize();
}
