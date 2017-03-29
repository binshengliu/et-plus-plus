#ifndef Prefs_First
#define Prefs_First

#include "Dialog.h"
#include "Preferences_e.h"

class NumItem;
class EditTextItem;
class ManyOfCluster;
class PopupItem;

//---- Id's ----------------------------------------------------------------

const int cIdPrefOpt          = cIdFirstUser + 3,
		cIdPrfBak     = cIdPrefOpt,
		cIdPrfFast    = cIdPrefOpt   + 1,
	  cIdTxtOpt           = cIdFirstUser + 13,
		cIdPrfStyled  = cIdTxtOpt,
		cIdPrfAuto    = cIdTxtOpt    + 1;

//---- Prefstate -----------------------------------------------------------

struct Prefstate {
    bool modified;
    
    char *make;
    int tabs;
    int size;
    int npanes;
    bool fasticons;
    bool bak;
    bool autoind;
    bool styled;
    
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- PrefDialog ---------------------------------------------------------

class PrefDialog: public Dialog {
    NumItem *ni;
    EditTextItem *make;
    ManyOfCluster *gopt, *txtopt;
    PopupItem *fontsize;  
    PopupItem *npanes;  
public:
    MetaDef(PrefDialog);

    PrefDialog();

    void DoSetup();
    void DoRestore();
    void DoStore();
    VObject *DoCreateDialog();
};

#endif Prefs_First
