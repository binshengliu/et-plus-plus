#ifndef XClipBoard_First
#define XClipBoard_First

#include "X11.h"
#include "ClipBoard.h"
#include "XWinSystem.h"

class XClipBoard: public ClipBoard {
    XWindow awin;
    Atom etprop, etformat;

    char *DevGetType();
    void ScratchChanged(char*);
    membuf *MakeBuf(char *type);
    void SendClipBoard(XSelectionRequestEvent*);
    long GetCurrentServerTime();
public:
    XClipBoard();
    ~XClipBoard();
    
    bool ProcessEvent(XEvent &ev);
};

#endif XClipBoard_First
