//$GotoDialog$

#include "GotoDialog.h"
#include "Dialog.h"
#include "BlankWin.h"

#include "NumItem.h"
#include "TextView.h"
#include "BorderItems.h"
#include "Buttons.h"
#include "EnumItem.h"

#include "Document.h"

#include "WindowSystem.h"
#include "ObjectTable.h"

static GotoDialog *gotoDialog;

//---- entry Point -------------------------------------------------------------

void GotoLine(TextView *tv)
{
    if (gotoDialog == 0) {
	gotoDialog= new GotoDialog;
	ObjectTable::AddRoot(gotoDialog);
    }
    if (gotoDialog->ShowGotoDialog(tv) != cIdYes)
	return;
    int at= gotoDialog->GetLineNo();
    LineMark *lm= tv->MarkAtLine(at-1);
    tv->SetSelection(lm->Pos(),lm->End(),TRUE);
    tv->RevealSelection();
    GraphicDelay(500);
    tv->SetSelection(lm->Pos(),lm->Pos(),TRUE);    
}

//---- GotoDialog --------------------------------------------------------------

MetaImpl(GotoDialog, (TP(view), TP(line), 0));

GotoDialog::GotoDialog() : Dialog(0, eBWinBlock)
{
    line= 0;
}

int GotoDialog::ShowGotoDialog(TextView *v)
{
    lineNo= -1;
    if (v) {
	view= v;
	return ShowOnWindow(v->GetWindow());
    }
    return -1;
}
 
VObject *GotoDialog::DoCreateDialog()
{
    return new BorderItem(
	new BorderItem(
	    new Cluster(cIdNone, eVObjHCenter, 10,
		new BorderItem ("Go to line",
		    new EnumItem(cIdNone, eVObjVBase, line= new NumItem(cIdNone, 1))
		),
		new Cluster(cIdNone, eVObjVBase, 20,
		    new ActionButton(cIdYes, "Ok", TRUE),
		    new ActionButton(cIdCancel, "Cancel"),
		    0
		),
		0
	    ),
	    20, 3
	),
	Point(2,2),
	1
    );
}

void GotoDialog::DoSetup()
{
    int f,t;
    view->GetSelection(&f, &t);
    line->SetValue(view->CharToLine(f)+1);
    line->SetRange(1, max(1, view->NumberOfLines()));
}

void GotoDialog::Control(int id, int p, void *v)
{
    Dialog::Control(id, p, v);
    if (id == cIdYes)
	lineNo= line->GetValue();
}
