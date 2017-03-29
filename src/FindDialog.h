#ifndef FindDialog_First
#ifdef __GNUG__
#pragma once
#endif
#define FindDialog_First
  
#include "Dialog.h"

//---- id's ----------------------------------------------------------------
 
const int cIdFind             = cIdFirstUser + 1,
	  cIdDoFind           = cIdFirstUser + 2,
	  cIdFindOpt          = cIdFirstUser + 3,
		cIdIgnCase    = cIdFindOpt,
		cIdMatchWord  = cIdFindOpt   + 1,
	  cIdFindMode         = cIdFirstUser + 13,
		cIdForward    = cIdFindMode,
		cIdBackward   = cIdFindMode  + 1;

//---- FindDialog --------------------------------------------------------------

class FindDialog : public Dialog {
protected:
    class RegularExp *rex;
    class TextView *tvp;
    class EditTextItem *ei1;
    class OneOfCluster *modecl;
    class ManyOfCluster *optionscl;
    bool atEnd, atBegin;
    char *pattern;
public:
    MetaDef(FindDialog);
    FindDialog(char *title, class TextView *tvp);
    ~FindDialog();
    class VObject *DoCreateDialog();
    void DoSetup();
    virtual void SetupButtons();
    void Control(int id, int, void *v);
    virtual bool DoFind(EditTextItem*, bool forward, bool msg= TRUE);
};

#endif FindDialog_First
