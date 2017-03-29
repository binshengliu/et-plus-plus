#ifndef ObjectView_First
#define ObjectView_First

#include "CollView.h"

//---- ObjectView --------------------------------------------------------------

class ObjectView: public CollectionView {
    static class ShowMembers *accessor;
    static int accessorcnt;
    
    class Ref *inspected;
    bool shifted;
public:
    MetaDef(ObjectView);

    ObjectView();
    ~ObjectView();
    
    void DoSelect(Rectangle r, int clicks);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void UpdateViews();
    void SetInspected(Ref *newinsp);
    Ref *GetInspected()
	{ return inspected; }

    void Build();
    void BuildInstanceView();
    void BuildArrayView();
    void Add(class InspectorItem0 *ii);

    void DoCreateMenu(Menu *menu);
    void DoSetupMenu(Menu *menu);
    Command *DoMenuCommand(int);
    Command *DispatchEvents(Point, Token, Clipper*);

    class InspectorItem *At(int i)
	{ return (InspectorItem*) GetCollection()->At(i); }
    void DoObserve(int, int, void*, Object*);
    bool PrintOnWhenObserved(Object *);
    void InspectedDied();
};

#endif ObjectView_First

