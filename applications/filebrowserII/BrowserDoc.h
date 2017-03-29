#ifndef BrowserDoc_First
#define BrowserDoc_First

#include "Document.h"

//---- BrowserDocument ---------------------------------------------------------------

class Text;
class BrowserView;
class FileBrowserTextView;
class Icon;
class TextItem;
class Window;
class ShellTextView;
class Scroller;
class FileType;

class BrowserDocument : public Document {
    BrowserView  *browserView;     // view on top of window
    FileBrowserTextView *textView; // text view below
    VObject *functionButton;

    Text *text, *shText;           // text installed in textView
    int   nFileLists;

    Icon *icon;
    TextItem *iconText;
    Window *shWin;
    ShellTextView *shell;
    Scroller *shScroller;

    bool  isCCode;
    
private: 

    void ShowShell(char *path);
    void ShowError(char *errMsg);
    void ShellExecute(char* cmd);
    void LoadFile(char *name, FileType *);
    bool SavedChanges();
    Document *IsAlreadyOpen(FileType *);
    void ApplyPreferences();
    void ExecuteMake();
    Text *MakeText();
    void InstallText(Text *, bool);  
    void ShowReverted(Text *, FileType *); 
     
public:
    MetaDef(BrowserDocument);
    
    BrowserDocument(int nFileList= 3);
    ~BrowserDocument();
    
    Window *DoMakeWindows();
    Icon   *DoMakeIcon (char *name);
    
    class TextView *GetTextView(); 
	
    //---- input/output
    void DoRead(istream &, class FileType *);
    void DoWrite(ostream &, int);
    bool CanImportDocument(FileType *);
    Command *DoImport(istream& s, FileType *);
    bool DoFileIsAlreadyOpen(Document *shown, char *name);
    void SetSharedWindowTitle();
    
    //---- sharing "changeCount"
    bool AlertChanges();
    void SetChangeCount(int delta);
    void InitChangeCount();
	
    //---- event handling
    void Control(int id, int, void *);

    //---- menus
    void DoCreateMenu(Menu *menu);
    Command *DoMenuCommand(int cmd);
    void DoSetupMenu(Menu *menu);    
};

#endif BrowserDoc_First
