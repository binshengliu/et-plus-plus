#ifndef DrawController_First
#define DrawController_First

#include "Object.h"

//---- DrawController --------------------------------------------------------
//     Helper class for reusing Draw as a building block independent from a
//     specific document type

class DrawController: public Object {
    class DrawView *drawView;
public:
    MetaDef(DrawController);
    DrawController(class DrawView *);
    virtual class MenuBar *CreateMenuBar(class EvtHandler *next);
    virtual class Command *DoMenuCommand(int cmd);
    
    //---- input/output
    virtual class ObjList *ReadShapes(istream &from);
    virtual void WriteShapes(ostream &from);
    
    virtual Command *DoImport(istream &s, FileType *ft);
};

#endif DrawController_First

