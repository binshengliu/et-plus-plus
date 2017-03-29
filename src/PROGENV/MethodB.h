#ifndef MethodBrowser_First
#define MethodBrowser_First

#include "Dialog.h"
#include "CollView.h"
#include "ByteArray.h"
#include "ProgEnv.h"
		   
//---- MethodBrowser ------------------------------------------------------------

class MethodBrowser: public CollectionView {
    class Collection *allMethods, *filteredMethods;
    Class *ofClass;
    
    class MethodItem *GetSelectedMethod();
public:
    MetaDef(MethodBrowser);
    MethodBrowser(EvtHandler *);
    
    void DoSelect(Rectangle r, int clicks);
    void ShowMethodsOf(class Class *);
    void SelectMethod(class MethodReference *mr);
    void Filter();
    void RemoveFilter();

    Command *DoMenuCommand(int cmd);
    void DoCreateMenu(Menu *menu);
    void DoSetupMenu(Menu *menu);
};

//---- Implementors ------------------------------------------------------------

class Implementors: public CollectionView {
    bool SameMethods(char *m1, char *m2);
    bool IsDestructor(char *m1);
public:
    Implementors(EvtHandler *);
    void DoSelect(Rectangle r, int clicks);
    void ShowImplementorsOf(class MethodReference *m, bool onlySubclasses);
    void ShowInherited(MethodReference *m);
};

//---- MethodFilterDialog ----------------------------------------------------

class MethodFilterDialog : public Dialog {
    class EditTextItem *pattern;
    bool hidePublic, hidePrivate, hideProtected;
    class RegularExp *rex;
public:
    MethodFilterDialog();
    void Control(int id, int, void *v);
    VObject *DoCreateDialog();
    Collection *DoIt(Collection *allMethods);
};

#endif MethodBrowser_First

