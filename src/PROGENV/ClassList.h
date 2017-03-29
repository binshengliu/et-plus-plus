#ifndef ClassListView_First
#define ClassListView_First

#include "CollView.h"
#include "ObjArray.h"

//---- ClassListView ---------------------------------------------------------

class ClassListView: public CollectionView {
    class OrdCollection *classItems;
    bool hideEmptyClasses;
    bool showInstCount;
public:
    MetaDef(ClassListView);
    ClassListView(EvtHandler *, bool icount= FALSE);
    ~ClassListView();
    void LoadClasses();
    void ToggleHideEmpty();
    bool HideEmptyClasses()
	{ return hideEmptyClasses; }
    bool SelectClass(Class *cl);
    Class *SelectedClass();
    void DoSelect(Rectangle r, int clicks);
    void DoObserve(int, int what, void* val, Object* op);
};

#endif ClassListView_First
