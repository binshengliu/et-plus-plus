#ifndef Command_First
#ifdef __GNUG__
#pragma once
#endif
#define Command_First

#include "Object.h"
#include "Point.h"

typedef enum {
    eTrackPress,
    eTrackMove,
    eTrackRelease,
    eTrackIdle,
    eTrackExit
} TrackPhase;

typedef enum {
    eCmdCanUndo      = BIT(eObjLast+1),
    eCmdCausesChange = BIT(eObjLast+2),
    eCmdDone         = BIT(eObjLast+3),
    eCmdMoveEvents   = BIT(eObjLast+4),
    eCmdIdleEvents   = BIT(eObjLast+5),
    eCmdFullScreen   = BIT(eObjLast+6),
    eCmdDoDelete     = BIT(eObjLast+7),
    eCmdNoReplFeedback= BIT(eObjLast+8),
    eCmdDefault      = eCmdCanUndo | eCmdCausesChange | eCmdDoDelete,
    eCmdLast         = eObjLast + 8
} CommandFlags;

class Command: public Object {
    int CmdNumber;
    char *CmdName;

public:
    MetaDef(Command);

    Command(int= 0, char *CmdName= 0, CommandFlags cf= eCmdDefault);
    Command(char *CmdName, CommandFlags cf= eCmdDefault);
    ~Command();

    void SetName(char *name)
	{ CmdName= name; }
    char *GetName()
	{ return CmdName; }
    char *GetUndoName();
    void SetId(int cmd)
	{ CmdNumber= cmd; }
    int GetId()
	{ return CmdNumber; }
	
    void Finish(Command *newcmd);
    int Perform();
    int Undo();
    int Do();

protected:
    virtual void Commit();
    virtual void UndoIt();
    virtual void DoIt();
    virtual void RedoIt();
    virtual void Done(Command *newCommand);

friend class Clipper;
    virtual void TrackConstrain(Point anchorPoint, Point previousPoint, 
							    Point *nextPoint);
    virtual void TrackFeedback(Point anchorPoint, Point previousPoint,
	    bool turnItOn);

    virtual Command *TrackMouse(TrackPhase aTrackPhase, Point anchorPoint,
	    Point previousPoint, Point nextPoint);

    void InspectorId(char *buf, int sz);
};

extern Command *gNoChanges;
extern Command *gResetUndo;

#endif Command_First

