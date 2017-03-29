//$ShapeSelector,GroupCommand,UngroupCommand,SCutCopyCommand$
//$SPasteCommand,DupCommand,FrontBackCommand,ConnectCommand,PropertyCommand$
//$CursorMoveCommand,ShapeDragger,SimplePropertyCommand$

#include "Commands.h"
#include "Error.h"
#include "Connection.h"
#include "DrawView.h"
#include "Group.h"
#include "ObjList.h"
#include "CmdNo.h"
#include "StyledText.h"
#include "ObjArray.h"
#include "TextShape.h"

//---- Group Command ------------------------------------------------------------

GroupCommand::GroupCommand(DrawView *dv) : DrawCommand(dv, cGROUP, "group") 
{
    group= new Group(view->GetDeepCopyOfSelection());
}     

void GroupCommand::SaveDoIt()
{
    view->SetDeleted(saveselection, TRUE);
    view->Insert(group);
}

void GroupCommand::RestoreUndoIt()
{
    view->SetDeleted(saveselection, FALSE);
    group->SetDeleted(TRUE);
}

void GroupCommand::RestoreRedoIt()
{
    view->SetDeleted(saveselection, TRUE);
    group->SetDeleted(FALSE);
}

//---- Ungroup Command ------------------------------------------------------------

UngroupCommand::UngroupCommand(DrawView *dv, Group *gp)
					: DrawCommand(dv, cUNGROUP, "ungroup") 
{
    group= gp;
    newshapes= group->Ungroup();
}

UngroupCommand::~UngroupCommand()
{
    SafeDelete(newshapes);
}

void UngroupCommand::SaveDoIt()
{
    group->SetDeleted(TRUE);
    view->InsertShapes(newshapes);
}

void UngroupCommand::RestoreUndoIt()
{
    group->SetDeleted(FALSE);
    view->SetDeleted(newshapes, TRUE);
}

void UngroupCommand::RestoreRedoIt()
{
    group->SetDeleted(TRUE);
    view->SetDeleted(newshapes, FALSE);
}

//---- SCutCopy Command --------------------------------------------------------

SCutCopyCommand::SCutCopyCommand(DrawView *dv, int cmd, char *name)
						    : DrawCommand(dv, cmd, 0) 
{
    if (name == 0) {
	if (cmd == cCUT)
	    name= "cut";
	else if (cmd == cCOPY)
	    name= "copy";
    }
    SetName(name);
    if (cmd == cCOPY)
	ResetFlag(eCmdCausesChange);
}

void SCutCopyCommand::SaveDoIt()
{
    if (GetId() != cCOPY)
	view->SetDeleted(saveselection, TRUE);
}

void SCutCopyCommand::RestoreUndoIt()
{
    if (GetId() != cCOPY)
	view->SetDeleted(saveselection, FALSE);
}

//---- SPaste Command -----------------------------------------------------------

SPasteCommand::SPasteCommand(DrawView *dv, ObjList *ns, Point p)
					    : DrawCommand(dv, cPASTE, "paste") 
{
    Rectangle bbox;
    Iter next(ns);
    register Shape *s;
    
    newshapes= ns;
    
    while (s= (Shape*)next()) {
	s->SetContainer(view);
	bbox.Merge(s->bbox);
    }
    p-= bbox.origin;
    newshapes->ForEach(Shape,Moveby)(p);
}

SPasteCommand::SPasteCommand(DrawView *dv, Shape *ns, Point p) : (dv, cPASTE, "paste") 
{
    newshapes= new ObjList;
    newshapes->Add(ns);
    newshapes->ForEach(Shape,SetContainer)(view);
    ns->Moveby(p - ns->bbox.origin);
}

SPasteCommand::~SPasteCommand()
{
    SafeDelete(newshapes);
}

void SPasteCommand::SaveDoIt()
{
    view->InsertShapes(newshapes);
}

void SPasteCommand::RestoreUndoIt()
{
    view->SetDeleted(newshapes, TRUE);
}

void SPasteCommand::RestoreRedoIt()
{
    view->SetDeleted(newshapes, FALSE);
}

