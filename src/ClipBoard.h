#ifndef ClipBoard_First
#ifdef __GNUG__
#pragma once
#endif
#define ClipBoard_First

#include "Object.h"

//---- clipboard types ---------------------------------------------------------

class ClipBoard : public Root {
protected:
    char *type;
    class View *view;   
    class membuf *mb;
    bool owner;

public:
    ClipBoard();
    virtual ~ClipBoard();
    
    void SelectionToClipboard(class View*);
    class Command *PasteClipboard(class View*);
    void NotOwner(char *type);
    char *GetBuf(u_long *size);
    bool CanPaste(class View*);
    char *GetType();
    membuf *GetBuf()
	{ return mb; }

    virtual void SetType(char *type);
    virtual char *DevGetType();
    virtual membuf *MakeBuf(char *type);
    virtual void ScratchChanged(char *t);
};

extern ClipBoard *gClipBoard;

#endif ClipBoard_First
