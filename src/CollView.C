//$CellSelector,CollectionView$
#include "CollView.h"
#include "Menu.h"
#include "String.h"
#include "Collection.h"
#include "VObject.h"
#include "CmdNo.h"
#include "Error.h"
#include "Math.h"
 
//---- CollectionView ----------------------------------------------------------

MetaImpl(CollectionView, (TP(coll), T(selection), T(gap), T(minExtent),
			    T(rows), T(cols), TP(defaultItem), TP(menu), 0));

CollectionView::CollectionView(EvtHandler *eh, Collection *m, CollViewOptions o,
					    int r, int c) : View(eh, gRect0)
{
    SetFlag(o);
    defaultItem= new VObject();
    rows= r;
    cols= c;
    SetCollection(m);
}

CollectionView::~CollectionView()
{
    if (coll) {
	coll->RemoveObserver(this);
	coll->FreeAll();
	SafeDelete(coll);
    }
    SafeDelete(xPos);
    SafeDelete(yPos);
    SafeDelete(defaultItem);
}

//---- init methods

int CollectionView::SetSelection(Rectangle newsel)
{
    if (selection != newsel) {
	if (IsOpen()) {
	    InvalidateRect(ItemRect(selection));
	    InvalidateRect(ItemRect(newsel));
	}
	selection= newsel;
	if (selection.IsEmpty())
	    return 0;   // no selection
	return 1;       // selection changed
    }
    return 2;           // no change
}

void CollectionView::SetCollection(class Collection* m, bool freeold)
{
    if (coll)
	coll->RemoveObserver(this);
    if (freeold && coll) {
	coll->FreeAll();
	delete coll;
    }
    if (coll= m) {
	coll->AssertClass(VObject);
	coll->AddObserver(this);
    }
    selection= gRect0;
    Modified();
    Scroll(cPartScrollAbs, gPoint0, FALSE);
}

void CollectionView::SetDefaultItem(class VObject *d)
{ 
    if (defaultItem)
	delete defaultItem;
    defaultItem= d; 
}

void CollectionView::SetMinExtent(Point e)
{
    if (minExtent != e) {
	minExtent= e;
	Modified();
    }
}

Metric CollectionView::GetMinSize()
{
    Update();
    return GetExtent();
}

//---- layout

void CollectionView::ConstrainScroll(Point *p)
{
    bool outside;
    Point pp= PointToItem(*p, &outside);
    if (! outside)
	p->y= ItemRect(0, pp.y).origin.y;
}

void CollectionView::SetOrigin(Point at)
{
    register int x, y;
    
    View::SetOrigin(at);
    at+= gap;
    for (x= 0; x < cols; x++)
	for (y= 0; y < rows; y++)
	    GetItem(x, y)->SetOrigin(Point(xPos[x]+at.x, yPos[y]+at.y));
}

void CollectionView::Update()
{
    register VObject *gop;
    register int x, y, ww, hh, w, h;
    int xpos= 0, ypos= 0, sz;
    Rectangle r;
    Point g;

    if (!TestFlag(eCVModified))
	return;
    ResetFlag(eCVModified);
    if (coll == 0) {
	ForceRedraw();
	return;
    }
    sz= coll->Size();
    if (rows <= 0)
	SetFlag(eCVExpandRows);
    if (cols <= 0)
	SetFlag(eCVExpandCols);
    
    if (TestFlag(eCVExpandRows) && TestFlag(eCVExpandCols)) {
	cols= intsqrt(sz);
	rows= (sz+cols-1) / cols;
    } else if (TestFlag(eCVExpandRows))
	rows= (sz+cols-1) / cols;
    else if (TestFlag(eCVExpandCols))
	cols= (sz+rows-1) / rows;

    yPos= (short*) Realloc(yPos, (rows+1) * sizeof(short));
    xPos= (short*) Realloc(xPos, (cols+1) * sizeof(short));

    xpos= ypos= 0;
    g= 2*gap;
    if (TestFlag(eCVGrid))
	g+= gPoint1;
    
    for (x= 0; x < cols; x++) {
	xPos[x]= xpos;
	ww= minExtent.x;
	for (y= 0; y < rows; y++) {
	    gop= GetItem(x, y);
	    gop->SetContainer(this);
	    gop->CalcExtent();
	    w= gop->Width();
	    ww= max(ww, w);
	}
	xpos+= ww+g.x;
    }
    xPos[x]= xpos;

    for (y= 0; y < rows; y++) {
	yPos[y]= ypos;
	hh= minExtent.y;
	for (x= 0; x < cols; x++) {
	    h= GetItem(x, y)->Height();
	    hh= max(hh, h);
	}
	ypos+= hh+g.y;
    }
    yPos[y]= ypos;

    for (x= 0; x < cols; x++)
	for (y= 0; y < rows; y++)
	    GetItem(x, y)->SetContentRect(ItemRect(x, y).Inset(gap), FALSE);

    if (TestFlag(eCVGrid)) {
	xpos--;
	ypos--;
    }
    SetExtent(Point(xpos, ypos));
    ForceRedraw();
}

