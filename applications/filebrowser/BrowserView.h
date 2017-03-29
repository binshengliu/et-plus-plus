#ifndef BrowserView_First
#define BrowserView_First

#include "Dialog.h"
#include "BrowserItems.h"
#include "ByteArray.h"
#include "SeqColl.h"

//---- VObject ids ---------------------------------------------------------

const int cIdLoadFile  =  cIdFirstUser;
// request passed with Control to next event handler to load a file

//---- BrowserView ---------------------------------------------------------

class BrowserView: public DialogView {
    SeqCollection *path, *directories;    
    class CompositeVObject *fileLists;  // shown file lists
    int nShown;                         // number of shown file lists
    int left;                           // index of left most file list in "path"
    VObject *shiftLeft, *shiftRight;    // buttons

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
};

#endif BrowserView_First
