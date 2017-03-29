#ifndef GotoDialog_First
#ifdef __GNUG__
#pragma once
#endif
#define GotoDialog_First

#include "GotoDialog_e.h"
#include "Dialog.h"

//---- GotoDialog ---------------------------------------------------------------

class GotoDialog : public Dialog {
    class TextView *view;
    class NumItem *line;
    int lineNo;
    
    VObject *DoCreateDialog();
    void Control(int id, int, void *v);
public:
    MetaDef(GotoDialog);
    
    GotoDialog();
    int ShowGotoDialog(class TextView*); 
    void DoSetup(); 
    GetLineNo()
	{ return lineNo; }
};

#endif GotoDialog_First