//---- update

void CollectionView::DoObserve(int, int, void *, Object *op)
{
    if (op == coll)
	Modified();
}

void CollectionView::Modified()
{
    SetFlag(eCVModified);
    if (IsOpen())
	Update();
}

//---- mapping

Point CollectionView::PointToItem(Point p, bool *outside)
{
    register int x, y;
    register bool out= FALSE;

    if (coll == 0 || coll->Size() <= 0) {
	if (outside)
	    *outside= TRUE;
	return gPoint0;
    }
    p-= GetOrigin();
    if (p.x < xPos[0]) {
	out= TRUE;
	x= 0;
    } else if (p.x >= xPos[cols]) {
	out= TRUE;
	x= cols-1;
    } else {
	for (x= 0; x < cols; x++)
	    if (p.x >= xPos[x] && p.x < xPos[x+1])
		break;
    }
    if (p.y < yPos[0]) {
	out= TRUE;
	y= 0;
    } else if (p.y >= yPos[rows]) {
	out= TRUE;
	y= rows-1;
    } else {
	for (y= 0; y < rows; y++)
	    if (p.y >= yPos[y] && p.y < yPos[y+1])
		break;
    }
    if (outside)
	*outside= out;
    return Point(x, y);
}

Rectangle CollectionView::ItemRect(Rectangle r)
{
    Rectangle rr;

    if (r.IsEmpty())
	return gRect0;
    Update();
    rr.origin.x= xPos[r.origin.x];
    rr.origin.y= yPos[r.origin.y];
    rr.extent.x= xPos[r.origin.x+r.extent.x] - rr.origin.x;
    rr.extent.y= yPos[r.origin.y+r.extent.y] - rr.origin.y;
    if (TestFlag(eCVGrid))
	rr.extent-= gPoint1;
    return rr+GetOrigin();
}

Point CollectionView::ItemPos(VObjPtr g)
{
    VObjPtr gop;
    Iter next(coll);

    Update();
    for (int i= 0; gop= (VObject*) next(); i++)
	if (gop == g)
	    break;
    if (i >= coll->Size())
	return gPoint_1;
    return Point(i/rows, i%rows);
}

Rectangle CollectionView::ItemRect(int x, int y)
{
    x= range(0, cols, x);
    y= range(0, rows, y);
    Rectangle r(xPos[x], yPos[y], xPos[x+1]-xPos[x], yPos[y+1]-yPos[y]);
    if (TestFlag(eCVGrid))
	r.extent-= gPoint1;
    r.origin+= GetOrigin();
    return r;
}

VObject *CollectionView::GetItem(int x, int y)
{
    VObject *gop= 0;
    int ix= x*rows+y;
    
    if (ix >= 0 && ix < coll->Size())
	gop= (VObject*)coll->At(x*rows+y);
    return gop ? gop : defaultItem;
}

//---- menus

Menu *CollectionView::GetMenu()
{
    if (menu)
	return menu;
    return View::GetMenu();
}

void CollectionView::DoCreateMenu(Menu *m)
{
    if (menu == 0)
	View::DoCreateMenu(m);
}

//---- event handlung

Command *CollectionView::DoLeftButtonDownCommand(Point lp, Token t, int clicks)
{
    if (coll && coll->Size() > 0)
	return new CellSelector(this, clicks);
    return View::DoLeftButtonDownCommand(lp, t, clicks);
}

