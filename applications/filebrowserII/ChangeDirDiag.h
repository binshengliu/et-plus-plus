#ifndef ChangeDirDiag_First
#define ChangeDirDiag_First

#include "Dialog.h"

//---- ChangeDirDiag --------------------------------------------------------------

class ChangeDirDiag : public Dialog {
protected:
    class BrowserView *bvp;
    class EditTextItem *ei1;
    class CollectionView *collview;
    class OrdCollection *mylist;
    class Scroller *scroller;

    void UpdateHistory(char *pathname);
    
public:
    MetaDef(ChangeDirDiag);
    ChangeDirDiag(class BrowserView *vp);
    ~ChangeDirDiag() {};
    class VObject *DoCreateDialog();
    void DoSetup();
    void Control(int id, int, void *v);
    bool ChangeDir();
};

#endif ChangeDirDiag_First
