#ifndef Icon_First
#ifdef __GNUG__
#pragma once
#endif
#define Icon_First

#include "BlankWin.h"

//---- Icon --------------------------------------------------------------------
  
class Icon: public BlankWin {
    class Menu *menu;
public:
    MetaDef(Icon);
    Icon(EvtHandler *eh, VObject *v);
    ~Icon();
    
    Command *DoLeftButtonDownCommand(Point, Token, int);
    class Menu *GetMenu();
    void DoCreateMenu(class Menu *);

    virtual void UpdateIconLabel(char *label);
};

#endif Icon_First
