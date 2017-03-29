#ifndef GraphView_First
#ifdef __GNUG__
#pragma once
#endif
#define GraphView_First

#include "TreeView.h"
#include "Collection.h"

//---- GraphNode ------------------------------------------------------------------

enum GraphNodeFlags {
    eGraphPosSet        =   BIT(eTreeNodeLast+1),
    eGraphInCalc        =   BIT(eTreeNodeLast+2),
    eGraphNodeLast      =   eTreeNodeLast+2
};

class GraphNode: public TreeNode {
    Collection *constrainingNodes;
    Collection *nonTreeOutEdges;
    int inEdges;
public:
    MetaDef(GraphNode);

    GraphNode(int id= cIdNone, Collection *cp= 0);
    ~GraphNode();
    
    //---- construction
    void AddNonTreeOutEdge(GraphNode*);
    void AddConstrainingNode(GraphNode*);
    void AddNode(VObject *vp)
	{ GetList()->Add(vp); }
    void AddInEdge(VObject *)
	{ inEdges++; }
    int NumInEdges()
	{ return inEdges; }
	
    //---- layout
    int CalcShift();   
	// calculate amout to shift a node to the right
    bool InCalcShift()
	{ return TestFlag(eGraphInCalc); }
    bool IsPositionSet()
	{ return TestFlag(eGraphPosSet); }
    VObject *Image();
    
    //---- drawing
    void Draw(Rectangle);
    void DrawConnections();

    //---- input/output
    void Export(ostream &s);
};

//---- GraphView ----------------------------------------------------------------

class GraphView: public TreeView {
    class Collection *paths, *refs;

protected:    
    class IdDictionary *nodes;      // maps objects to GraphNodes
    void DrawPaths(Rectangle r);
    void DrawReferences(Rectangle r);
    VObject *DoCreateDialog();
    Iterator *MakeChildrenIter(Object *op);
public:
    MetaDef(GraphView);
    GraphView(EvtHandler *dp);
    ~GraphView();
    
    //---- construction 
    void EmptyGraph();
    GraphNode *BuildGraphDFS(Object *);
    void BuildGraphBFS(Object *);
    GraphNode *FindRoot();
    void SetGraph(GraphNode *root, bool free=TRUE);
    void CalcExtent();
    
    //---- hooks for BuildGraph/FindRoot
    virtual VObject *DoCreateRoot();
    virtual Iterator *MakeSubPartsIter(Object *);
	
    //---- drawing
    void SetConnType(TreeConnection ct);
    void Draw(Rectangle);

    //---- accessing
    VObject *AssociatedVObject(Object *);
    GraphNode *AssociatedGraphNode(Object *);
    GraphNode *FindNode(Point p);
    
    Command *DoLeftButtonDownCommand(Point lp, Token t, int cl);
    Command *DoMiddleButtonDownCommand(Point lp, Token t, int cl);

    //---- path handling 
    void AddPath(class GraphPath*);
    GraphPath *RemovePath(class GraphPath*);
    void RemoveAllPaths();

    //---- references handling
    void AddReference(class GraphReference*);
    GraphReference *RemoveReference(GraphReference*);
    void RemoveAllReferences();
};

//---- GraphPath ------------------------------------------------------------

class GraphPath: public VObject {
    class Collection *nodes;
    GraphView *gvp;
    int width;
    Ink *ink;
    bool free;
public:
    GraphPath(GraphView *, Collection *nodes, Ink *ink, int width= 2, bool free= FALSE);
    ~GraphPath();
    void Draw(Rectangle);
    Rectangle BBox();
};
    
//---- GraphReference ------------------------------------------------------------

class GraphReference: public VObject {
protected:
    class Collection *nodes;
    Object *referto;
    GraphView *gvp;
    int width;
    Ink *ink;
    bool free;
public:
    GraphReference(GraphView *, Object *op, Collection *nodes, Ink *ink, int width= 2, bool free= FALSE);
    ~GraphReference();
    virtual void Draw(Rectangle);
    virtual void DrawConnection(int n, VObject *from, VObject *to);
    Rectangle BBox();
};

#endif GraphView_First


