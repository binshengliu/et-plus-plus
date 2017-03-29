#ifndef Window_First
#ifdef __GNUG__
#pragma once
#endif
#define Window_First

#include "BlankWin.h"
#include "BorderItems.h"

enum WindowFlags {
    eWinCanClose    =   BIT(eBWinLast+1),
    eWinDestroy     =   BIT(eBWinLast+2),
    eWinDefault     =   eBWinDefault,
    eWinLast        =   eBWinLast + 2
};

class Window: public BlankWin {
    class Menu *menu;
    class WindowPort *MakePort();

public:
    MetaDef(Window);
    
    Window(EvtHandler*, Point e, WindowFlags f, VObject*, char *title= 0);
    ~Window();
    void Control(int id, int part, void *vp);
    Command *DispatchEvents(Point lp, Token t, Clipper *vf);
    
    //---- title related methods
    void SetTitle(char*, bool redraw= TRUE);
    char *GetTitle();

    //---- Menu related methods
    class Menu *GetMenu();
};

//---- WindowBorderItem --------------------------------------------------------

class WindowBorderItem: public AbstractBorderItem {
    bool fixed;
public:
    MetaDef(WindowBorderItem);
    
    WindowBorderItem(bool fixed, VObject *title, VObject *inner);
    
    Rectangle TitleRect();
    void Draw(Rectangle);
    Command *DoLeftButtonDownCommand(Point p, Token, int);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

#endif Window_First

