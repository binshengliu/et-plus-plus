#ifndef BrowserView_First
#define BrowserView_First

#include "Dialog.h"
#include "BrowserItems.h"
#include "ByteArray.h"
#include "SeqColl.h"
#include "ShellTView.h"
#include "CodeTextView.h"

//---- BrowserView ---------------------------------------------------------

class BrowserView: public DialogView {
protected:
    Menu *menu;
    SeqCollection *path, *directories;    
    class CompositeVObject *fileLists;  // shown file lists
    int nShown;                         // number of shown file lists
    int left;                           // index of left most file list in "path"
    VObject *shiftLeft, *shiftRight;    // buttons
    class ChangeDirDiag *changeDir;    

    void LoadFile(int at, class FileList *fl);
    void Shell(int at, char *path, char *cmd= 0);
    
public:
    MetaDef(BrowserView);
    BrowserView(EvtHandler *dp, int numFilelists);
    ~BrowserView();
    
    //---- layout
    VObject *DoCreateDialog();
    
    //---- respond to user input
    void Control(int id, int detail, void *data);
    void DoSetup();
   
    //---- directory handling
    Collection *ReadDirectory(char *name);
    void ShowDirectory(int at, char *name);
    void ShowParentDirectory();
    void ShiftFileLists(int to);
    void UpdateButtons();
    
    //---- accessing path and directories by index
    FileList *FileListAt(int i)
	{ return (FileList*)fileLists->At(i); }
    char *PathNameAt(int i);
    char *DirectoryNameAt(int i);
    Collection *PathAt(int i)
	{ return (Collection*)path->At(i); }

    void RemoveAndFreeLast(SeqCollection *col);
    Menu *GetMenu();

};

//---- BrowserShellTView ------------------------------------------------

class BrowserShellTView: public ShellTextView {
    class Menu *menu;
    class EvtHandler *receiver;
public:
    MetaDef(BrowserShellTView);

    BrowserShellTView(EvtHandler *eh, Rectangle r, Text *t, char *name, char **args);
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);
    void DoCreateMenu(Menu *menu);
    Menu *GetMenu();
};

//---- FileBrowserTextView ---------------------------------------------------------

class FileBrowserTextView: public CodeTextView {
    class Menu *functionMenu;

public:
    MetaDef(FileBrowserTextView);
    FileBrowserTextView(EvtHandler *eh, Rectangle r, Text *t);    
    ~FileBrowserTextView();   

    class PrettyPrinter *MakePrettyPrinter(Text *t, Style *cs, Style *fs, Style *cds, Style *ps);

    void UpdateFunctionMenu();
    void GotoFunction(int i);
    Menu *GetFunctionMenu();
    void ResetFunctionMenu(); 
    void SetupFunctionMenu(); 
};

class FileBrowserPPrinter: public PrettyPrinter {
    FileBrowserTextView *tvp;
public:
    FileBrowserPPrinter(FileBrowserTextView *tvp, Text *t, Style *cs, Style *fs, Style *cds, Style *ps);
    void End();
    void ClassDecl(int, int start, int end, char *);
    void Function(int line, int start, int end, char *name, char *classname);
};

#endif BrowserView_First
