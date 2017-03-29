//$GraphView,GraphNode,GraphNodeMover,GraphPath,GraphReference$

#include "ObjList.h"
#include "IdDictionary.h"
#include "OrdColl.h"
#include "Dialog.h"
#include "GraphView.h"

//---- GraphNode -------------------------------------------------------------

MetaImpl(GraphNode, (TP(constrainingNodes), TP(nonTreeOutEdges), T(inEdges), 0));

GraphNode::GraphNode(int id, Collection *cp) : TreeNode(id, cp)
{ 
    constrainingNodes= 0; 
    nonTreeOutEdges= 0;
    inEdges= 0; 
}

GraphNode::~GraphNode()
{
    SafeDelete(nonTreeOutEdges);
    Object *op= Image();
    if (op) {
	op->FreeAll();
	SafeDelete(op);
    }
}

VObject *GraphNode::Image()
{
    Collection *cp= GetList();
    if (cp)
	return (VObject*)(cp->At(0));
    return 0;
}

int GraphNode::CalcShift()
{
    Point origin= contentRect.origin;
    
    if (InCalcShift()) {
	Warning("CalcShift", "cycle in graph");
	return contentRect.origin.x;
    }
    SetFlag(eGraphInCalc);
    if (!IsPositionSet()) {
	SetFlag(eGraphPosSet); 
	if (constrainingNodes) {
	    Iter next(constrainingNodes->MakeIterator());
	    GraphNode *gp;
	    while (gp= (GraphNode*)next()) 
		origin.x = max(origin.x, gp->CalcShift());
	    //if (origin.x != contentRect.origin.x)
		origin.x += 2*(Gap().x);
	}
	SafeDelete(constrainingNodes);
    }
    if (origin != contentRect.origin)
	SetOrigin(origin);   
	    
    ResetFlag(eGraphInCalc);
    return contentRect.origin.x+contentRect.extent.x;
}

void GraphNode::AddNonTreeOutEdge(GraphNode *gp)
{
    if (nonTreeOutEdges == 0)
	nonTreeOutEdges= new OrdCollection;
    nonTreeOutEdges->Add(gp);
}

void GraphNode::AddConstrainingNode(GraphNode *gp)
{
    if (constrainingNodes == 0)
	constrainingNodes= new OrdCollection;
    constrainingNodes->Add(gp);
}

void GraphNode::Draw(Rectangle r)
{
    Image()->DrawAll(r, FALSE);
}

void GraphNode::DrawConnections()
{
    TreeNode::DrawConnections();
    if (nonTreeOutEdges) {
	VObject *image= Image();
	Point p1= image->contentRect.E();
	Iter next(nonTreeOutEdges);
	GraphNode *gp;
	while (gp= (GraphNode*)next()) 
	    GrLine(p1, gp->Image()->contentRect.W());
    }
}

void GraphNode::Export(ostream &s)
{
    // leave node
    if ((GetList()->Size() + (nonTreeOutEdges ? nonTreeOutEdges->Size() : 0)) == 1)
	return;
	
    GraphNode *gp;
    int i= 0;
    if (GetList()) {
	Iter next1(GetList());
	while (gp= (GraphNode*)next1()) {
	    if (i++ == 0) // the image of the node itself
		s << gp->AsString() << ": "; 
	    else
		s << gp->Image()->AsString() SP;
	} 
    } 
    if (nonTreeOutEdges) {
	Iter next2(nonTreeOutEdges);
	while (gp= (GraphNode*)next2()) 
	    s << gp->Image()->AsString() SP;
    }
    s NL;
}

//---- GraphNodeMover -------------------------------------------------------------

class GraphNodeMover: public Command {
    GraphView *gvp;
    GraphNode *gnp;
    VObject *item;
    GrCursor oldcursor;
    Point delta;
public:
    GraphNodeMover(GraphView *g, GraphNode *v) : Command(cIdNone, "move node")
	{ gvp= g; gnp= v; item= gnp->Image(); delta= 0; }
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
    void DoIt();
    void UndoIt();
};

