//$DrawCommand,ShapeSketcher,Shape,ChangePropertyCommand,ShapeStretcher$
#include "Shape.h"
#include "DrawView.h"
#include "ObjList.h"
#include "RunArray.h"
#include "ObjInt.h"
#include "WindowSystem.h"

Point HandleSize(5,5);
Point HandleSize2= HandleSize/2;
Point HandleSize3= HandleSize2+1;

Point Shape::spts[10];

//---- DrawCommand Methods -----------------------------------------------------

MetaImpl(DrawCommand, (T(moved), TP(view), TP(saveselection), TP(saveselection2), 0));

DrawCommand::DrawCommand(class DrawView *dv, int cmd, char *cmdname)
							: Command(cmd, cmdname)
{   
    moved= FALSE; 
    view= dv; 
}

DrawCommand::~DrawCommand()
{
    SafeDelete(saveselection);
    SafeDelete(saveselection2);
    if (view && TestFlag(eCmdCausesChange) && GetId())
	view->RemoveDeleted();
}

void DrawCommand::TrackConstrain(Point, Point, Point *np)
{
    view->Point2Grid(np);
}

Command *DrawCommand::TrackMouse(TrackPhase atp, Point ap, Point, Point np)
{
    delta= np-ap;
    switch (atp) {
    case eTrackPress:
	oldcursor= GrGetCursor();
	break;
    case eTrackMove:
	if (abs(delta.x) > 4 || abs(delta.y) > 4)
	    moved= TRUE;
	break;
    case eTrackRelease:
	GrSetCursor(oldcursor);
	break;
    }
    return this;
}

void DrawCommand::DoIt()
{
    saveselection= view->GetCopyOfSelection();
    SaveDoIt();
    saveselection2= view->GetCopyOfSelection();
}

void DrawCommand::UndoIt()
{
    RestoreUndoIt();
    view->SetSelection(saveselection);
}

void DrawCommand::RedoIt()
{
    RestoreRedoIt();
    view->SetSelection(saveselection2);
}

void DrawCommand::SaveDoIt()
{
    AbstractMethod("SaveDoIt");
}

void DrawCommand::RestoreRedoIt()
{
    SaveDoIt();
}

void DrawCommand::RestoreUndoIt()
{
    AbstractMethod("RestoreUndoIt");
}

//---- Shape Methods -----------------------------------------------------------

AbstractMetaImpl(Shape, (TP(container), T(ink), T(penink), T(pensize),
			     TE(arrows), T(isDeleted), T(isEnabled), T(bbox)));

Shape::Shape()
{
    Init1();
}

Shape::~Shape()
{
}

void Shape::Init1()
{
    ink= 1;
    penink= 2;
    pensize= 1;
    isDeleted= FALSE;
    isEnabled= TRUE;
    bbox= gRect0;
    container= 0;
    arrows= eDefaultCap;
    ResetFlag(eShapeSplit);
}

short *Shape::GetImage()
{
    return 0;
}

GrCursor Shape::SketchCursor()
{
    return eCrsCross;
}

void Shape::SetContainer(VObject *vop)
{
    container= vop;
}
	
DrawView *Shape::GetView()
{
    return Guard(container,DrawView);
}

void Shape::Flip(int n)
{
}

Rectangle Shape::GetSpan()
{
    return Rectangle(bbox.NW(), bbox.SE());
}

void Shape::SetDeleted(bool b)
{
    isDeleted= b;       
    Changed();
}

void Shape::SetProperty(ShapeProperties, Object*)
{
    Changed();
}

void Shape::SetSimpleProperty(ShapeProperties what, int p)
{
    switch (what) {
    case eShapePattern:
	ink= p;
	break;
    case eShapePenPattern:
	penink= p;
	break;
    case eShapePensize:
	pensize= p;
	break;
    case eShapeArrows:
	arrows= (GrLineCap) p;
	break;
    }
    Changed();
}

void Shape::ResetProperty(ShapeProperties what, Object *op)
{
    SetProperty(what, op);
}

