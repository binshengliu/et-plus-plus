#ifndef MiniEditDoc_First
#define MiniEditDoc_First

#include "Document.h"
           
class MiniEditDoc : public Document {
    class TextView *view;
    class Text *text;
public:
    MetaDef(MiniEditDoc);
    
    MiniEditDoc();
    ~MiniEditDoc();
	
    Window *DoMakeWindows();
    
    void DoRead(istream &, class FileType *);
    void DoWrite(ostream &, int);
};

#endif MiniEditDoc_First

