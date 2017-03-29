//$DrawView$
#include "DrawView.h"
#include "ObjList.h"
#include "Commands.h"
#include "Menu.h"
#include "Document.h"
#include "TextShape.h"
#include "TextCmd.h"
#include "Group.h"
#include "CmdNo.h"
#include "CollView.h"
#include "ClipBoard.h"
#include "System.h"
#include "ImageShape.h"
#include "PictureShape.h"
#include "Picture.h"

const int cARRANGEMENU  = 1121,
	  cOPTIONEMENU  = 1122,
	  cGridSize     = 8;
	  
//---- DrawView ----------------------------------------------------------------

MetaImpl(DrawView, (TP(activeTextView), TP(currShape), T(lastClick), T(grid),
	TB(showGrid), TP(shapes), TP(selection), 0));

DrawView::DrawView(Document *dp, Point extent, ObjList *sl) : View(dp, extent)
{
    showGrid= TRUE;
    grid= cGridSize;
    activeText= 0;
    activeTextView= 0;
    lastClick = Point(100);
    shapes= sl;
    currShape= 0;
    selection= new ObjList;
}

DrawView::~DrawView()
{
    SafeDelete(selection);
    if (shapes) {
	shapes->FreeAll();
	SafeDelete(shapes);
    }
}

//---- initializing ------------------------------------------------------------

void DrawView::SetShapes(class ObjList *sl)
{
    if (shapes) {
	shapes->FreeAll();
	SafeDelete(shapes);
    }
    shapes= sl;
    SetSelection(0);
    shapes->ForEach(Shape,SetContainer)(this);
    ForceRedraw(); 
}

//---- drawing -----------------------------------------------------------------

void DrawView::Invalidate(ObjList *ol)
{
    Rectangle bbox;
    Iter next(ol);
    Shape *s;
    
    while (s= (Shape*)next())
	bbox.Merge(s->InvalRect());
    InvalidateRect(bbox);
}

static Bitmap *Grid= 0;

void DrawView::Draw(Rectangle r)
{
    RevIter previous(shapes);
    register Shape *s;
    
    if (!gPrinting && showGrid) {
	if (Grid == 0) {
	    Grid= new Bitmap(Point(32));
	    Grid->SetPixel(0, 0, 1);
	    Grid->SetPixel(16, 0, 1);
	    Grid->SetPixel(0, 16, 1);
	}
	GrPaintRect(r, Grid);
    }

    while (s= (Shape*) previous())
	s->DrawAll(r);
    if (! gPrinting && selection) {
	Iter next(selection);

	while (s= (Shape*)next())
	    if (! s->GetDeleted())
		s->Highlight(On);
    }
}

//---- shape list management ---------------------------------------------------
    
void DrawView::Insert(Shape *s)
{
    SetSelection(0);
    s->SetContainer(this);
    shapes->Insert(s);
    selection->Add(s);
    InvalidateRect(s->InvalRect());
    ShowSelection();
}

void DrawView::Remove(Shape *s)
{
    shapes->RemovePtr(s);
}

void DrawView::InsertShapes(ObjList *ol, bool tofront)
{
    RevIter previous(ol);
    Shape *s;
    
    SetSelection(0);
    while (s= (Shape*) previous()) {
	if (! s->IsKindOf(Shape))
	    continue;
	s->SetContainer(this);
	if (tofront)
	    shapes->Insert(s);
	else
	    shapes->Add(s);
	selection->Add(s);
    }
    Invalidate(ol);
    ShowSelection();
}

Shape *DrawView::FindShape(Point p)
{
    Iter next(shapes);
    Shape *s;
    
    while (s= (Shape*)next())
	if (s->ContainsPoint1(p))
	    return s;
    return 0;
}

Shape *DrawView::FindHandle(Point p, int *nh)
{
    Iter next(selection);
    Shape *s;
    int handle;
    
    while (s= (Shape*)next())
	if ((handle= s->PointOnHandle(p)) >= 0) {
	    *nh= handle;
	    return s;
	}
    return 0;
}

//---- misc --------------------------------------------------------------------

void DrawView::Point2Grid(Point *np)  // constrain to viewsize and align to grid
{
    Point p= Min(GetExtent(), Max(gPoint3, *np));
    *np= ((p+grid/2) / grid) * grid;
}

