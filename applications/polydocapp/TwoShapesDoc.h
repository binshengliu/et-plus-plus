#ifndef TwoShapesDoc_First
#define TwoShapesDoc_First

#include "Document.h"

//---- ShapeDocument -----------------------------------------------------

extern char *cDocTypeShapes;

class ShapeDocument: public Document {
    class ShapeView *view;
public:
    MetaDef(ShapeDocument);
    ShapeDocument();
    Window *DoMakeWindows();
    void DoWrite(ostream &s, int);
    void DoRead(istream &s, class FileType *);
};

#endif TwoShapesDoc_First

