#ifndef EditDoc_First
#define EditDoc_First

#include "Document.h"

class EditDoc : public Document {
    class TextView *view;
    class Text *text;
public:
    MetaDef(EditDoc);
    
    EditDoc();
    ~EditDoc();
	
    Window *DoMakeWindows();
    void DoRead(istream &, FileType *);
    void DoWrite(ostream &, int);
    bool CanImportDocument(class FileType *);
    class Command *DoImport(istream& s, class FileType *);
};

#endif EditDoc_First