void DrawView::ConstrainScroll(Point *p)
{
    if (activeTextView)
	activeTextView->ConstrainScroll(p);
}

void DrawView::RequestTool(int tix)
{
    Control(GetId(), 0, (void*) tix);
}

void DrawView::SetTool(Object *cs)
{
    Shape *oldShape= currShape;

    if (cs && cs->IsKindOf(Shape))
	currShape= (Shape*) cs;
    else
	currShape= 0;
	
    // enter text mode
    if ((oldShape == 0 || !oldShape->IsKindOf(TextShape)) &&
				currShape && currShape->IsKindOf(TextShape)) {
	PerformCommand(gResetUndo);
	SetSelection(0);
    }

    // leave text mode
    if ((currShape == 0 || !currShape->IsKindOf(TextShape)) &&
				oldShape && oldShape->IsKindOf(TextShape)) {
	PerformCommand(gResetUndo);
	SetSelection(0);
	SetActiveText(0);
    }
}

void DrawView::ShowInfo(TrackPhase tp, char *va_(fmt), ...)
{
    if (tp == eTrackRelease)
	Control(GetId(), 123, "");
    else {
	char buf[100];
	va_list ap;
	va_start(ap, va_(fmt));
	vsprintf(buf, va_(fmt), ap);
	va_end(ap);
	Control(GetId(), 123, buf);
    }
}

//---- Selection ---------------------------------------------------------------

Shape *DrawView::OneSelected()
{
    if (Selected() != 1)
	return 0;
    return (Shape*)selection->First();
}

ObjList *DrawView::GetCopyOfSelection()
{
    //RemoveDeleted();
    return (ObjList*)selection->Clone();
}

ObjList *DrawView::GetDeepCopyOfSelection()
{
    //RemoveDeleted();
    return (ObjList*)selection->DeepClone();
}

ObjList *DrawView::SetSelection(ObjList *newsel)
{
    ObjList *oldsel= selection;
    
    // Reset old Selection
    if (selection->Size()) {
	selection->ForEach(Shape,SetSplit)(FALSE);
	Invalidate(selection);
	SafeDelete(selection);
	selection= 0;
	UpdateEvent();
    }
    if (newsel) {
	selection= (ObjList*) newsel->Clone();
	Invalidate(selection);
	ShowSelection();   
    } else
	selection= new ObjList;
    return oldsel;
}

void DrawView::SetDeleted(ObjList* ol, bool b)
{
    ol->ForEach(Shape,SetDeleted)(b);
    Invalidate(ol);
}

void DrawView::RemoveDeleted()
{
    Iter next(shapes);
    register Shape *s;

    while (s= (Shape*) next())
	if (s->IsGarbage()) {
	    Object *tmp= shapes->Remove(s);
	    delete tmp;
	}
}

void DrawView::ShowSelection()
{
    Rectangle bbox= BoundingBox();
    RevealRect(bbox.Expand(8), bbox.extent/3);
}

void DrawView::SelectInRect(Rectangle r)
{
    Iter next(shapes);
    register Shape *s;
    
    while (s= (Shape*)next())
	if (!s->GetDeleted() && r.ContainsRect(s->bbox))
	    selection->Add(s);
    Invalidate(shapes);
}

Rectangle DrawView::BoundingBox()
{
    Rectangle bbox;
    Iter next(selection);
    Shape *s;
    
    while (s= (Shape*)next())
	bbox.Merge(s->bbox);
    return bbox;
}

bool DrawView::HasSelection()
{
    return (bool) (activeTextView || selection->Size() > 0);
}

//---- text --------------------------------------------------------------------

void DrawView::SetActiveText(TextShape *tp)
{
    if (activeText) {
	selection->Remove(activeText);
	//activeText->Invalidate();
	activeTextView->SetNoSelection();
	activeText= 0;
	activeTextView= 0;
    }
    activeText= tp;
    activeTextView= 0;
    if (activeText) {
	activeTextView= activeText->GetTextView();
	if (! selection->ContainsPtr(activeText))
	    selection->Add(activeText);
	//activeText->Invalidate();
	//activeText->Invalidate();
    }
    UpdateEvent();
}

//---- event handling ----------------------------------------------------------

