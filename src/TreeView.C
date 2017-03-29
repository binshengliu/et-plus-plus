//$TreeNode,TreeView,TreeNodeSelector$
#include "TreeView.h"
#include "Menu.h"

#include "String.h"
#include "OrdColl.h"

void Indent(ostream &s, int l)
{
    for (int i= 0; i<l; i++)
	s << " ";
}

//---- TreeNode ----------------------------------------------------------------

MetaImpl(TreeNode, (TP(treeview), 0));

TreeNode::TreeNode(int id,  Collection *cp) : CompositeVObject(id, cp)
{
}

TreeNode::TreeNode(int va_(id), ...) : CompositeVObject(va_(id), (Collection*)0)
{
    va_list ap;
    va_start(ap,va_(id));
    SetItems(ap);
    va_end(ap);
}

TreeNode::TreeNode(int id, va_list ap) : CompositeVObject(id, ap)
{
}

void TreeNode::SetOrigin(Point at)
{
    Iter next(list);
    VObject *dip, *first;

    VObject::SetOrigin(at);
    first= (VObject*) next();

    if (Collapsed() || Size() == 1)
	first->SetOrigin(at);
    else {
	switch (Layout()) {
	case eTLTopDown:
	    first->SetOrigin(at+Point((Width()-first->Width())/2, 0));
	    at.y+= Gap().y+first->Height();
	    while (dip= (VObject*) next()) {
		dip->SetOrigin(at);
		at.x+= dip->Width()+Gap().x;
	    }
	    break;
	case eTLIndented:
	case eTLLeftRight:
	    if (Layout() == eTLIndented) {
		first->SetOrigin(at);
		at+= Point(Gap().x, first->Height()+Gap().y);
	    } else {
		first->SetOrigin(at+Point(0, (Height()-first->Height())/2));
		at.x+= Gap().x+first->Width();
	    }
	    while (dip= (VObject*) next()) {
		dip->SetOrigin(at);
		at.y+= dip->Height()+Gap().y;
	    }
	    break;
	}
    }
}

void TreeNode::Open(bool mode)
{
    CompositeVObject::Open(mode);
    GetTreeView()->OpenTreeNode(this, mode);    
}

void TreeNode::Collapse()
{
    Iter next(MakeIterator());
    VObject *vop;
    next();
    while (vop= (VObject*)next())
	vop->Open(Collapsed());
    InvertFlag(eTreeNodeCollapsed);
    Control(cIdNone, cPartLayoutChanged, 0);
}

Metric TreeNode::GetMinSize()
{
    Metric m, m1, mn;
    Iter next(list);
    VObject *dip, *first;

    first= (VObject*) next();

    if (Collapsed() || Size() == 1)
	return first->GetMinSize();
    m1= first->GetMinSize();
    switch(Layout()) {
    case eTLTopDown:
	while (dip= (VObject*) next()) {
	    mn= dip->GetMinSize();
	    m.extent.y= max(m.extent.y, mn.extent.y);
	    m.extent.x+= mn.extent.x+Gap().x;
	}
	m.extent.x-= Gap().x;
	m.extent.x= max(m.extent.x, m1.extent.x);
	m.extent.y+= m1.extent.y+Gap().y;
	m.base= m.extent.y;
	break;
    case eTLIndented:
    case eTLLeftRight:
	while (dip= (VObject*) next()) {
	    mn= dip->GetMinSize();
	    m.extent.x= max(m.extent.x, mn.extent.x);
	    m.extent.y+= mn.extent.y+Gap().y;
	}
	if (Layout() == eTLIndented) {
	    m.extent.y+= m1.extent.y;
	    m.extent.x+= Gap().x;
	    m.extent.x= max(m.extent.x, m1.extent.x);
	    m.base= m1.base;
	} else {
	    m.extent.y-= Gap().y;
	    m.extent.y= max(m.extent.y, m1.extent.y);
	    m.extent.x+= m1.extent.x+Gap().x;
	    m.base= m.extent.y;
	}
	break;
    }
    return m;
}

