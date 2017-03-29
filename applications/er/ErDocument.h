#ifndef ErDocument_First
#define ErDocument_First

#include "Document.h"

//---- ErDocument --------------------------------------------------------------

extern char *cDocTypeER;

class ErDocument: public Document {
    class ErView *view;
    class SeqCollection *list;

public:
    MetaDef(ErDocument);
    
    ErDocument();
    ~ErDocument();
    
    Window *DoMakeWindows();
    
    //---- shapes
    SeqCollection *GetShapes()
	{ return list; }
    void SetShapes(SeqCollection *shapes, bool redraw);
    void AddShape(class ErShape *c);
    void RemoveShape(class ErShape *c);
    
    //---- menues
    Command *DoMenuCommand(int);
    void DoCreateMenu(Menu*);
    void DoSetupMenu(Menu*);

    //---- input/output
    void DoWrite(ostream &s, int o);
    void DoRead(istream &s, FileType *ft);
};

#endif ErDocument_First
       
