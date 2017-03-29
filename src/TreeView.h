#ifndef TreeView_First
#ifdef __GNUG__
#pragma once
#endif
#define TreeView_First

#include "Dialog.h"

enum TreeLayout {
    eTLIndented,
    eTLTopDown,
    eTLLeftRight
};

enum TreeConnection {
    eTCNone,
    eTCDiagonal,
    eTCPerpendicular,
    eTCDiagonal2
};

//---- TreeView ----------------------------------------------------------------

class TreeView: public DialogView {
public:
    Point gap;
    TreeLayout layout;
    TreeConnection connType;
    class TreeNode *tree, *oldtree;
    VObject *selection;
public:
    MetaDef(TreeView);

    TreeView(EvtHandler*, TreeLayout lt= eTLLeftRight, 
				    TreeConnection ct= eTCPerpendicular);

    VObject *DoCreateDialog();
    void SetTree(TreeNode*, bool freeold= TRUE);
    void Promote(TreeNode*);
    void Collapse(TreeNode*);
    virtual void OpenTreeNode(TreeNode*, bool);
    VObject *FindNode(VObject *);
    TreeNode *GetTree()
	{ return tree; }

    //---- layout
    void SetLayout(TreeLayout);
    void SetConnType(TreeConnection);

    //---- event handling
    virtual Command *NodeSelected(VObject*, int cl);
    virtual Command *GetNodeSelector(TreeNode *tn, int cl);

    //---- input/output
    ostream &PrintOn(ostream&);
    istream &ReadFrom(istream&);
    void Export(ostream&);
    
    //---- hooks for BuildTree
    void InstallTree(Object *root);
    VObject *BuildTree(Object *root);
    virtual VObject *NodeAsVObject(Object *);
    virtual Iterator *MakeChildrenIter(Object *);

    //---- selection
    void SetSelection(VObject *gp);
    VObject *GetSelection()
	{ return selection; }
};

//---- TreeNode ------------------------------------------------------------------

enum TreeNodeFlags {
    eTreeNodeCollapsed  =   BIT(eCompVObjLast+1),
    eTreeNodeDefault    =   eCompVObjDefault,
    eTreeNodeLast       =   eCompVObjLast+1
};

class TreeNode: public CompositeVObject {
    TreeView *treeview;
public:
    MetaDef(TreeNode);

    TreeNode(int id= cIdNone, Collection *cp= 0);
    TreeNode(int id, ...);
    TreeNode(int id, va_list ap);

    void SetContainer(VObject*);
    TreeView *GetTreeView()
	{ return treeview; }
    bool Collapsed()
	{ return TestFlag(eTreeNodeCollapsed); }
    void Open(bool);
    void Collapse();
    TreeConnection ConnType()
	{ return treeview->connType; }
    TreeLayout Layout()
	{ return treeview->layout; }
    Point Gap()
	{ return treeview->gap; }
    VObject *Detect(BoolFun f, void *arg);
    VObject *FindExpandedItem(VObject*);

    //---- layout
    void SetOrigin(Point at);
    Metric GetMinSize();
    int Base();

    //---- drawing
    void Draw(Rectangle);
    void DrawConnections();
    void Outline2(Point, Point);
    void Highlight(HighlightState hst);
    GrCursor GetCursor(Point lp);

    //---- event handling
    Command *DispatchEvents(Point, Token, Clipper*);
    Command *DoMiddleButtonDownCommand(Point lp, Token t, int cl);
    Command *DoLeftButtonDownCommand(Point lp, Token t, int cl);

    //---- input/output
    void Export(ostream &s, int level);
};

//---- TreeNodeSelector -------------------------------------------------------------

class TreeNodeSelector: public Command {
    TreeNode *item;
    bool inside, lastinside;
    int clicks;
public:
    TreeNodeSelector(TreeNode *s, int cl);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

#endif TreeView_First

