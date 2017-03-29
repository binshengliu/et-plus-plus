#ifndef ErCommands_First
#define ErCommands_First

#include "Command.h"

//---- InsertCommand -----------------------------------------------------------

class InsertCommand: public Command {
protected:
    class ErShape *newshape;
    class ErDocument *doc;

public:
    InsertCommand(ErDocument *d, ErShape *ns, char *name= "new shape");
    ~InsertCommand();
    
    void DoIt();
    void UndoIt();
    void Commit();
};

//---- ConnectCommand ----------------------------------------------------------

class ConnectCommand: public InsertCommand {
    class ErShape *start, *end;
    class ErView *view;

public:
    ConnectCommand(ErView *v, ErDocument *d, ErShape *h);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase atp, Point ap, Point, Point np);
};

#endif ErCommands_First