//---- Duplicate Command --------------------------------------------------------

DupCommand::DupCommand(DrawView *dv) : DrawCommand(dv, cDUP, "dup") 
{
    dups= view->GetDeepCopyOfSelection();
    dups->ForEach(Shape,SetContainer)(view);
    ddelta= 16;
}
	
DupCommand::~DupCommand()
{
    SafeDelete(dups);
}

void DupCommand::Done(Command *nextcmd)
{
    if (nextcmd->GetId() == cDUP)
	((DupCommand*)nextcmd)->ddelta= ddelta;
    if (nextcmd->GetId() == cDRAG)
	((CursorMoveCommand*)nextcmd)->dragDelta= ddelta;
}

void DupCommand::SaveDoIt()
{
    dups->ForEach(Shape,Moveby)(ddelta);
    view->InsertShapes(dups);
}

void DupCommand::RestoreUndoIt()
{
    view->SetDeleted(dups, TRUE);
}

void DupCommand::RestoreRedoIt()
{
    view->SetDeleted(dups, FALSE);
}

//---- FrontBack Command --------------------------------------------------------

FrontBackCommand::FrontBackCommand(DrawView *dv, int cmd, char *s)
						    : DrawCommand(dv, cmd, s)
{
    newshapes= view->GetDeepCopyOfSelection();
    newshapes->ForEach(Shape,SetContainer)(view);
}

FrontBackCommand::~FrontBackCommand()
{
    SafeDelete(newshapes);
}

void FrontBackCommand::SaveDoIt()
{
    view->InsertShapes(newshapes, (bool) (GetId() == cTOFRONT));
    view->SetDeleted(saveselection, TRUE);
}

void FrontBackCommand::RestoreUndoIt()
{
    view->SetDeleted(saveselection, FALSE);
    view->SetDeleted(newshapes, TRUE);
}

void FrontBackCommand::RestoreRedoIt()
{
    view->SetDeleted(saveselection, TRUE);
    view->SetDeleted(newshapes, FALSE);
}

//---- ConnectCommand -----------------------------------------------------------

ConnectCommand::ConnectCommand(DrawView *dv)
					: DrawCommand(dv, cCONNECT, "connect") 
{
    Iter next(view->GetSelectionIter());
    register Shape *first, *p1, *p2;
    
    connections= new ObjList;
    for (first= p1= (Shape*)next(); p1 && (p2= (Shape*)next()); p1= p2)
	connections->Insert(new Connection(view, p1, p2));
    if (view->Selected() > 2)
	connections->Insert(new Connection(view, p1, first));
}

ConnectCommand::~ConnectCommand()
{
    SafeDelete(connections);
}

void ConnectCommand::SaveDoIt()
{
    view->InsertShapes(connections);
}

void ConnectCommand::RestoreUndoIt()
{
    view->SetDeleted(connections, TRUE);
}

void ConnectCommand::RestoreRedoIt()
{
    view->SetDeleted(connections, FALSE);
}

//---- CursorMoveCommand Methods ------------------------------------------------

CursorMoveCommand::CursorMoveCommand(DrawView *dv, Point d, bool s)
					    : DrawCommand(dv, cDRAG, "move") 
{
    undoDelta= gPoint0;
    dragDelta= gPoint16;
    delta= d;
    scroll= s;
}

void CursorMoveCommand::Done(Command *nextcmd)
{
    if (nextcmd->GetId() == cDRAG) {
	((CursorMoveCommand*)nextcmd)->undoDelta= undoDelta;
	((CursorMoveCommand*)nextcmd)->dragDelta= dragDelta;
    }
    if (nextcmd->GetId() == cDUP)
	((DupCommand*)nextcmd)->ddelta= dragDelta;
}

void CursorMoveCommand::SaveDoIt()
{
    if (scroll)
	view->ShowSelection();
    view->Invalidate(saveselection);
    saveselection->ForEach(Shape,Moveby)(delta);
    view->Invalidate(saveselection);
    undoDelta+= delta;
    dragDelta-= delta;
}
	