Command *DrawView::DoLeftButtonDownCommand(Point p, Token t, int)
{
    Shape *ShapeUnderMouse, *s;
    int handle;
    SketchModes sm= eSMDefault;
    bool inselection;
    
    if (t.Flags & eFlgShiftKey)
	sm= (SketchModes)(sm | eSMSquare);
    if (t.Flags & eFlgCntlKey)
	sm= (SketchModes)(sm | eSMCenter);
       
    if (currShape == 0 || !currShape->IsKindOf(TextShape))  // exit text mode
	SetActiveText(0);
	
    ShapeUnderMouse= FindShape(p);
    lastClick= p;
    
    if (currShape) {    // not pointer mode
	if (currShape->IsKindOf(TextShape) && ShapeUnderMouse
			&& ShapeUnderMouse->IsKindOf(TextShape)) {  // Text Mode
	    SetActiveText((TextShape*) ShapeUnderMouse);
	    return activeTextView->DispatchEvents(p, t, focus);
	}
	SetSelection(0);
	return currShape->NewSketcher(this, sm);
    }
    
    if (s= FindHandle(p, &handle))
	return s->NewStretcher(this, handle);

    if (ShapeUnderMouse == 0) {
	SetSelection(0);
	return new ShapeSelector(this);
    } 
    
    inselection= selection->ContainsPtr(ShapeUnderMouse);
    if (! inselection) {
	if (! (t.Flags & eFlgShiftKey))
	    SetSelection(0);
	selection->Add(ShapeUnderMouse);
	ShapeUnderMouse->Invalidate();
    } else {
	if (t.Flags & eFlgShiftKey) {
	    selection->Remove(ShapeUnderMouse);
	    ShapeUnderMouse->Invalidate();
	}
    }
    if (selection->ContainsPtr(ShapeUnderMouse)) {
	if ((handle= ShapeUnderMouse->PointOnHandle(p)) >= 0)
	    return ShapeUnderMouse->NewStretcher(this, handle);
	return new ShapeDragger(this, ShapeUnderMouse);
    }
    return gNoChanges;
}

Command *DrawView::DoKeyCommand(int code, Point lp, Token t)
{
    TextShape *ts;
    
    if (activeTextView)
	return activeTextView->DispatchEvents(lp, t, focus);

    if (code == gBackspace)
	return new SCutCopyCommand(this, cDELETE, "delete");
	
    //----- enter text mode and create an attached TextShape
    Shape *chief= OneSelected();
    RequestTool(1);
    
    //---- on text shape and selected hence append typed character at end
    if (chief && chief->IsKindOf(TextShape)) { 
	SetSelection(0);
	SetActiveText((TextShape*)chief);
	activeTextView->SetSelection(cMaxInt, cMaxInt);
	return activeTextView->DispatchEvents(lp, t, focus);
	//return activeTextView->DoKeyCommand(code, lp, t);
    }
    
    //---- create new textshape
    SetSelection(0);
    ts= (TextShape*) currShape->Clone();
    ts->SetContainer(this);
    if (chief) {
	Rectangle textRect= chief->GetTextRect();
	ts->Init(textRect.NW(), textRect.SE());
	ts->MakeDependentOn(chief); // make the text object dependend if there is a chief
    } else
	ts->Init(lastClick, lastClick);
    Insert(ts);
    SetActiveText(ts);
    
    // return activeTextView->DispatchEvents(lp, t, focus);
    return activeTextView->DoKeyCommand(code, lp, t);
}

Command *DrawView::DoCursorKeyCommand(EvtCursorDir cd, Point p, Token t)
{
    if (activeTextView)  
	return activeTextView->DoCursorKeyCommand(cd, p, t);
	
    if (selection->Size() > 0) {
	Point delta= t.CursorPoint() * grid;
	Rectangle bb= BoundingBox() + delta;
	delta+= bb.AmountToTranslateWithin(GetExtent());
	return new CursorMoveCommand(this, delta);
    }
    return View::DoCursorKeyCommand(cd, p, t);
}

GrCursor DrawView::GetCursor(Point lp)
{
    if (currShape)
	return currShape->SketchCursor();
    return View::GetCursor(lp);
}

//---- menus -------------------------------------------------------------------