int TreeNode::Base()
{
    if (Layout() == eTLIndented)
	return At(0)->Base();
    return CompositeVObject::Base();
}

void TreeNode::Draw(Rectangle r)
{
    VObject *sel= GetTreeView()->GetSelection(), *vop;

    if (Collapsed()) {
	vop= At(0);
	vop->DrawAll(r, vop == sel);
	GrStrokeRoundRect(contentRect, gPoint10);
    } else {
	if (ConnType() != eTCNone) {
	    GrSetPenNormal();
	    DrawConnections();
	}
	Iter next(MakeIterator());
	while (vop= (VObject*)next())
	    vop->DrawAll(r, vop == sel);
    }
}

void TreeNode::DrawConnections()
{
    if (Size() == 1)
	return;
    Iter next(list);
    VObject *dip, *first;
    Point p1, p2;
    int l;

    GrSetPenSize(0);
    first= (VObject*) next();

    switch (Layout()) {
    case eTLIndented:
	if (ConnType() == eTCPerpendicular) {
	    l= Gap().x/2;
	    p1= first->contentRect.SW() + Point(l, 0);
	    while (dip= (VObject*) next()) {
		p2= dip->contentRect.origin+Point(0, dip->Base()-5);
		GrLine(p2, p2-Point(l,0));
	    }
	    GrLine(p1, p2-Point(l,0));
	}
	break;
    case eTLTopDown:
	if (ConnType() == eTCPerpendicular || ConnType() == eTCDiagonal2) {
	    l= ConnType() == eTCPerpendicular ? Gap().y/2 : 5;
	    p1= first->contentRect.S();
	    p2= p1+Point(0,l);
	    GrLine(p1, p2);
	    while (dip= (VObject*) next())
		GrLine(dip->contentRect.N(), dip->contentRect.N()-Point(0,l));
	}
	if (ConnType() == eTCDiagonal || ConnType() == eTCDiagonal2) {
	    next.Reset(list);
	    first= (VObject*) next();
	    l= ConnType() == eTCDiagonal ? 0 : 5;
	    p1= first->contentRect.S()+Point(0,l);

	    while (dip= (VObject*) next())
		GrLine(p1, dip->contentRect.N()-Point(0,l));
	} else {
	    l= Gap().y/2;
	    p1= At(1)->contentRect.N()-Point(0,l);
	    p2= At(Size()-1)->contentRect.N()-Point(0,l);
	    GrLine(p1, p2);
	}
	break;
    case eTLLeftRight:
	if (ConnType() == eTCPerpendicular || ConnType() == eTCDiagonal2) {
	    l= ConnType() == eTCPerpendicular ? Gap().x/2 : 5;
	    p1= first->contentRect.E();
	    p2= p1+Point(l,0);
	    GrLine(p1, p2);
	    while (dip= (VObject*) next())
		GrLine(dip->contentRect.W(), dip->contentRect.W()-Point(l,0));
	}
	if (ConnType() == eTCDiagonal || ConnType() == eTCDiagonal2) {
	    next.Reset(list);
	    first= (VObject*) next();
	    l= ConnType() == eTCDiagonal ? 0 : 5;
	    p1= first->contentRect.E()+Point(l,0);

	    while (dip= (VObject*) next())
		GrLine(p1, dip->contentRect.W()-Point(l,0));
	} else {
	    l= Gap().x/2;
	    p1= At(1)->contentRect.W()-Point(l,0);
	    p2= At(Size()-1)->contentRect.W()-Point(l,0);
	    GrLine(p1, p2);
	}
	break;
    }
}