void Shape::ResetSimpleProperty(ShapeProperties what, int p)
{
    SetSimpleProperty(what, p);
}

Object *Shape::GetProperty(ShapeProperties)
{
    return 0;
}

int Shape::GetSimpleProperty(ShapeProperties what)
{
    switch (what) {
    case eShapePattern:
	return ink;
    case eShapePenPattern:
	return penink;
    case eShapePensize:
	return pensize;
    case eShapeArrows:
	return arrows;
    case eShapeSmooth:
	return ePolyDefault;
    }
}

Point Shape::GetConnectionPoint(Point)
{
    return bbox.Center();
}

Point Shape::Chop(Point p)
{
    return bbox.AngleToPoint(bbox.PointToAngle(p));
}

Rectangle Shape::GetTextRect()
{
    return bbox.Inset(Point(4));
}

void Shape::SetSplit(bool b)
{  
    SetFlag(eShapeSplit, b);
    Invalidate();
    Changed();
}

bool Shape::CanSplit()
{
    return FALSE;
}

Rectangle Shape::InvalRect()
{
    return bbox.Expand(Max(HandleSize3, Point(pensize)));
}

bool Shape::ContainsPoint1(Point p)
{
    if (GetDeleted())
	return FALSE;
    if (InvalRect().ContainsPoint(p))
	return ContainsPoint(p);
    return FALSE;
}

bool Shape::IsGarbage()
{
    return (bool) (GetDeleted() || (penink == 0 && ink == 0));
}

bool Shape::ContainsPoint(Point)
{
    return TRUE;
}

void Shape::Moveby(Point delta)
{
    bbox.origin+= delta;
    Changed();
}

void Shape::SetSpan(Rectangle r)
{
    bbox= NormRect(r.origin, r.extent);
    Changed();
}

void Shape::Init(Point p1, Point p2)
{
    bbox= NormRect(p1, p2);
}

void Shape::DrawAll(Rectangle r)
{
    if (IsInRect(&r)) {
	GrSetPattern(InkPalette[ink]);
	GrSetPenPattern(InkPalette[penink]);
	GrSetPenSize(pensize);
	GrSetLineCap(arrows);
	Draw(r);
    }
}

void Shape::Draw(Rectangle)
{
}

void Shape::Outline(Point, Point)
{
}

void Shape::Invalidate()
{
    if (GetContainer())
	GetContainer()->InvalidateRect(InvalRect());
}

static RGBColor *firstHandleColor;

void Shape::Highlight(HighlightState)
{
    int n;
    Point *pl= MakeHandles(&n),
	  offset= bbox.origin - HandleSize2;
    Rectangle r(HandleSize);
    Ink *pat= isEnabled ? gHighlightColor : ePatGrey50;
    
    for (int i= n-1; i >= 0; i--) {
	r.origin= offset + pl[i];
	if (i == 0 && GetSplit()) {
	    if (gColor) {
		if (firstHandleColor == 0)
		    firstHandleColor= new RGBColor(255, 0, 0);
		GrPaintRect(r, firstHandleColor);
	    } else
		GrPaintRect(r.Expand(2), pat);
	} else
	    GrPaintRect(r, pat);
    }
}

Point *Shape::MakeHandles(int *n)
{    
    spts[0].x= spts[3].x= spts[5].x = -1;
    spts[1].x= spts[6].x          = (bbox.extent.x/2);
    spts[2].x= spts[4].x= spts[7].x = bbox.extent.x;
    spts[0].y= spts[1].y= spts[2].y = -1;
    spts[3].y= spts[4].y          = (bbox.extent.y/2);
    spts[5].y= spts[6].y= spts[7].y = bbox.extent.y;
    *n= 8;
    return spts;
}

void Shape::HighlightHandle(int h)
{
    int n;
    Point *handles= MakeHandles(&n);

    if (h < 0 || h > n-1)
	return;
    GrInvertRect(Rectangle(bbox.origin-HandleSize2+handles[h], HandleSize));
}   

