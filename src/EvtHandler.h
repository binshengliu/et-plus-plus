#ifndef EvtHandler_First
#ifdef __GNUG__
#pragma once
#endif
#define EvtHandler_First

#include "Object.h"
#include "Port.h"

enum EvtFlags {
    eEvtDefault = eObjDefault,
    eEvtLast    = eObjLast + 0
};

class EvtHandler: public Object {
public:
    MetaDef(EvtHandler);
    
    EvtHandler();

    virtual EvtHandler *GetNextHandler();  
    EvtHandler *FindNextHandlerOfClass(Class *cla);
    
    virtual class Menu *GetMenu();
    virtual void DoSetupMenu(class Menu*);
    virtual void DoCreateMenu(class Menu*);
    virtual class Command *DoMenuCommand(int);
    virtual void PerformCommand(Command *);
    
    virtual Command *DoIdleCommand();
    virtual void Send(int id= cIdNone, int part= cPartAnyChange, void *val= 0);
    virtual void Control(int id, int part, void *val);
    virtual void SendDown(int id, int part, void *val);
};

#endif EvtHandler_First

