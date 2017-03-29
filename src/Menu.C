//$MenuItem,Menu,MenuLineItem,ShadowBorderItem$

#include "Menu.h"
#include "SortedOList.h"
#include "OrdColl.h"
#include "String.h"
#include "BlankWin.h"
#include "WindowSystem.h"

static int cMBorder_x= 4,
	   cMBorder_y= 0;

const int cPullRight= 16;

//---- MenuItem ----------------------------------------------------------------

MetaImpl(MenuItem, (TP(nextMenu), 0));

MenuItem::MenuItem(int id, VObject *g, class Menu *nm)
		    : VObjectPair(g,
			new TextItem("\336", new_Font(eFontSymbol, gSysFont->Size()), Point(cMBorder_x, cMBorder_y)),
			Point(5,0))
{
    SetId(id);
    nextMenu= nm;
    left->SetFlag(eVObjHFixed);
    right->SetFlag(eVObjHFixed);
}

MenuItem::~MenuItem()
{
    SafeDelete(nextMenu);
}

void MenuItem::SetContainer(VObject *v)
{
    VObjectPair::SetContainer(v);
    nextMenu->SetNextHandler(this);
}

void MenuItem::Enable(bool b, bool)
{
    if (nextMenu && !b)
	nextMenu->DisableAll();
}

VObject *MenuItem::Detect(BoolFun find, void *arg)
{
    if (VObject::Detect(find, arg))
	return this;
    if (nextMenu)
	return nextMenu->Detect(find, arg);
    return 0;
}

void MenuItem::DoOnItem(int m, VObject*, Point p)
{
    int w= right->Width();
    if (m == 2 && nextMenu && Enabled() && ContainsPoint(p)) {
	if (p.x > lastxpos && 
		(p.x > enterxpos + cPullRight || p.x >contentRect.SE().x-w)) {
	    p.y= contentRect.Center().y + 8;
	    nextMenu->Show(p, this);
	    lastxpos= contentRect.SE().x+1;
	} else
	    lastxpos= p.x;
    } else {
	lastxpos= contentRect.SE().x+1;
	enterxpos= p.x;
    }
}

ostream& MenuItem::PrintOn(ostream &s)
{
    VObjectPair::PrintOn(s);
    return s << nextMenu SP;
}

istream& MenuItem::ReadFrom(istream &s)
{
    VObjectPair::ReadFrom(s);
    return s >> nextMenu;
}

void MenuItem::Parts(Collection* col)
{
    VObjectPair::Parts(col);
    col->Add(ContMenu());
}

//---- MenuLineItem ------------------------------------------------------------

MetaImpl0(MenuLineItem);

MenuLineItem::MenuLineItem() : LineItem(TRUE, 1, 20, cIdNone)
{
    ResetFlag(eVObjEnabled);
}

void MenuLineItem::Enable(bool, bool)
{
    ResetFlag(eVObjEnabled);
}

//---- Menu --------------------------------------------------------------------

MetaImpl(Menu, (TP(title), TP(clipper), T(selection), TP(window), 0));

Menu::Menu(char *t, bool s, int r, int c, bool st)
			    : CollectionView(0, 0, eCVDontStuckToBorder, r, c)
{
    Init(new TextItem(t), s, st);
}

Menu::Menu(VObject *t, bool s, int r, int c, bool st)
			    : CollectionView(0, 0, eCVDontStuckToBorder, r, c)
{
    Init(t, s, st);
}

Menu::~Menu()
{
    SafeDelete(window);
}

void Menu::Init(VObject *t, bool s, bool st)
{
    if (s)
	SetCollection(new SortedObjList);
    else
	SetCollection(new OrdCollection);
    title= t;
    SetFlag(eMenuIsNew);
    selection= cIdNone;
    level= 0;
    window= 0;
    if (st)
	SetFlag(eMenuTitle);
}

Point Menu::InitialPos()
{
    Rectangle s= GetSelection();
    if (s.IsEmpty())
	s= gPoint0;
    Point initialPos= ItemRect(s.origin.x, s.origin.y).W()+Point(4,0);
    // clipper->Scroll(cPartScrollAbs, initialPos, FALSE); // CSE
    return initialPos;
}

void Menu::Update()
{
    if (TestFlag(eCVModified)) {
	CollectionView::Update();
	if (window == 0) {
	    VObject *bal;
	    clipper= new Clipper(this);
	    if (TestFlag(eMenuTitle))
		bal= new ShadowBorderItem(title, clipper);
	    else
		bal= new ShadowBorderItem(clipper, 1);
	    window= new BlankWin(bal, gPoint_1, 
		(BWinFlags)(eBWinOverlay+eBWinBlock), ePatNone, eCrsRightArrow);
	    window->contentRect.extent= gPoint0;
	    window->Update();
	}
	window->contentRect.extent= gPoint0;
	//if (title && TestFlag(eMenuTitle))
	//    SetMinExtent(Point(window->GetMinSize().extent.x, 0));
	if (! TestFlag(eMenuNoScroll))
	    clipper->SetMinExtent(ItemRect(2,15).origin);
    }
}