int Shape::PointOnHandle(Point p)
{
    int n;
    Point *pl= MakeHandles(&n);
    
    p= p - bbox.origin + HandleSize2;
    for (int i= 0; i<n; i++)
	if (Rectangle(pl[i], HandleSize).Expand(1).ContainsPoint(p))
	    return i;
    return -1;
}

ShapeStretcher *Shape::NewStretcher(DrawView *dv, int handle)
{
    return new ShapeStretcher(dv, this, handle);
}

ShapeSketcher *Shape::NewSketcher(class DrawView *dv, SketchModes m)
{
    return new ShapeSketcher(dv, this, m);
}

ostream &Shape::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    return s << bbox SP << ink SP << penink SP << pensize SP << arrows SP;
}

istream &Shape::ReadFrom(istream& s)
{
    Init1();
    Object::ReadFrom(s);
    return s >> bbox >> ink >> penink >> pensize  >> Enum(arrows);
}

//---- ShapeSketcher Methods ---------------------------------------------------

ShapeSketcher::ShapeSketcher(DrawView *dv, Shape *pro, SketchModes m)
					: DrawCommand(dv, cNEWSHAPE, "new shape") 
{
    mode= m;
    proto= pro;     // points to a prototype shape (for cloning)
}

void ShapeSketcher::TrackConstrain(Point ap, Point pp, Point *np)
{
    Point p, extent;
    int maxextent;
    
    DrawCommand::TrackConstrain(ap, pp, np);
    if (mode & eSMSquare) {
	extent= *np - ap;
	maxextent= max(abs(extent.x), abs(extent.y));
	p= Point(maxextent, maxextent);
	
	if (extent.y < 0)
	    p.y= -p.y;
	if (extent.x < 0)
	    p.x= -p.x;
	*np= ap + p;
    }
}

void ShapeSketcher::TrackFeedback(Point ap, Point pp, bool)
{
    if (mode & eSMCenter)
	proto->Outline(2*ap-pp, pp);
    else
	proto->Outline(ap, pp);
}
 
Command *ShapeSketcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    Point extent= Abs(ap - np);
    view->ShowInfo(tp, "w: %4d h: %4d", extent.x, extent.y);
    if (tp == eTrackRelease) {
	if (extent > MinShapeSize) {
	    newshape= (Shape*)proto->Clone();
	    if (mode & eSMCenter)
		newshape->Init(2*ap-pp, pp);
	    else
		newshape->Init(ap, np);
	} else
	    return gNoChanges;
    }
    return this;
}

void ShapeSketcher::SaveDoIt()
{
    view->Insert(newshape);
    view->RequestTool(0);
}

void ShapeSketcher::RestoreUndoIt()
{
    newshape->SetDeleted(TRUE);
    newshape->Invalidate();
}

void ShapeSketcher::RestoreRedoIt()
{
    newshape->SetDeleted(FALSE);
    newshape->Invalidate();
}

//---- Change Property Command --------------------------------------------------

ChangePropertyCommand::ChangePropertyCommand(class DrawView *dv,
			int cmd, char *cmdname) : DrawCommand(dv, cmd, cmdname)
{
    cnt= view->Selected();
}

void ChangePropertyCommand::SaveDoIt()
{
    Iter next(saveselection);
    Shape *p;
    Rectangle bbox;
    
    for (int i= 0; p= (Shape*) next(); i++) {
	SaveProperty(p, i);
	bbox.Merge(p->InvalRect());
	SetProperty(p, i);
	bbox.Merge(p->InvalRect());
    }
    view->InvalidateRect(bbox);
}

void ChangePropertyCommand::RestoreUndoIt()
{
    Iter next(saveselection);
    Shape *p;
    Rectangle bbox;
    
    for (int i= 0; p= (Shape*) next(); i++) {
	bbox.Merge(p->InvalRect());
	RestoreProperty(p, i);
	bbox.Merge(p->InvalRect());
    }
    view->InvalidateRect(bbox);
}