void GraphNodeMover::TrackFeedback(Point anchorPoint, Point nextPoint, bool)
{
    item->Outline(nextPoint - anchorPoint);
}

Command *GraphNodeMover::TrackMouse(TrackPhase tp, Point ap, Point, Point np)
{
    switch (tp) {
    case eTrackPress:
	oldcursor= GrGetCursor();
	break;
    case eTrackMove:
	GrSetCursor(eCrsMoveHand);
	break;
    case eTrackRelease:
	delta= np-ap;
	GrSetCursor(oldcursor);
	break;
    default:
	break;
    }
    return this;

}

void GraphNodeMover::DoIt()
{
    item->Move(delta);
    gnp->contentRect+= delta;
    gvp->CalcExtent();
    gvp->ForceRedraw();
}

void GraphNodeMover::UndoIt()
{
    item->Move(-delta);
    gnp->contentRect-= delta;
    gvp->CalcExtent();
    gvp->ForceRedraw();
}

//---- GraphView -----------------------------------------------------------------

MetaImpl(GraphView, (TP(nodes), TP(paths), TP(refs), 0));

GraphView::GraphView(EvtHandler *dp) : TreeView(dp)
{
    nodes= new IdDictionary; 
    connType= eTCDiagonal;
    paths= new OrdCollection;
    refs= new OrdCollection;
}

GraphView::~GraphView()
{
    nodes->FreeValues();
    SafeDelete(nodes);
    SafeDelete(paths);
    SafeDelete(refs);
}

void GraphView::Draw(Rectangle r)
{
    DictIterValues next(nodes);
    register GraphNode *np;
    
    View::Draw(r);
    if (connType != eTCNone)
	while (np= (GraphNode*) next()) 
	    np->DrawConnections();
	
    DrawPaths(r);
    DrawReferences(r);
	    
    next.Reset();
    while (np= (GraphNode*) next()) {
	VObject *vp= np->Image();
	if (vp && vp->contentRect.Intersects(r))
	    vp->DrawAll(r, vp == GetSelection());
    }
}

void GraphView::SetConnType(TreeConnection ct)
{
    if (ct != eTCNone && ct != eTCDiagonal)
	return;  
    connType= ct;
    ForceRedraw();  
}

GraphNode *GraphView::FindNode(Point lp)
{
    DictIterValues next(nodes);
    GraphNode *np;
    while (np= (GraphNode *)next()) {
	VObject *vp= np->Image();
	if (vp->ContainsPoint(lp))
	    return np;
    } 
    return 0;  
}

Command *GraphView::DoLeftButtonDownCommand(Point lp, Token, int cl)
{
    GraphNode *np= FindNode(lp);
    if (np)
	return new TreeNodeSelector(np, cl);
    return gNoChanges;  
}

Command *GraphView::DoMiddleButtonDownCommand(Point lp, Token, int)
{
    GraphNode *np= FindNode(lp);
    if (np)
	return new GraphNodeMover(this, np);
    return gNoChanges;  
}

void GraphView::CalcExtent()
{
    DictIterValues next(nodes);
    register GraphNode *np;
    Rectangle r;
    while (np= (GraphNode *)next()) 
	r.Merge(np->Image()->contentRect);
    SetExtent(r.extent+Point(200)); 
}

Iterator *GraphView::MakeSubPartsIter(Object *)
{
    AbstractMethod("MakeSubPartsIter");
    return 0;  
}

Iterator *GraphView::MakeChildrenIter(Object *op)
{
    return MakeSubPartsIter(op);
}

VObject *GraphView::DoCreateRoot()
{
    return new TextItem("Root");
}

VObject *GraphView::DoCreateDialog()
{
    return 0;
}

VObject *GraphView::AssociatedVObject(Object *op)
{
    GraphNode *gp= (GraphNode*) nodes->AtKey(op);
    if (gp)
	return gp->Image();
    return 0;      
}
    
