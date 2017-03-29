//$EvtHandler$
#include "EvtHandler.h"
#include "Command.h"
#include "View.h"
#include "Clipper.h"
#include "CmdNo.h"
#include "System.h"

AbstractMetaImpl0(EvtHandler);

EvtHandler::EvtHandler()
{
}
    
EvtHandler *EvtHandler::GetNextHandler()
{
    AbstractMethod("GetNextHandler");
    return 0;
}

EvtHandler *EvtHandler::FindNextHandlerOfClass(Class *cla)
{
    register EvtHandler *vp;
    
    for (vp= this; vp; vp= vp->GetNextHandler()) {
	if (vp->IsA()->isKindOf(cla))
		return vp;
    }
    return 0;
}

void EvtHandler::PerformCommand(Command *cmd)
{
    if (cmd && cmd != gNoChanges) {
	if (GetNextHandler())
	    GetNextHandler()->PerformCommand(cmd);
	else
	    cmd->Perform();
    }
}

class Menu *EvtHandler::GetMenu()
{ 
    if (GetNextHandler())
	return GetNextHandler()->GetMenu();
    return 0;
}

void EvtHandler::DoCreateMenu(class Menu *mp)
{
    if (GetNextHandler())
	GetNextHandler()->DoCreateMenu(mp);
}

void EvtHandler::DoSetupMenu(class Menu *mp)
{
    if (GetNextHandler())
	GetNextHandler()->DoSetupMenu(mp);
}

Command *EvtHandler::DoMenuCommand(int cmd)
{
    if (GetNextHandler())
	return GetNextHandler()->DoMenuCommand(cmd);
    return gNoChanges;
}

void EvtHandler::Send(int id, int part, void *val)
{
    Object::Send(id, part, val);
/*
    if (GetNextHandler())
	GetNextHandler()->Send(id, part, val);
*/
    if (GetNextHandler())
	GetNextHandler()->Control(id, part, val);
}

void EvtHandler::Control(int id, int part, void *val)
{
    if (GetNextHandler()) 
	GetNextHandler()->Control(id, part, val);
}

Command *EvtHandler::DoIdleCommand()
{
    if (GetNextHandler())
	return GetNextHandler()->DoIdleCommand();
    return gNoChanges;
}

void EvtHandler::SendDown(int, int, void*)
{
}

