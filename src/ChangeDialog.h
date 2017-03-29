#ifndef ChangeDialog_First
#ifdef __GNUG__
#pragma once
#endif
#define ChangeDialog_First
   
#include "FindDialog.h"
#include "Command.h"

const int cIdChange           = cIdFirstUser + 20,
	  cIdChangeBorder     = cIdFirstUser + 21,
	  cIdDoChange         = cIdFirstUser + 22,
	  cIdDoChangeAll      = cIdFirstUser + 23,
	  cIdChangeAllScope   = cIdFirstUser + 24,
	    cIdChangeAll      = cIdChangeAllScope,
	    cIdChagenSel      = cIdChangeAllScope+1;

//---- ChangeDialog ------------------------------------------------------------

class ChangeDialog : public FindDialog {
protected:
    class EditTextItem *ei2;
    class OneOfCluster *scopecl;
public:
    MetaDef(ChangeDialog);
    ChangeDialog(char *title, class TextView *tvp);
    class VObject *DoCreateDialog();
    void SetupButtons();
    void Control(int id, int, void *v);
    virtual void DoChangeAll(EditTextItem*, class Text*);
    virtual void DoChange(EditTextItem*, class Text*);
};

//---- class ChangeAllCommand ------------------------------------------------

class ChangeAllCommand: public Command {
    class TextView *tvp;
    class ChangeDialog *changedialog;
    int from, to;
    EditTextItem *find;
    class Text *change;
public:    
    ChangeAllCommand(class TextView *t, ChangeDialog *cdd, 
		    EditTextItem *find, class Text* change, int from, int to);
    void DoIt();
};

#endif ChangeDialog_First
