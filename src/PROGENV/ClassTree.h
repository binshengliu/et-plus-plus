#ifndef ClassTree_First
#define ClassTree_First

#include "TreeView.h"
#include "Expander.h"

//---- ClassTreeView ---------------------------------------------------------

class ClassTreeView: public TreeView {
    class Menu *menu;
    class Collection *classes;
    Collection *references;
public:
    MetaDef(ClassTreeView);
    ClassTreeView(EvtHandler *, Class *root);
    ~ClassTreeView();

    Command *GetNodeSelector(TreeNode *tn, int cl);
    void OpenTreeNode(TreeNode *, bool mode);

    void ShowClass(Class *cl);
    void ShowAllClasses();
    void ShowOnlyAbstractClasses();
    void ShowOnlyApplicationClasses();
    void InspectSomeInstance(class ClassItem *);

    class ClassItem *FindClassItem(Class *);
    Class *GetRootClass();
    TreeNode *FindTreeNode(VObject *selection);
    void Draw(Rectangle);

    //---- construction
    VObject *NodeAsVObject(Object *);
    Iterator *MakeChildrenIter(Object *);

    //---- menu
    Menu *GetMenu();
    void DoCreateMenu(Menu *menu);
    void DoSetupMenu(Menu *menu);
    Command *DoMenuCommand(int cmd);
    
    //---- references
    void AddReference(class ClassReference *);
    void RemoveAllReferences();
    void ShowMembers(ClassItem *);
    void ShowAllClients(ClassItem *);
};

//---- HierarchyBrowser ----------------------------------------------------

class HierarchyBrowser: public Expander {
    ClassTreeView *ct;
public:
    MetaDef(HierarchyBrowser);
    HierarchyBrowser(Class *root);
    ~HierarchyBrowser();
    void ShowClass(Class *cp);
    void UpdateButtons();
    void Control(int id, int part, void *vp);
};

//---- NameTreeNodeSelector -------------------------------------------------------------

class NameTreeNodeSelector: public Command {
    TreeNode *item;
    bool inside, lastinside;
    int clicks;
    char *name;
    Point p;
    Font *fp;
public:
    NameTreeNodeSelector(TreeNode *s, int cl);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

//---- ClassReference ------------------------------------------------------------------

class ClassReference: public Object {
    class ClassItem *cl;
    class Collection *nodes;
    int width;
    GrPattern pat;
public:
    ClassReference(ClassItem *cl, Collection *n, GrPattern pat= ePatBlack, int width= 1);
    ~ClassReference();
    Rectangle BBox();
    void Draw(Rectangle);
    bool Includes(ClassItem *);
    virtual void DrawConnection(ClassItem *from, ClassItem *to);
    virtual void CircularReference(ClassItem *cl);
    virtual void SetWidthAndPattern(ClassItem *from, ClassItem *to);
};

class MemberReference: public ClassReference {
public:
    MemberReference(ClassItem *cl, Collection *n);
    void SetWidthAndPattern(ClassItem *from, ClassItem *to);
};
    
#endif ClassTree_First