void ChangePropertyCommand::RestoreRedoIt()
{
    Iter next(saveselection);
    Shape *p;
    Rectangle bbox;
    
    for (int i= 0; p= (Shape*) next(); i++) {
	bbox.Merge(p->InvalRect());
	SetProperty(p, i);
	bbox.Merge(p->InvalRect());
    }
    view->InvalidateRect(bbox);
}

void ChangePropertyCommand::SetProperty(Shape *, int)
{
}

void ChangePropertyCommand::SaveProperty(Shape *, int)
{
}

void ChangePropertyCommand::RestoreProperty(Shape *, int)
{
}

//---- Shape Stretcher Methods -------------------------------------------------

ShapeStretcher::ShapeStretcher(DrawView *dv, Shape *p, int h)
			    : ChangePropertyCommand(dv, cSTRETCHSHAPE, "resize")
{
    sp= p;
    handle= h;
    theBbox= p->bbox;
    if (cnt > 1)
	allBbox= view->BoundingBox();
    oldSpans= new Rectangle[cnt];
    flip= 0;
}

ShapeStretcher::~ShapeStretcher()
{
    SafeDelete(oldSpans);
}

void ShapeStretcher::SaveProperty(Shape *p, int i)
{
    oldSpans[i]= p->GetSpan();
}

void ShapeStretcher::RestoreProperty(Shape *p, int i)
{
    p->SetSpan(oldSpans[i]);
    p->Flip(flip);
}

void ShapeStretcher::SetProperty(Shape *p, int i)
{
    Point pp1= Scale(oldSpans[i].origin-theBbox.origin, sx, sy) + bbox1.origin,
	  pp2= Scale(oldSpans[i].extent-theBbox.origin, sx, sy) + bbox1.origin;
    p->SetSpan(Rectangle(pp1, pp2));
    p->Flip(flip);
}

void ShapeStretcher::TrackFeedback(Point, Point, bool)
{
    sp->Outline(p1, p2);
    if (cnt > 1)
	GrStrokeRect(bboxn);
}

void ShapeStretcher::TrackConstrain(Point ap, Point pp, Point *np)
{
    DrawCommand::TrackConstrain(ap, pp, np);
    switch (handle) {
    case 1:
    case 6:
	np->x= ap.x;
	break;
    case 3:
    case 4:
	np->y= ap.y;
	break;
    }
}

Command *ShapeStretcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    p1= theBbox.NW();
    p2= theBbox.SE();
    switch (handle) {
    case 0:
    case 1:
    case 3:
	p1+= delta;
	break;
    case 2:
	p1.y+= delta.y;
	p2.x+= delta.x;
	break;
    case 5:
	p1.x+= delta.x;
	p2.y+= delta.y;
	break;
    default:
	p2+= delta;
	break;
    }
    bbox1= NormRect(p1, p2);
    sx= (float) bbox1.extent.x / (float) theBbox.extent.x;
    sy= (float) bbox1.extent.y / (float) theBbox.extent.y;

    view->ShowInfo(tp, "w: %4d h: %4d", bbox1.extent.x, bbox1.extent.y);
    
    if (cnt > 1) {  // scale extent and distance of the all enclosing bbox
	bboxn.extent= Scale(allBbox.extent, sx, sy);
	bboxn.origin= bbox1.origin + Scale(allBbox.origin-theBbox.origin, sx, sy);
    }
    
    switch (tp) {
    case eTrackPress:
	sp->HighlightHandle(handle);
	break;
    case eTrackRelease:
	sp->HighlightHandle(handle);
	if (abs(p1.x-p2.x) < 2 || abs(p1.y-p2.y) < 2 || bbox1 == theBbox)
	    return gNoChanges;
	if (p2.x > p1.x) {
	    if (p2.y < p1.y)
		flip= 1;
	} else if (p2.x < p1.x) {
	    if (p2.y > p1.y)
		flip= 2;
	    else if (p2.y < p1.y)
		flip= 3;
	}
	break;
    }    
    return this;
}
