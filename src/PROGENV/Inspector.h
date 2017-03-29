#ifndef Inspector_First
#define Inspector_First

#include "CollView.h"
#include "Expander.h"

//---- Inspector ---------------------------------------------------------------

class Inspector: public Expander {
    CollectionView *objects, *references;
    class ClassListView *classes;
    OrdCollection *ClassItems;
    SeqCollection *ObjectItems, *ReferenceItems;
    TextItem *refTitle, *objTitle; 
    VObject *start, *left, *right, *end;   
    Object *currentOp;
    class FindInspReferences *findReferences;
    class SysEvtHandler *refresh;
    class ObjectView *objView;
    class OrdCollection *path;
    int position;
    
    static OrdCollection *allInspectors;

public:
    MetaDef(Inspector);
    Inspector();
    ~Inspector();
    void Init(Object *op);
    void Layout();
    void LoadObjectsOfClass(class Class *, bool members);
    void Member(int mode, char *name, int offset, int size= 0);
    void References(Object*);
    void Reset();
    void ResetReferences();
    void FoundReference(Object *op, char *name, int index= -1);
    void ResetObjects();
    void FreedMemory(void *addr, void *end);
    void DoInspect(Object *);
    
    void MoveTo(int pos);
    void Push(class Ref *r);
    void ShowReference(class Ref *);
    void ShowClass(Class *cl);
    void ShowObject(Object *op);
    void SelectAndRevealItem(CollectionView *cv, int at);

    //---- menus ------------
    void Control(int id, int, void *v);
    void DoSetupMenu(Menu *menu);
    Command *DoMenuCommand(int cmd);
    void UpdateButtons();

    //---- commands ---------
    void ShowAllInstances();
    
    friend void InspectorFreeHook(void *, void *, size_t);
    void RemoveDeletedRef(int i, class Ref *r);
};

#endif Inspector_First