Command *CollectionView::DoKeyCommand(int ch, Point, Token)
{
    register VObject *gop= 0;
    register int i;
    int start= 0;
    Rectangle r= GetViewedRect();
    
    if (r.IsNotEmpty()) {
	start= r.origin.y+r.extent.y;
	if (start <= GetExtent().y)
	    start= PointToItem(Point(0,start)).y;
    }

    for (i= 0; i < rows; i++) {
	gop= GetItem(0, (start+i) % rows);
	if (gop && (sortmap[gop->AsString()[0]] == sortmap[ch]))
	    break;
    }

    if (gop)
	RevealAlign(ItemRect(Rectangle(ItemPos(gop), gPoint1)));
    return gNoChanges;
}

void CollectionView::DoOnItem(int m, VObject *vp, Point p)
{
    if (vp)
	vp->DoOnItem(m, 0, p);
}

void CollectionView::DoSelect2(Rectangle, int clickCount)
{
    if (GetSelection().IsNotEmpty())
	DoSelect(GetSelection(), clickCount);
}

void CollectionView::DoSelect(Rectangle r, int clicks)
{
    if (r.IsNotEmpty()) {
	int partcode= clicks >= 2 ? cPartCollDoubleSelect: cPartCollSelect;
	Control(GetId(), partcode, (void*) r.origin.y);
	if (TestFlag(eCVClearSelection))
	    SetNoSelection();
    }
}

//---- drawing

void CollectionView::Draw(Rectangle r)
{
    register int x, y;
    register VObject *gop;
    Point p1, p2;

    if (coll == 0 || coll->Size() <= 0)
	return;
    p1= PointToItem(r.NW());
    p2= PointToItem(r.SE()+gPoint1);
    int i= 0;
    for (x= p1.x; x <= p2.x; x++) {
	for (y= p1.y; y <= p2.y; y++) {
	    i++;
	    gop= GetItem(x, y);
	    gop->DrawAll(gop->contentRect,
			selection.ContainsPoint(Point(x, y)) && gop->Enabled());
	}
    }
    if (TestFlag(eCVGrid)) {
	GrSetPenNormal();
	DrawGrid(p1, p2);
    }
}

void CollectionView::DrawGrid(Point p1, Point p2)
{
    register int x, y;
    int xx= 0, yy= 0;
    Point o= GetOrigin();

    if (p2.x >= cols-1)
	xx= 1;
    if (p2.y >= rows-1)
	yy= 1;
    for (x= p1.x; x <= p2.x-xx; x++)
	GrLine(Point(xPos[x+1]-1, yPos[p1.y])+o, Point(xPos[x+1]-1, yPos[p2.y+1]-1)+o);
    for (y= p1.y; y <= p2.y-yy; y++)
	GrLine(Point(xPos[p1.x], yPos[y+1]-1)+o, Point(xPos[p2.x+1]-1, yPos[y+1]-1)+o);
}

//---- input/output

ostream& CollectionView::PrintOn(ostream &s)
{
    Object::PrintOn(s);
    return s << gap SP << minExtent SP << rows SP << cols SP << defaultItem SP << coll SP;
}

bool CollectionView::PrintOnWhenObserved(Object *from)
{
    return from != coll;
}

istream& CollectionView::ReadFrom(istream &s)
{
    Collection *m;
    VObject *di;

    Object::ReadFrom(s);
    s >> gap >> minExtent >> rows >> cols >> di >> m;
    SetCollection(m);
    SetDefaultItem(di);
    return s;
}

void CollectionView::Parts(Collection* col)
{
    View::Parts(col);
    if (coll && coll->Size() < 12) // hack
	col->Add(coll);
}

//---- CellSelector ------------------------------------------------------------

CellSelector::CellSelector(CollectionView* v, int clicks)
{ 
    SetFlag(eCmdNoReplFeedback);
    lvp= v;
    clickCount= clicks;
}

void CellSelector::TrackFeedback(Point, Point pp, bool on)
{
    if (on) {
	int code;
	if (itemptr && itemptr->Enabled())
	    code= lvp->SetSelection(Rectangle(item, gPoint1));
	else
	    code= lvp->SetSelection(gRect0);
	lvp->DoOnItem(code, itemptr, pp);
    }
}

Command *CellSelector::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    bool outside= FALSE;
    
    item= lvp->PointToItem(np, &outside);
    if (outside && lvp->TestFlag(eCVDontStuckToBorder))
	itemptr= 0;
    else
	itemptr= lvp->GetItem(item.x, item.y);
	
    switch (atp) {
    case eTrackRelease:
	lvp->DoSelect2(lvp->GetSelection(), clickCount);
    case eTrackExit:
	return gNoChanges;
    default:
	break;
    }
    return this;
}