GraphNode *GraphView::AssociatedGraphNode(Object *op)
{
    return (GraphNode*) nodes->AtKey(op);
}

void GraphView::SetGraph(GraphNode *root, bool)
{
    DictIterValues next(nodes);
    GraphNode *np;
    
    while (np= (GraphNode *)next()) {
	np->SetContainer(this);
	np->Enable();
    }
    // calculate tree layout
    root->CalcExtent();
    root->SetOrigin(GetOrigin());
    next.Reset();
    while (np= (GraphNode *)next()) 
	np->CalcShift();
    CalcExtent();
    ForceRedraw();
}

void GraphView::EmptyGraph()
{
    SetSelection(0);
    nodes->FreeValues();
}

GraphNode *GraphView::BuildGraphDFS(Object *np)
{
    Object *subNode;
    GraphNode *gp;
    
    if (gp= (GraphNode*) nodes->AtKey(np)) 
	return gp;
    
    GraphNode *node= new GraphNode;
    node->AddNode(NodeAsVObject(np));
    nodes->AtKeyPut(np, node); // register node in dictionary
    Iterator *it= MakeSubPartsIter(np);
    
    if (it) {
	Iter next(it);
	
	while (subNode= next()) {            
	    // node already occurred hence must be a nonTreeOutEdge
	    if (gp= (GraphNode*) nodes->AtKey(subNode)) { // node already occurred, -> nonTreeOutEdge
		node->AddNonTreeOutEdge(gp);
		gp->AddConstrainingNode(node);
	    } else {
		gp= BuildGraphDFS(subNode);
		node->AddNode(gp);
	    }
	    gp->AddInEdge(node);
	}
    }
    return node;    
}   

void GraphView::BuildGraphBFS(Object *op)
{
    ObjList q;          // queue for breadth first search
    GraphNode *gnp, *snp;
    Object *subnode, *node;
    
    q.Add(op);
    while (q.Size() > 0) {
	node= q.RemoveFirst();
	if ((gnp= (GraphNode *) nodes->AtKey(node)) == 0) {
	    gnp= new GraphNode;
	    nodes->AtKeyPut(node, gnp);
	    gnp->AddNode(NodeAsVObject(node));
	}
	Iterator *it= MakeSubPartsIter(node);
	if (it) {
	    Iter next(it);
	    while (subnode= next()) {
		// node was already visited
		if (snp= (GraphNode*)nodes->AtKey(subnode)) {
		    gnp->AddNonTreeOutEdge(snp);
		    snp->AddConstrainingNode(gnp);
		} else {
		    snp= new GraphNode();
		    snp->AddNode(NodeAsVObject(subnode));
		    gnp->AddNode(snp);
		    nodes->AtKeyPut(subnode, snp);
		    q.Add(subnode);
		}
		snp->AddInEdge(gnp);
	    }
	}
    }
}   

GraphNode *GraphView::FindRoot()
{
    OrdCollection *oc= new OrdCollection();    
    DictIterValues next(nodes);
    GraphNode *np, *root;

    while (np= (GraphNode *)next())
	if (np->NumInEdges() == 0)
	    oc->Add(np);
	    
    if (oc->Size() == 1) {
	root= (GraphNode*)oc->First();
	SafeDelete(oc);
	return root;
    }
    // create artificial root
    VObject *vop= DoCreateRoot();
    oc->AddFirst(vop);
    root= new GraphNode(cIdNone, oc);
    nodes->AtKeyPut(vop, root); 
    return root;
}

void GraphView::DrawPaths(Rectangle r)
{
    Iter next(paths);
    GraphPath *gp;
    while (gp= (GraphPath*)next()) 
	gp->Draw(r);
}

void GraphView::AddPath(GraphPath *p)
{
    paths->Add(p);
    InvalidateRect(p->BBox());
}