int Menu::Show(Point p, VObject *fp)
{
    level++;
    ResetFlag(eMenuIsNew);
    SetFlag(eCVModified);
    /*
    if (title && TestFlag(eMenuTitle))
	SetMinExtent(Point(title->GetMinSize().extent.x + 2*10, 0));
    */
    Update();
    int oldselection= selection;
    
    Token t(eEvtLeftButton, eFlgNone, clipper->GetPortPoint(InitialPos()));
    window->PushBackEvent(t);
    window->OpenAt(p-t.Pos, fp);
    level--;
    if (selection == cIdNone) {
	SetSelectedItem(oldselection);
	return cIdNone;
    }
    return selection;
}

void Menu::Close()
{
    window->Close();
}

void Menu::DoOnItem(int m, VObject *vop, Point p)
{
    if (level > 1 && (p.x < 3 || p.y < -15))
	Close();
    else
	CollectionView::DoOnItem(m, vop, p);
}

void Menu::DoSelect2(Rectangle sel, int)
{
    selection= cIdNone;
    if (sel.IsNotEmpty()) {
	VObject *mi= GetItem(GetSelection().origin.x, GetSelection().origin.y);
	if (mi->Enabled()) {
	    if (mi->IsKindOf(MenuItem))
		selection= ((MenuItem*)mi)->ContMenu()->GetSelectedId();
	    else
		selection= mi->GetId();
	}
    }
    Close();
}

void Menu::Insert(VObject *mip)
{
    Guard(GetCollection(), SeqCollection)->AddFirst(mip);
}

void Menu::InsertItem(char *s, int id)
{
    Insert(MakeMenuItem(s, id));
}

void Menu::Append(VObject *mip)
{
    GetCollection()->Add(mip);
}

void Menu::InsertBefore(int id, VObject *mip)
{
    VObject *vop= FindItem(id);
    SeqCollection *col= Guard(GetCollection(), SeqCollection);

    if (vop)
	col->InsertBeforePtr(vop, mip);
    else
	col->AddFirst(mip);
}

void Menu::InsertAfter(int id, VObject *mip)
{
    VObject *vop= FindItem(id);
    SeqCollection *col= Guard(GetCollection(), SeqCollection);

    if (vop)
	col->InsertAfterPtr(vop, mip);
    else
	GetCollection()->Add(mip);
}

void Menu::InsertItemAfter(int atId, char *s, int id)
{
    InsertAfter(atId, MakeMenuItem(s, id));
}

void Menu::InsertItemBefore(int atId, char *s, int id)
{
    InsertBefore(atId, MakeMenuItem(s, id));
}

void Menu::AppendItem(char *s, int id)
{
    GetCollection()->Add(MakeMenuItem(s, id));
}

void Menu::AppendItems(char *va_(first), ...)
{
    va_list ap;
    va_start(ap,va_(first)); 
    InsertVItems(cIdNone, FALSE, va_(first), ap);
    va_end(ap);
}

void Menu::InsertItemsBefore(int va_(atId), ...)
{
    va_list ap;    
    va_start(ap,va_(atId));
    char *first= va_arg(ap, char*);
    InsertVItems(va_(atId), TRUE, first, ap);
    va_end(ap);
}

void Menu::InsertItemsAfter(int va_(atId), ...)
{
    va_list ap;    
    va_start(ap,va_(atId));
    char *first= va_arg(ap, char*);
    InsertVItems(va_(atId), FALSE, first, ap);
    va_end(ap);
}

VObject *Menu::MakeMenuItem(char *s, int id)
{
    if (strcmp(s, "-") == 0)
	return new MenuLineItem;
    return new TextItem(id, s, gSysFont,
		    Point(cMBorder_x, cMBorder_y));
}

void Menu::InsertVItems(int atId, bool before, char *first, va_list ap)
{
    SeqCollection *col=  Guard(GetCollection(),SeqCollection);
    char *s;
    int id= -1;
    VObject *vop, *atvop= 0;

    if (atId != cIdNone)
	atvop= FindItem(atId);

    for (int i= 0;; i++) {

	if (i == 0)
	    s= first;
	else 
	    if ((s= va_arg(ap, char*)) == 0)
		break;
	    
	if (strcmp(s, "-"))
	    id= va_arg(ap, int);
	vop= MakeMenuItem(s, id);
	if (atvop == 0)
	    col->Add(vop); 
	else if (before && i== 0)
	    col->InsertBeforePtr(atvop, vop);
	else
	    col->InsertAfterPtr(atvop, vop);
	atvop= vop;
    }
}

void Menu::DisableAll()
{
    GetCollection()->ForEach(VObject,Enable)(FALSE, FALSE);
}

void Menu::ReplaceItem(int id, char *s)
{
    TextItem *t= (TextItem*) FindItem(id);

    if (t && t->IsKindOf(TextItem)) {
	t->SetString(s);
	Modified();
    }
}
    
