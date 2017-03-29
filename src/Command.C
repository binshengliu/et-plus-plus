//$Command$
#include "Command.h"
#include "Error.h"
#include "String.h"
#include "Port.h"
#include "ObjectTable.h"

AbstractMetaImpl(Command, (T(CmdNumber), TP(CmdName), 0));

static Command NoChanges(0);
static Command ResetUndo(0, "", eCmdCausesChange);

Command *gNoChanges= &NoChanges;
Command *gResetUndo= &ResetUndo;

Command::Command(int itsCmdNumber, char *itsCmdName, CommandFlags cf) : Object(cf)
{
    CmdNumber= itsCmdNumber;
    CmdName= itsCmdName;
}

Command::Command(char *itsCmdName, CommandFlags cf) : Object(cf)
{
    CmdNumber= 9999;
    CmdName= itsCmdName;
}

Command::~Command()
{ 
    if (this == gNoChanges || this == gResetUndo || !TestFlag(eCmdDoDelete)) 
	this= 0;
}

void Command::Commit()
{
}

void Command::DoIt()
{
}

void Command::RedoIt()
{
    DoIt();
}

void Command::UndoIt()
{
}

void Command::Done(Command *newCmd)
{
}

void Command::TrackConstrain(Point, Point, Point *)
{
}

void Command::TrackFeedback(Point anchorPoint, Point nextpoint, bool)
{
}

Command *Command::TrackMouse(TrackPhase, Point, Point, Point)
{
    ResetFlag(eCmdMoveEvents);
    return this;
}

void Command::Finish(Command *cmd)
{
    if (this && this != gNoChanges && this != gResetUndo) {
	if (TestFlag(eCmdDone)) {
	    Commit();
	}
	Done(cmd);
	if (TestFlag(eCmdDoDelete))
	    delete this;
    }
}

char *Command::GetUndoName()
{     
    char *s1, *s, *cs= GetName();
    
    if (TestFlag(eCmdCanUndo))
	s1= "";
    else
	s1= "can't ";

    if (TestFlag(eCmdDone))
	s= "undo";
    else
	s= "redo";
	
    if (cs && *cs)
	return form("%s%s %s", s1, s, cs);
    return form("%s%s", s1, s);
}

int Command::Undo()
{
    if (TestFlag(eCmdDone)) {
	UndoIt();
	ResetFlag(eCmdDone);
	return -1;
    }
    RedoIt();
    SetFlag(eCmdDone);
    return 1;
}

int Command::Do()
{
    DoIt();
    SetFlag(eCmdDone);
    if (TestFlag(eCmdCausesChange))
	return 1;
    return 0;
}

int Command::Perform()
{
    int cnt= 0;
    
    // don't do anything on gNoChanges!!
    if ((this != gNoChanges) && (CmdNumber != 0 || this == gResetUndo)) {
	if (this != gResetUndo)
	    cnt= Do(); 
	Finish(0);
    }
    return cnt;
}

void Command::InspectorId(char *buf, int sz)
{
    if (GetName())
	strn0cpy(buf, GetName(), sz);
    else
	Object::InspectorId(buf, sz);
}

