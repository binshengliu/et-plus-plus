#ifndef myVObjectTextView_First
#define myVObjectTextView_First

#include "VObjectTView.h"

class myVObjectTextView: public VObjectTextView {
    class Menu *notemenu;
public:
    MetaDef(myVObjectTextView);
    myVObjectTextView(class Document *d, Rectangle, class VObjectText *t);
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);    
    Command *InsertVObject(VObject *gop, bool withWrapper= TRUE);
};

#endif VObjectTextView_First