void TreeNode::Outline2(Point p1, Point p2)
{
    Rectangle r= NormRect(p1, p2);
    if (Collapsed())
	GrStrokeRoundRect(r, gPoint10);
    else if (Layout() == eTLIndented) {
	Point sw= At(0)->contentRect.SW()+(r.origin-contentRect.origin);
	int gap= Gap().x;

	GrLine(r.NW(), r.NE());
	GrLine(r.NE()+Point(0,1), r.SE());
#ifdef sun
	GrLine(r.SE()-Point(1,0), r.SW()+Point(gap,0));
#else
	Point pp= r.SW()+Point(gap,0);
	GrLine(r.SE()-Point(1,0), pp);
#endif
	GrLine(r.SW()+Point(gap,-1), r.NW()+Point(gap, At(0)->Height()));
	GrLine(sw+Point(gap-1,0), sw);
	GrLine(sw-Point(0,1), r.NW());
    } else
	CompositeVObject::Outline2(p1, p2);
}

void TreeNode::SetContainer(VObject *vop)
{
    CompositeVObject::SetContainer(vop);
    treeview= (TreeView*)GetView();     // ????
}

void TreeNode::Highlight(HighlightState hst)
{
    At(0)->Highlight(hst);
}

Command *TreeNode::DispatchEvents(Point lp, Token t, Clipper *vf)
{
    if (At(0)->ContainsPoint(lp))
	return VObject::DispatchEvents(lp, t, vf);
    if (Collapsed())
	return 0;
    return CompositeVObject::DispatchEvents(lp, t, vf);
}

Command *TreeNode::DoMiddleButtonDownCommand(Point, Token t, int)
{
    if (t.Flags & eFlgShiftKey) 
	treeview->Promote(this);
    else
	treeview->Collapse(this);
    return gNoChanges;
}

Command *TreeNode::DoLeftButtonDownCommand(Point, Token, int cl)
{
    return treeview->GetNodeSelector(this, cl);
}
    
GrCursor TreeNode::GetCursor(Point lp)
{
    if (Collapsed() && At(0)->ContainsPoint(lp))
	return eCrsBoldCross;
    return CompositeVObject::GetCursor(lp);
}

void TreeNode::Export(ostream &s, int level)
{    
    Iter next(list);
    TreeNode *tn;

    tn= (TreeNode*) next();
    if (tn) {
	Indent(s, level);
	s << tn->AsString() NL;
	while (tn= (TreeNode*) next()) {
	    if (tn->IsKindOf(TreeNode))
		tn->Export(s, level+2);
	    else {
		Indent(s, level+2);
		s << tn->AsString() NL;
	    }
	}
    }
}

VObject *TreeNode::Detect(BoolFun f, void *arg)
{
    if (!Collapsed()) {
	VObject *vop= CompositeVObject::Detect(f, arg);
	if (vop && !vop->IsKindOf(TreeNode))
	    return vop;
    }
    return 0;
}

VObject *TreeNode::FindExpandedItem(VObject *vop)
{
    VObject *op, *found= 0;
    
    Iter next(list);    
    while ((op= (VObject*)next()) && !found) {
	if (op->IsKindOf(TreeNode)) 
	    found= ((TreeNode*)op)->FindExpandedItem(vop);
	else
	    found= op->FindItem(vop);
	if (Collapsed() && found)
	    found= At(0);
    }
    return found;
}

//---- TreeNodeSelector ----------------------------------------------------------------

TreeNodeSelector::TreeNodeSelector(TreeNode *s, int cl)
{
    item= s;
    lastinside= FALSE;
    clicks= cl;
}

void TreeNodeSelector::TrackFeedback(Point, Point, bool)
{
    VObject *image= item->At(0);
    if (item && (lastinside != inside)) {
	item->At(0)->Outline(0);
	lastinside= inside;
    }
}

Command *TreeNodeSelector::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    VObject *image= item->At(0);
    inside= image->ContainsPoint(np);
    if (atp == eTrackRelease) {
	if (item && lastinside) {
	    image->Outline(0);
	    item->GetTreeView()->SetSelection(image);
	    return item->GetTreeView()->NodeSelected(item->At(0), clicks);
	}                
    }
    return this;
}

