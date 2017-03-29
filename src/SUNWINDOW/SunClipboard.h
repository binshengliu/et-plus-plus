#ifndef SunClipBoard_First
#define SunClipBoard_First

#include "ClipBoard.h"

class SunClipBoard: public ClipBoard {
    class ClipboardConnection *conn;

    void ScratchChanged(char *t);
    membuf *MakeBuf(char *type);
    char *DevGetType();
public:
    SunClipBoard();
    ~SunClipBoard();
};

#endif SunClipBoard_First
