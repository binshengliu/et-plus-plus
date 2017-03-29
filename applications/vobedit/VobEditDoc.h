#ifndef VobEditDoc_First
#define VobEditDoc_First

#include "Document.h"

//---- VobEditDoc ---------------------------------------------------------------

extern char *cVobDocType;

class VobEditDoc : public Document {
    class TextView *view;
    class VObjectText *text;
public:
    MetaDef(VobEditDoc);
    VobEditDoc();
    ~VobEditDoc();
    
    Window *DoMakeWindows();
    Command *DoMenuCommand(int);  
    void DoWrite(ostream&, int);
    void DoRead(istream&, class FileType*);  
    bool CanLoadDocument(class FileType*);
    class MenuBar *CreateMenuBar();
};

#endif VobEditDoc_First
