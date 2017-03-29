//$ShapeMover,InsertCommand,ConnectCommand$

#include "ErCommands.h"
#include "ErDocument.h"
#include "ErShapes.h"
#include "ErView.h"

//---- InsertCommand -----------------------------------------------------------

InsertCommand::InsertCommand(ErDocument *d, ErShape *ns, char *name)
								 : Command(name)
{
    doc= d;
    newshape= ns;
}

InsertCommand::~InsertCommand()
{
    SafeDelete(newshape);
}

void InsertCommand::DoIt()
{
    doc->AddShape(newshape);
}

void InsertCommand::UndoIt()
{
    doc->RemoveShape(newshape);
}

void InsertCommand::Commit()
{
    newshape= 0;
}

//---- ConnectCommand ----------------------------------------------------------

ConnectCommand::ConnectCommand(ErView *v, ErDocument *d, ErShape *st)
						: InsertCommand(d, 0, "connect")
{
    view= v;
    start= st;
}

void ConnectCommand::TrackFeedback(Point, Point np, bool)
{
    GrLine(start->Center(), end ? end->Center() : np);
}

Command *ConnectCommand::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    end= view->FindShape(np);
    if (end == 0                      // mouse not on any shape
	|| end == start         // can't connect me with myself
	    || !end->CanConnectWith(start))
	end= 0;

    if (atp == eTrackRelease) {
	if (end == 0)
	    return gNoChanges;
	newshape= new Connection(start, end);
    }
    return this;
}