void CursorMoveCommand::RestoreUndoIt()
{
    view->Invalidate(saveselection);
    saveselection->ForEach(Shape,Moveby)(-undoDelta);
    view->Invalidate(saveselection);
}

void CursorMoveCommand::RestoreRedoIt()
{
    view->Invalidate(saveselection);
    saveselection->ForEach(Shape,Moveby)(undoDelta);
    view->Invalidate(saveselection);
}

//---- Dragger Methods ----------------------------------------------------------

ShapeDragger::ShapeDragger(DrawView *dv, Shape *s)
					: CursorMoveCommand(dv, gPoint0, FALSE) 
{
    sp= s;
    onlyone= (bool) (view->Selected() == 1);
    bbox= view->BoundingBox();
    span= sp->GetSpan();
}
       
void ShapeDragger::TrackFeedback(Point, Point, bool)
{
    if (moved) {
	sp->Outline(span.origin+delta, span.extent+delta);
	if (! onlyone)
	    GrStrokeRect(Rectangle(bbox.origin+delta, bbox.extent));
    }
}

void ShapeDragger::TrackConstrain(Point ap, Point pp, Point *np)
{
    Rectangle r= bbox;
    r.origin+= pp-ap;
    *np+= r.AmountToTranslateWithin(view->GetExtent());
    DrawCommand::TrackConstrain(ap, pp, np);
}

Command *ShapeDragger::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    view->ShowInfo(tp, "x: %d y: %d", bbox.origin.x+delta.x, bbox.origin.y+ delta.y);
    switch (tp) {
    case eTrackMove:
	if (moved)
	    GrSetCursor(eCrsMoveHand);
	break;
    case eTrackRelease:
	if (delta == gPoint0)
	    return gNoChanges;
	undoDelta= delta;
	break;
    }
    return this;
}

//---- Shape Selector Methods --------------------------------------------------

void ShapeSelector::TrackFeedback(Point anchorPoint, Point nextpoint, bool)
{
    GrStrokeRect(NormRect(anchorPoint, nextpoint));
}

Command *ShapeSelector::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    DrawCommand::TrackMouse(tp, ap, pp, np);
    switch (tp) {
    case eTrackPress:
	GrSetCursor(eCrsHand); 
	break;
    case eTrackRelease:
	view->SelectInRect(NormRect(ap, np));
	return gNoChanges;
    }
    return this;
}

//---- Property Command --------------------------------------------------------

PropertyCommand::PropertyCommand(DrawView *dv, ShapeProperties w, Object *prop,
				char *cmd) : ChangePropertyCommand(dv, w+1, cmd)
{
    oldprop= new ObjArray(cnt);
    newprop= prop;
    what= w;
}

PropertyCommand::~PropertyCommand()
{
    for (int i= 0; i < cnt; i++) {
	Object *op= oldprop->At(i);
	SafeDelete(op);
    }
    SafeDelete(oldprop);
    SafeDelete(newprop);
}

void PropertyCommand::SetProperty(Shape *p, int)
{
    p->SetProperty(what, newprop);
}

void PropertyCommand::SaveProperty(Shape *p, int i)
{
    oldprop->AtPut(i, p->GetProperty(what));
}

void PropertyCommand::RestoreProperty(Shape *p, int i)
{
    p->ResetProperty(what, oldprop->At(i));
}

//---- SimpleProperty Command --------------------------------------------------------

SimplePropertyCommand::SimplePropertyCommand(DrawView *dv, ShapeProperties w, int prop,
				char *cmd) : ChangePropertyCommand(dv, w+1, cmd)
{
    oldprop= new int[cnt];
    newprop= prop;
    what= w;
}

SimplePropertyCommand::~SimplePropertyCommand()
{
    SafeDelete(oldprop);
}

void SimplePropertyCommand::SetProperty(Shape *p, int)
{
    p->SetSimpleProperty(what, newprop);
}

void SimplePropertyCommand::SaveProperty(Shape *p, int i)
{
    oldprop[i]= p->GetSimpleProperty(what);
}

void SimplePropertyCommand::RestoreProperty(Shape *p, int i)
{
    p->SetSimpleProperty(what, oldprop[i]);
}