GraphPath *GraphView::RemovePath(GraphPath *p)
{
    GraphPath *gp;
    gp= (GraphPath*)paths->RemovePtr(p);
    if (gp)
	InvalidateRect(p->BBox());
    return gp;
}

void GraphView::RemoveAllPaths()
{
    paths->FreeAll();
    paths->Empty(0);
    ForceRedraw();
}

void GraphView::DrawReferences(Rectangle r)
{
    Iter next(refs);
    GraphReference *gp;
    while (gp= (GraphReference*)next()) 
	gp->Draw(r);
}

void GraphView::AddReference(GraphReference *p)
{
    refs->Add(p);
    InvalidateRect(p->BBox());
}

GraphReference *GraphView::RemoveReference(GraphReference *p)
{
    GraphReference *gp;
    gp= (GraphReference*)refs->RemovePtr(p);
    if (gp)
	InvalidateRect(p->BBox());
    return gp;
}

void GraphView::RemoveAllReferences()
{
    refs->FreeAll();
    refs->Empty(0);
    ForceRedraw();
}

//---- GraphPath ------------------------------------------------------------

GraphPath::GraphPath(GraphView *g, Collection *n, Ink *i, int w, bool f)
{
    nodes= n;
    free= f;
    gvp= g;
    width= w;
    ink= i;
}
	    
GraphPath::~GraphPath()
{
    if (free)
	SafeDelete(nodes);
}
    
void GraphPath::Draw(Rectangle)
{
    Object *op;
    VObject *image;
    Iter next(nodes);

    for (int i= 0; op= next(); i++) {
	image= gvp->AssociatedVObject(op);
	if (!image)
	    continue;//Error("Draw", "node in GraphPath without representation");
	GrSetPenNormal();
	GrStrokeRect(image->contentRect.Expand(1));
	GrSetPenSize(width);
	GrSetPenPattern(ink);
	if (i == 0)
	    GrMoveto(image->contentRect.Center());
	else
	    GrLineto(image->contentRect.Center());
    }
}

Rectangle GraphPath::BBox()
{
    Rectangle r;
    Object *op;
    VObject *image;
    Iter next(nodes);
    while (op= next()) 
	if (image= gvp->AssociatedVObject(op))
	    r.Merge(image->contentRect.Expand(4));
    return r;
}

//---- GraphReference ------------------------------------------------------------

GraphReference::GraphReference(GraphView *g, Object *op, Collection *n, Ink *i, int w, bool f)
{
    nodes= n;
    free= f;
    gvp= g;
    width= w;
    ink= i;
    referto= op;
}
	    
GraphReference::~GraphReference()
{
    if (free)
	SafeDelete(nodes);
}
    
void GraphReference::Draw(Rectangle)
{
    Object *op;
    VObject *image, *opimage;

    Iter next(nodes);
    opimage= gvp->AssociatedVObject(referto);
    if (!opimage)
	return;
    GrSetPenNormal();
    GrStrokeRect(opimage->contentRect.Expand(1)); 
       
    for (int i= 0; op= next(); i++) {
	image= gvp->AssociatedVObject(op);
	if (!image)
	    continue;//Error("Draw", "node in GraphPath without representation");
	GrSetPenNormal();
	GrStrokeRect(image->contentRect.Expand(1));
	DrawConnection(i, opimage, image);
    }
}

void GraphReference::DrawConnection(int, VObject *from, VObject *to)
{
	GrSetPenSize(width);
	GrSetPenPattern(ink);
	Point p1= from->contentRect.Center();
	Point p2= to->contentRect.Center();
	GrLine(p1, p2);
}

Rectangle GraphReference::BBox()
{
    Rectangle r;
    Object *op;
    VObject *image, *opimage;
    Iter next(nodes);
  
    opimage= gvp->AssociatedVObject(referto);
    while (opimage && (op= next())) {
	if (image= gvp->AssociatedVObject(op)) {
	    r.Merge(opimage->contentRect.Expand(4));
	    r.Merge(image->contentRect.Expand(4));
	}
    }
    return r;
}
