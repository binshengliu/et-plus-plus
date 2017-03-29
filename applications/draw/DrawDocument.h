#ifndef DrawDocument_First
#define DrawDocument_First

#include "Document.h"

extern char *cDocTypeDraw;

class DrawDocument : public Document {
    class ObjArray *prototypes;
    class ObjList *tools;
    class VObject *paletteFrame;
    class DrawView *drawView;
    class DrawController *drawController;
    class CollectionView *paletteView;
    class ObjList *shapes;
    class Shape *textPrototype;
    class TextItem *info;
    
public:
    MetaDef(DrawDocument);
    
    DrawDocument(ObjArray*, ObjList*);
    ~DrawDocument();
    
    Window *DoMakeWindows();
    Command *DoMenuCommand(int cmd);
    class MenuBar *CreateMenuBar();
    void DoRead(istream &from, FileType *type);
    void DoWrite(ostream &from, int opt);
    bool CanLoadDocument(FileType *);
    bool CanImportDocument(class FileType *ft);
    Command *DoImport(istream &, class FileType *ft);
    void Control(int id, int part, void *val);
};

#endif DrawDocument_First

