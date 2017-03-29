//$ChangeDialog,ChangeAllCommand$

#include "RegularExp.h"
#include "ChangeDialog.h"
#include "Alert_e.h"
#include "String.h"
#include "TextView.h"
#include "BorderItems.h"
#include "OneOfCluster.h"
#include "ManyOfCluster.h"
#include "EditTextItem.h"
#include "Buttons.h"
#include "Window.h"
#include "Document.h"
#include "Expander.h"
#include "Form.h"

//---- ChangeDialog ------------------------------------------------------------

MetaImpl(ChangeDialog, (TP(scopecl)));

ChangeDialog::ChangeDialog(char *title, TextView *tv) : FindDialog(title, tv)
{
}

VObject *ChangeDialog::DoCreateDialog()
{
    //---- dialog parts ----
    VObject *findChange= 
	new Form(cIdNone, eVObjHCenter, 2,
		"Find:", ei1= new EditTextItem(cIdFind, "", 370),
		"Change:", ei2= new EditTextItem(cIdChange, "", 370),
	    0);
    findChange->SetFlag(eVObjVFixed);

    VObject *Mode=
	new BorderItem ("Direction",
	    modecl= new OneOfCluster(cIdFindMode, eVObjHLeft, 0, "Forward", "Backward", 0),
	    Point(6,4)
	);
    VObject *Options=
	new BorderItem ("Options",
	    optionscl= new ManyOfCluster(cIdFindOpt, eVObjHLeft, 0,
					"Ignore Case", "Match Whole Word", 0),
	    Point(6,4)
	);

    VObject *ChangeAllScope=
	new BorderItem ("Change All Scope",
	    scopecl= new OneOfCluster(cIdChangeAllScope, eVObjHLeft, 0,
					"All of Document", "Selection Only", 0),
	    Point(6,4)
	);

    VObject *Actions=
	new Expander (cIdNone, eHor, 10, 
	    new ActionButton (cIdDoFind, "Find Next", TRUE), 
	    new ActionButton (cIdDoChange, "Change, Then Find"), 
	    new ActionButton (cIdDoChangeAll, "Change All"),
	    0
	);

    //---- overall layout ----
    return
	new BorderItem(
	    new Expander(cIdNone, eVert, 10,
		findChange,
		new Expander (cIdNone, eHor, 10,
		    Mode,
		    Options,
		    ChangeAllScope,
		    0 
		),
		Actions,
		0
	    ),
	    10, 0
	);
}

void ChangeDialog::SetupButtons()
{
    FindDialog::SetupButtons();
    bool b= ei1->GetTextSize() > 0;
    EnableItem(cIdDoChange, b);
    EnableItem(cIdDoChangeAll, b);
    EnableItem(cIdChangeBorder, b);
}

void ChangeDialog::Control(int id, int p, void *v)
{
    switch (id) {

    case cIdDoChange:
	DoChange(ei1, ei2->GetText());
	break;

    case cIdDoChangeAll:
	DoChangeAll(ei1, ei2->GetText());
	break;

    default:
	FindDialog::Control(id, p, v); 
	break;
    }
}

void ChangeDialog::DoChange(EditTextItem *e, Text *c)
{
    int from, to, f1, t1;

    tvp->GetSelection(&from, &to);
    bool forward= modecl->GetCurrentItem() == cIdForward;
    
    if (forward)
	tvp->SetSelection(from, from);
    else
	tvp->SetSelection(to, to);
    if (DoFind(e, forward)) {
	tvp->GetSelection(&f1,&t1);
	if (f1 == from && t1 == to) {
	    tvp->GetSelection(&from, &to);
	    tvp->PerformCommand(tvp->InsertText(c));
	    if (!forward)
		tvp->SetSelection(from, from);
	    DoFind(ei1, forward);
	}
    }
    tvp->RevealSelection();
}

void ChangeDialog::DoChangeAll(EditTextItem *e, Text *c)
{
    if (ShowAlert(eAlertCaution, "\"Change All\" not undo-able, go ahead?") == cIdYes) {
	int from, to;

	tvp->GetSelection(&from, &to);
	if (scopecl->GetCurrentItem() == cIdChangeAll) {
	    from= 0;
	    to= tvp->GetText()->Size();
	}
	tvp->PerformCommand(new ChangeAllCommand(tvp, this, e, c, from, to));
    }
}

//---- class ChangeAllCommand -------------------------------------------------

ChangeAllCommand::ChangeAllCommand(TextView *tv, ChangeDialog *cd, 
				    EditTextItem *fnd, Text* c, int f, int t)
     : Command(cCHANGEALL, "ChangeAll", (CommandFlags)(eCmdCausesChange | eCmdDoDelete))
{
    tvp= tv;
    changedialog= cd;
    from= f;
    to= t;
    find= fnd;
    change= c;
};

void ChangeAllCommand::DoIt()
{
    int f, t, nChanges= 0;

    tvp->SetSelection(from, from);
    while (!TestInterrupt("search") && changedialog->DoFind(find, TRUE, FALSE)) {
	tvp->GetSelection(&f, &t);
	if (t > to)
	    break;
	tvp->Paste(change);
	nChanges++;
    }
    if (nChanges) {
	tvp->ForceRedraw();
	tvp->RevealSelection();
	tvp->UpdateEvent();
    }
    ShowAlert(eAlertNote, "%d occurrences changed", nChanges);
}