void DrawView::DoCreateMenu(Menu *menu)
{
    View::DoCreateMenu(menu);
    Menu *m;

    menu->InsertItemsAfter(cLASTEDIT, 
		    "delete",       cDELETE,
		    "-",
		    "dup",          cDUP,
		    "edit shape",   cSPLIT,
		    "connect",      cCONNECT,
		    0);

    m= new Menu("arrange");
    m->AppendItems(
		    "bring to front",   cTOFRONT,
		    "send to back",     cTOBACK,
		    "-",
		    "group",            cGROUP,
		    "ungroup",          cUNGROUP,
		    0);
    menu->AppendMenu(m, cARRANGEMENU);
    
    m= new Menu("options");
    m->AppendItems(
		    "grid off",     cGRID,
		    "hide grid       ",cSHWGRID,
		    0);
    menu->AppendMenu(m, cOPTIONEMENU);
}    

void DrawView::DoSetupMenu(Menu *menu)
{
    if (activeTextView) {
	if (!activeTextView->Caret())
	    menu->EnableItem(cDELETE);
    } else {
	int n= selection->Size();
    
	if (n > 0) {
	    Shape *s;
    
	    menu->EnableItems(cDELETE, cDUP, cTOFRONT, cTOBACK, cARRANGEMENU, 0);
	    if (s= OneSelected()) {
		if (s->IsKindOf(Group))
		    menu->EnableItem(cUNGROUP);
		if (s->CanSplit())
		    menu->EnableItem(cSPLIT);
	    } else if (n >= 2)
		menu->EnableItems(cCONNECT, cGROUP, 0);
	}
    }
    menu->ReplaceItem(cGRID, (grid == cGridSize) ? "grid off" : "grid on");
    menu->ReplaceItem(cSHWGRID, showGrid ? "hide grid" : "show grid");
    menu->EnableItems(cOPTIONEMENU, cGRID, cSHWGRID, 0);
    View::DoSetupMenu(menu);
}

Command *DrawView::DoMenuCommand(int cmd)
{
    Shape *p= OneSelected();

    switch(cmd) {
    case cTOFRONT:
	return new FrontBackCommand(this, cmd, "bring to front");

    case cTOBACK:
	return new FrontBackCommand(this, cmd, "bring to back");

    case cDELETE:
	if (activeTextView)
	    return new CutCopyCommand(activeTextView, cCUT, "delete text");
	return new SCutCopyCommand(this, cmd, "delete");

    case cCUT:
    case cCOPY:
	gClipBoard->SetType((char*) cDocTypeET);
	View::DoMenuCommand(cmd);
	if (activeTextView)
	    return activeTextView->DoMenuCommand(cmd);
	return new SCutCopyCommand(this, cmd);

    case cDUP:
	return new DupCommand(this);
	
    case cGROUP:
	return new GroupCommand(this);
	
    case cUNGROUP:
	return new UngroupCommand(this, (Group*)p);
	
    case cSPLIT:
	if (p)
	    p->SetSplit(TRUE);
	break;
	
    case cCONNECT:
	return new ConnectCommand(this);
	
    case cGRID:
	grid= (grid == 1) ? cGridSize : 1;
	break;
	
    case cSHWGRID:
	showGrid= (bool) (! showGrid);
	ForceRedraw();
	break;
		
    default:
	return View::DoMenuCommand(cmd);
    }
    return gNoChanges;
}

//---- clipboard ----------------------------------------------------------------

void DrawView::SelectionToClipboard(char *type, ostream &os)
{
    if (activeTextView)
	activeTextView->SelectionToClipboard(type, os);
    else if (strcmp(type, cDocTypeET) == 0)
	os << selection SP;
}

Command *DrawView::PasteData(char *type, istream &is)
{
    if (activeTextView)
	return activeTextView->PasteData(type, is);
    
    Shape *ns= 0;

    if (strcmp(type, cDocTypeET) == 0) {
	Object *op;
	is >> op;
	if (op) {
	    if (op->IsKindOf(ObjList))
		return new SPasteCommand(this, (ObjList*)op, lastClick);
	    if (op->IsKindOf(Picture))
		ns= new PictureShape((Picture*)op);
	}
    } else if (strcmp(type, cDocTypeBitmap) == 0) {
	Bitmap *bm= 0;
	is >> bm;
	if (bm)
	    ns= new ImageShape(bm);
    }
    if (ns)
	return new SPasteCommand(this, ns, lastClick);
    return gNoChanges;
}

bool DrawView::CanPaste(char *type)
{
    if (activeTextView)
	return activeTextView->CanPaste(type);
    return strismember(type, cDocTypeET, cDocTypeBitmap, 0);
}
