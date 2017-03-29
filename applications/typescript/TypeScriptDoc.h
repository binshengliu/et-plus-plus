#ifndef TypeScriptDoc_First
#define TypeScriptDoc_First

#include "Document.h"

class TypeScriptDoc : public Document {
    class ShellTextView *view;
    class Scroller *shScroller;
    class Text *text;
    Font *fd;
    int cnt;
public:
    MetaDef(TypeScriptDoc);
    
    TypeScriptDoc(int c);
    ~TypeScriptDoc();
	
    Window *DoMakeWindows();
    void DoWrite(ostream &s, int);
    void DoRead(istream &s, class FileType*);
    bool CanLoadDocument(class FileType*);

    bool Save()
	{ return TRUE; }
    bool Modified();
	
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);

    void Control(int id, int, void *);
};

#endif TypeScriptDoc_First