//---- TreeView ----------------------------------------------------------------

MetaImpl(TreeView, (TE(layout), TE(connType), TP(tree), TP(selection), 0));

TreeView::TreeView(EvtHandler *eh, TreeLayout lt, TreeConnection tc) 
								: DialogView(eh)
{
    connType= tc;
    layout= lt;
    gap= Point(30, 4);
    tree= oldtree= 0;
    selection= 0;
}

VObject *TreeView::DoCreateDialog()
{
    return new TextItem("root");
}

void TreeView::SetLayout(TreeLayout tl)
{
    layout= tl;
    switch (layout) {
    case eTLIndented:
	gap= Point(20, 2);
	break;
    case eTLTopDown:
	gap= Point(10, 40);
	break;
    case eTLLeftRight:
	gap= Point(30, 4);
	break;
    }
    CalcLayout();
}

void TreeView::SetConnType(TreeConnection ct)
{
    connType= ct;
    CalcLayout();
}

void TreeView::SetTree(TreeNode *t, bool freeold)
{
    if (freeold && tree) {
	tree->FreeAll();
	delete tree;
    }
    SetDialog(tree= t);
    oldtree= 0;
}

void TreeView::Promote(TreeNode *newt)
{
    if (oldtree != 0 && newt == tree) {
	SetTree(oldtree, FALSE);
	oldtree= 0;
    } else {
	if (oldtree == 0)
	    oldtree= tree;
	SetTree(newt, FALSE);
    }
    ForceRedraw();
}

void TreeView::Collapse(TreeNode *tn)
{
    tn->Collapse();
}

void TreeView::OpenTreeNode(TreeNode*, bool)
{
}

ostream &TreeView::PrintOn(ostream &s)
{
    return s << layout SP << connType SP << gap SP << tree SP;
}

istream &TreeView::ReadFrom(istream &s)
{
    s >> Enum(layout) >> Enum(connType) >> gap >> tree;
    SetTree(tree);
    return s;
}

void TreeView::Export(ostream &s)
{
    if (tree)
	tree->Export(s, 0);
}

VObject *TreeView::FindNode(VObject *gp)
{
    if (tree)
	return tree->FindExpandedItem(gp);
    return 0;
}

Command *TreeView::GetNodeSelector(TreeNode *tn, int cl)
{
    return new TreeNodeSelector(tn, cl);
}

Command *TreeView::NodeSelected(VObject* vop, int cl)
{
    int partcode= cl >= 2 ? cPartTreeDoubleSelect: cPartTreeSelect;
    Control(GetId(), partcode, (void*) vop);
    return gNoChanges;
}

void TreeView::SetSelection(VObject *vop)
{
    if (vop != selection) {
	if (selection)
	    selection->ForceRedraw();
	selection= vop;
	if (selection) 
	    selection->ForceRedraw();
    }
}

VObject *TreeView::BuildTree(Object *op)
{
    VObject *g= NodeAsVObject(op);
    Collection *list= new OrdCollection;
    list->Add(g);
    Iterator *it= MakeChildrenIter(op);
    if (it) {
	Iter next(it); 
	while (op= next()) {
	    VObject *newitem= BuildTree(op);
	    if (!newitem->IsKindOf(TreeNode))
		newitem= new TreeNode(5, newitem, 0);
	    list->Add(newitem);
	}
    }
    g= new TreeNode(4, list);
    return g;
}

VObject *TreeView::NodeAsVObject(Object *)
{
    AbstractMethod("AsVObject");
    return 0;  
}

Iterator *TreeView::MakeChildrenIter(Object *)
{
    AbstractMethod("GetSubPartsIter");
    return 0;  
}

void TreeView::InstallTree(Object *root)
{
    SetTree((TreeNode*)BuildTree(root));    
}
