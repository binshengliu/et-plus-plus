#ifndef OStruBrowser_First
#define OStruBrowser_First

#include "GraphView.h"
#include "Expander.h"

//---- Object Structure Browser ------------------------------------------------

class ObjectStructureView: public GraphView {
    Object *currentOp;
    Object *rootOp;
    bool lines;
    bool varNames;
    class Menu *menu;
public:
    MetaDef(ObjectStructureView);
    ObjectStructureView();

    Object *GetRoot();
    VObject *NodeAsVObject(Object *);
    Iterator *MakeSubPartsIter(Object *);
    Command *NodeSelected(VObject*, int cl);

    Menu *GetMenu();
    Command *DoMenuCommand(int cmd);
    void DoCreateMenu(class Menu *menu);
    void DoSetupMenu(class Menu *menu);

    //---- queries 
    bool CheckObject(Object*);
    void EvtHandlers(Object*);
    void Containers(Object*);
    void Dependents(Object*);
    void References(Object*);
    void Pointers(Object*);
    void ShowObject(Object*);
    void ToggleShowLines();
    
    void LoadETGraph(Object *root);
};

//---- ObjectBrowser ----------------------------------------------------

class ObjectBrowser: public Expander {
    ObjectStructureView *osv;
public:
    MetaDef(ObjectBrowser);
    ObjectBrowser();
    ~ObjectBrowser();
    void ShowObject(Object *op);
    void Control(int id, int part, void *vp);
};

//---- LabeledGraphReference ------------------------------------------------------------

class LabeledGraphReference: public GraphReference {
    Collection *labels;
public:
    LabeledGraphReference(GraphView *, Object *op, Collection *nodes, 
		   Collection *labels, Ink *i, int width= 2, bool free= FALSE);
    ~LabeledGraphReference();
    void DrawConnection(int n, VObject *from, VObject *to);
};

//---- NestedIter: iterate over a 0 terminated variable argument list of Objects
//                 expanding Collections

class NestedIter : public Iterator {
    Iterator *it, *subIter;
    Collection *cont;
    bool free;
public:
    NestedIter(Object *, Object *, ...);
    NestedIter(Collection *, bool free);
    ~NestedIter();
    ObjPtr operator()();
    Collection *Coll();
    void Reset(Collection *s= 0);
};

#endif OStruBrowser_First
