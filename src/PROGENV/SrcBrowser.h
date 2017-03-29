#ifndef SrcBrowser_First
#define SrcBrowser_First

#include "Document.h"
#include "CodeTextView.h"
#include "Mark.h"

//---- SourceBrowser ------------------------------------------------------------

class SourceBrowser: public Document {
    class SrcBrowserTextView *textview;
    class ClassListView *listview;
    class MethodBrowser *methods;
    class Implementors *implementors;
    class TextItem *implTitle;
    class Text *text;
    Class *ccl;
    
    bool isCCode, definition, base;
    class OrdCollection *log;

    void DoSetClass(Class *, bool decl, bool unique, bool reveal);
    char *FirstWordOfSelection();
public:
    MetaDef(SourceBrowser);
    SourceBrowser();
    ~SourceBrowser();

    Window *DoMakeWindows();
    bool Open();
    void SetWindowTitle(char *name);

    void DoRead(istream &s, FileType *);
    void DoWrite(ostream &s, int);
    bool CanImportDocument(FileType *);
    Command *DoImport(istream& s, FileType *);
    void Parts(Collection *col);

    void SetClass(Class *, bool decl, bool reveal= TRUE);
    void RevealMethod(class MethodReference *mr);
    void RevealAndSelectLine(int l);
    void SelectSourceLine(bool redraw);
    bool FileOfClass(Class *cl, char *fname, bool decl);
    void SetMode(bool decl);
    Class *GetClass()
	{ return ccl; }
    bool GetMode()
	{ return definition; }

    //---- menus
    void DoCreateMenu(Menu *menu);
    Command *DoMenuCommand(int cmd);
    void DoSetupMenu(Menu *menu);
    
    void Control(int id, int part, void *val);

    //--- commands
    void Log(Class *cl);
    void Spawn();
    void EditSelectedClass();
    void ShowImplementors();
    void InspectSomeInstance();
};

//---- SrcBrowserTextView ---------------------------------------------------------

class SrcBrowserTextView: public CodeTextView {
public:
    MetaDef(SrcBrowserTextView);
    SrcBrowserTextView(EvtHandler *eh, Rectangle r, Text *t);    

    class PrettyPrinter *MakePrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps);

    void ShowMethod(class MethodReference *mr);
};

class SrcBrowserPPrinter: public PrettyPrinter {
    SrcBrowserTextView *tvp;
public:
    SrcBrowserPPrinter(SrcBrowserTextView *tvp, Text *t, Style *cs, Style *fs, Style *cds, Style *ps);
    void Function(int line, int start, int end, char *name, char *classname);
};

//---- MethodMark ---------------------------------------------------------

class MethodMark: public Mark {
public:
    char *className;
    char *method;
    
    MetaDef(MethodMark);
    MethodMark(char *classname, char *method, int p= 0, int l= 0);
    ~MethodMark();
};

#endif SrcBrowser_First