void Menu::ToggleItem(int id, bool toggle, char *toggleTrue, char *toggleFalse)
{
    ReplaceItem(id, toggle ? toggleTrue : toggleFalse);
}

void Menu::ToggleItem(int id, bool toggle, char *fmt, char *toggleTrue, char *toggleFalse)
{
    ReplaceItem(id, form(fmt, toggle ? toggleTrue : toggleFalse));
}

VObject *Menu::RemoveItem(int id)
{
    VObject *vop= FindItem(id);
    if (vop) {
	VObject *cont= vop->GetContainer();
	Menu *m= this;
	if (cont->IsKindOf(Menu))
	    m= (Menu*)cont;    
	m->GetCollection()->RemovePtr(vop);
    }
    return vop;
}

void Menu::RemoveAllItems(bool free)
{
    SetNoSelection();
    Collection *cp= GetCollection();
    if (free)
	cp->FreeAll();
    cp->RemoveAll(cp);
}

VObject *Menu::Detect(BoolFun find, void *arg)
{
    Iter next(GetCollection());
    register VObject *g1, *g2;

    while (g1= (VObject*) next())
	if (g2= g1->Detect(find, arg))
	    return g2;
    return 0;
}

Menu *Menu::FindMenuItem(int id)
{
    VObject *vop= FindItem(id);
    if (!vop || !vop->IsKindOf(MenuItem))
	return 0;
    MenuItem *mip= (MenuItem*)vop;
    return mip->ContMenu();
}

void Menu::EnableItem(int id, bool b)
{
    VObject *vop= FindItem(id);
    if (vop)
	vop->Enable(b, FALSE);
}

void Menu::EnableItems(int va_(first), ...)
{
    va_list ap;
    va_start(ap,va_(first));
    register int id;
    
    EnableItem(va_(first));
    while (id= va_arg(ap, int))
	EnableItem(id);
    va_end(ap);
}

void Menu::SetSelectedItem(int id)
{
    VObject *mi= FindItem(id);
    if (mi) {
	selection= id;
	SetSelection(Rectangle(ItemPos(mi), gPoint1));
    }
}

void Menu::AppendMenu(Menu *m, int id)
{
    char *t= m->GetTitle()->AsString();
    VObject *ti= new TextItem(t, gSysFont, Point(cMBorder_x, cMBorder_y));
    GetCollection()->Add(new MenuItem(id, ti, m));
}

ostream& Menu::PrintOn(ostream &s)
{
    CollectionView::PrintOn(s);
    return s << title SP;
}

istream& Menu::ReadFrom(istream &s)
{
    CollectionView::ReadFrom(s);
    selection= cIdNone;
    level= 0;
    window= 0;
    s >> title;
    SetFlag(eCVModified);
    Update();
    SetFlag(eMenuIsNew);
    return s;
}

void Menu::InspectorId(char *buf, int sz)
{
    if (title)
	title->InspectorId(buf, sz);
    else
	CollectionView::InspectorId(buf, sz);
}

//---- ShadowBorderItem --------------------------------------------------------

MetaImpl(ShadowBorderItem, (T(shadowWidth), 0));

ShadowBorderItem::ShadowBorderItem(VObject *title, VObject *inner, int shadow)
				: AbstractBorderItem(title, inner, 0, cIdNone)
{
    shadowWidth= shadow;
    border= gPoint2;
}

ShadowBorderItem::ShadowBorderItem(char *title, VObject *inner, int shadow)
				: AbstractBorderItem(title, inner, 0, cIdNone)
{
    shadowWidth= shadow;
    border= gPoint2;
}

ShadowBorderItem::ShadowBorderItem(VObject *inner, int shadow)
				: AbstractBorderItem((VObject*)0, inner, 0, cIdNone)
{
    shadowWidth= shadow;
    border= gPoint2;
}

void ShadowBorderItem::Draw(Rectangle r)
{
    Rectangle rr= contentRect;

    rr.extent-= shadowWidth;
    GrPaintRect(rr, ePatWhite);
    if (! interior->contentRect.ContainsRect(r)) {
	if (shadowWidth) {
	    GrPattern pat= shadowWidth == 1 ? ePatBlack : ePatGrey50;
	    GrPaintRect(Rectangle(rr.origin.x+rr.extent.x, rr.origin.y+shadowWidth, shadowWidth, rr.extent.y), pat);
	    GrPaintRect(Rectangle(rr.origin.x+shadowWidth, rr.origin.y+rr.extent.y, rr.extent.x, shadowWidth), pat);
	}
	GrSetPenNormal();
	GrStrokeRect(rr);
    }
    if (titleBar) {
	Rectangle cr= contentRect;
	RGBColor c(230, 230, 230);
	
	cr.extent-= shadowWidth;
	cr.extent.y= titleBar->Height();
	cr= cr.Inset(2);
	if (GrHasColor())
	    GrPaintRect(cr, &c);
	AbstractBorderItem::Draw(r);
	if (!GrHasColor())
	    GrInvertRect(cr);
    } else
	AbstractBorderItem::Draw(r);
}

