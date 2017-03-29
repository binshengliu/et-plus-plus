#ifndef CmdHistDoc_First
#ifdef __GNUG__
#pragma once
#endif
#define CmdHistDoc_First

#include "Document.h"

//--- CmdHistDocument ----------------------------------------------------------

class CmdHistDocument: public Document {
    ObjList *cmdHistory;
public:
    MetaDef(CmdHistDocument);
    CmdHistDocument(const char *doctype= cDocTypeUndef);
    ~CmdHistDocument();

    //---- menues
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);

    void PerformCommand(Command* cmd);
    void Undo();
    void Redo();
    void DeleteCmdHistory();
};

#endif CmdHistDoc_First

