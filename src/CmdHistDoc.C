//$CmdHistDocument$

#include "CmdHistDoc.h"
#include "ObjList.h"
#include "Menu.h"
#include "CmdNo.h"
#include "Window.h"

//--- CmdHistDocument ----------------------------------------------------------

MetaImpl(CmdHistDocument, (TP(cmdHistory), 0));

CmdHistDocument::CmdHistDocument(const char *doctype) : Document(doctype)
{
    cmdHistory= new ObjList;
}

CmdHistDocument::~CmdHistDocument()
{
    cmdHistory->FreeAll();
    SafeDelete(cmdHistory);
}

void CmdHistDocument::DoCreateMenu(class Menu *menu)
{
    Document::DoCreateMenu(menu);
    menu->InsertItemAfter(cUNDO, "redo", cREDO);
}

void CmdHistDocument::DoSetupMenu(Menu *menu)
{
    Document::DoSetupMenu(menu);

    if (changeCount > 0) {
	Command *lastCmd= (Command*) cmdHistory->GetAt(changeCount-1);
	menu->ReplaceItem(cUNDO, lastCmd->GetUndoName());
	menu->EnableItem(cUNDO);
    } else {
	menu->ReplaceItem(cUNDO, "undo");
	menu->DisableItem(cUNDO);
    }
    
    if (changeCount < cmdHistory->Size()) {
	Command *nextCmd= (Command*) cmdHistory->GetAt(changeCount);
	menu->ReplaceItem(cREDO, nextCmd->GetUndoName());
	menu->EnableItem(cREDO);
    } else {
	menu->ReplaceItem(cREDO, "redo");
	menu->DisableItem(cREDO);
    }
}

void CmdHistDocument::PerformCommand(Command* cmd)
{
    int i;
    
    // don't do anything on gNoChanges!!
    if (cmd && (cmd != gNoChanges) && (cmd->GetId() || cmd == gResetUndo)) {
	if (cmd != gResetUndo) {
	    for (i= changeCount; i < cmdHistory->Size(); i++)
		cmdHistory->RemovePtr(cmdHistory->GetAt(i));
		
	    if (cmdHistory->GetAt(changeCount-1) != cmd) {
		cmdHistory->Add(cmd);
		changeCount+= cmd->Do();
	    }
	} else {
	    // commit all commands
	    Iter next(cmdHistory);
	    Command *c;
	    
	    for (i= 0; c= (Command*) next(); i++) {
		cmdHistory->RemovePtr(c);
		if (i < changeCount)
		    c->Finish((Command*)cmdHistory->GetAt(i));
	    }
	    cmdHistory->Empty(0);
	    changeCount= 0;
	}
	lastCmd= cmd;
    }
    if (window)
	window->UpdateEvent();
}

void CmdHistDocument::Undo()
{
    lastCmd= (Command*) cmdHistory->GetAt(changeCount-1);
    Document::Undo();
}

void CmdHistDocument::Redo()
{
    lastCmd= (Command*) cmdHistory->GetAt(changeCount);
    Document::Undo();
}

void CmdHistDocument::DeleteCmdHistory()
{
    Iter next(cmdHistory);
    Command *c;
    
    while (c= (Command*) next()) {
	cmdHistory->RemovePtr(c);
	delete c;
    }
    cmdHistory->Empty(0);
}
