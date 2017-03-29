#ifndef BrowserDoc_First
#define BrowserDoc_First

#include "Document.h"

//---- BrowserDocument ---------------------------------------------------------------

class BrowserDocument : public Document {
    class BrowserView *browserView;  // view on top of window
    class TextView *textView;        // text view below
    class Text *text;                // text installed in textView
    int   nFileLists;
public:
    MetaDef(BrowserDocument);
    
    BrowserDocument(int nFileList= 3);
    ~BrowserDocument();
    
    Window *DoMakeWindows();
    
    //---- input/output
    void DoRead(istream &, class FileType *);
    void DoWrite(ostream &, int);
    
    //---- event handling
    void Control(int id, int, void *);
};

#endif BrowserDoc_First
