#ifndef TwoViewDoc_First
#define TwoViewDoc_First

#include "Document.h"

class TwoViewDoc : public Document {
    class TextView *view1, *view2;
    class Text *text;
public:
    MetaDef(TwoViewDoc);

    TwoViewDoc();
    ~TwoViewDoc();
	
    Window *DoMakeWindows();
    void DoRead(istream &, FileType *);
    void DoWrite(ostream &, int);
};

#endif TwoViewDoc_First

