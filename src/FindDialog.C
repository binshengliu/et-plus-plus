//$FindDialog$

#include "RegularExp.h"
#include "FindDialog.h"
#include "Alert_e.h"
#include "String.h"
#include "BorderItems.h"
#include "OneOfCluster.h"
#include "ManyOfCluster.h"
#include "EditTextItem.h"
#include "Buttons.h"
#include "TextView.h"
#include "Window.h"
#include "Document.h"
#include "Expander.h"
#include "Form.h"

//---- FindDialog -------------------------------------------------------------

MetaImpl(FindDialog, (TP(rex), TP(tvp), TP(ei1), TB(atEnd), TB(atBegin),
			TP(modecl), TP(optionscl), 0));


FindDialog::FindDialog(char *title, TextView *tv) : Dialog(title, eWinDefault)
{
    tvp= tv;
    rex= new RegularExp;
    pattern= 0;
    atEnd= atBegin= FALSE;
}

FindDialog::~FindDialog()
{
    SafeDelete(rex);
    SafeDelete(pattern);
}

VObject *FindDialog::DoCreateDialog()
{
    //---- dialog parts ----
    VObject *find= 
	new Form(cIdNone, eVObjHCenter, 2,
		"Find:", ei1= new EditTextItem(cIdFind, "", 270),
	    0);
    find->SetFlag(eVObjVFixed);

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
    VObject *Actions= new ActionButton (cIdDoFind, "Find Next", TRUE);

    //---- overall layout ----
    return
	new BorderItem(
	    new Expander(cIdNone, eVert, 10,
		find,
		new Expander(cIdNone, eHor, 10,
		    Mode,
		    Options,
		    0 
		),
		Actions,
		0
	    ),
	    10, 0
	);
}

void FindDialog::SetupButtons()
{
    EnableItem(cIdDoFind, ei1->GetTextSize() > 0);
}

void FindDialog::DoSetup()
{
    // copy selection in associated TextView as default value 
    if (tvp->HasSelection()) {
	int from, to;
	tvp->GetSelection(&from, &to);
	if (tvp->CharToLine(from) == tvp->CharToLine(to)) {
	    byte buf[1000];
	    tvp->GetText()->CopyInStr(buf, sizeof buf, from, to);
	    ei1->SetString(buf);
	    SetKbdFocus(ei1);
	}
    }
    SetupButtons();
    UpdateEvent();
}

void FindDialog::Control(int id, int p, void *v)
{
    switch (id) {

    case cIdFind:
	Dialog::Control(id, p, v);
	if (p == cPartChangedText)
	    SetupButtons();
	break;

    case cIdDoFind:
	DoFind(ei1, modecl->GetCurrentItem() == cIdForward);
	optionscl->SetAllItemsOff();
	tvp->RevealSelection();
	break;

    default:
	Dialog::Control(id, p, v); 
	break;
    }
}

bool FindDialog::DoFind(EditTextItem *e, bool direction, bool msg)
{
    bool found;
    char *wpattern= 0;

    if (atEnd) {
	tvp->Home();
	atEnd= FALSE;
    }
    if (atBegin) {
	tvp->Bottom();
	atBegin= FALSE;
    }  
    SafeDelete(pattern);
    char *p= pattern= e->GetText()->AsString();
    if (optionscl->IsItemOn(cIdMatchWord)) {
	wpattern= strprintf((char*)rex->MatchWordPattern(), pattern);
	p= wpattern;
    }
    rex->Reset(p, optionscl->IsItemOn(cIdIgnCase));
    SafeDelete(wpattern);
    if (rex->GetExprState()) {
	ShowAlert(eAlertNote, rex->GetExprState());
	return FALSE;
    }
    found= tvp->SelectRegExpr(rex, direction);
    if (!found && msg) {
	ShowAlert(eAlertNote, "\"%s\" not found", pattern);
	if (direction)
	    atEnd= TRUE;
	else
	    atBegin= TRUE;
    }
    return found;
}
