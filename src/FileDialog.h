#ifndef FileDialog_First
#ifdef __GNUG__
#pragma once
#endif
#define FileDialog_First
 
#include "Dialog.h"
#include "VObjectPair.h"

//---- class Path ------------------------------------------------------------

class Path {    // pathname scanner
    char *path;
    char *p;
public:
    Path();
    ~Path();
    char *operator()();
    const char *GetPath();
    void Reset();
    void Start();
    char *At(int);
    int Components();
};

//---- FileDialog --------------------------------------------------------------

const int cMaxPathName= 400;

//---- id's 

const int cIdName       =   cIdFirstUser + 110,
	  cIdList       =   cIdFirstUser + 111,
	  cIdUpdate     =   cIdFirstUser + 112,
	  cIdPath       =   cIdFirstUser + 113,
	  cIdComponent  =   cIdFirstUser + 120;

enum FileDialogFlags {
    eFDRead,
    eFDWrite,
    eFDImport
};

class FileDialog : public Dialog {
protected:
    class CompositeVObject *scroller;
    class EditTextItem *eti;
    class TextItem *title;
    char *pathname, *initDir;
    FileDialogFlags flags;
    class CollectionView *collview;
    class OrdCollection *fileList;
    EvtHandler *doc;
    class FileType *doctype;
    class Menu *pathMenu;
    class PopupItem *pathPopup;
    Path path;
    
protected:
    void Control(int id, int, void *v);
    Command *DispatchEvents(Point lp, Token t, Clipper*);
    int ItemHeight();
    virtual void UpdateList();
    virtual void UpdatePath();
    virtual bool OpenOrChangeDir();
    virtual VObject *MakeFileItem(char *name);

public:
    MetaDef(FileDialog);
    
    FileDialog(char *title= "File Dialog");
    ~FileDialog();
    
    int ShowInWindow(FileDialogFlags, Clipper*, EvtHandler *eh= 0, char *msg= 0);
    char *FileName()
	{ return pathname; }
    class FileType *GetDocType()
	{ return doctype; }
    virtual bool NotWritable(char *name);
    virtual bool NotReadable(char *name);
    virtual bool WrongType();
    virtual bool ChangeDirectory();
    
    VObject *DoCreateDialog();
    void DoSetup();
    
    //---- Hooks
    virtual VObject *Hook(FileDialogFlags);
    virtual int GetSaveOption();
};

//---- FileItem --------------------------------------------------------------

class FileItem: public VObjectPair {
protected:
    TextItem *label;
    ImageItem *icon;
    class FileType *type;

protected:
    bool shallow;
    virtual char *LookupName();
    virtual void UpdateIcon();
    
public:
    MetaDef(FileItem);
    FileItem(char *name, bool shallow= TRUE);
    ~FileItem();
    //---- types

    FileType *GetType()
	{ return type; }
    bool IsDirectory();
    
    char *Name()
	{ return label->AsString(); }
    
    char *AsString ();
    void Draw(Rectangle r);
};

extern const Point cIconSize;
extern Bitmap *gDirectoryIcon, *gFileIcon, *gExecIcon, *gAsciiIcon;
	    
#endif FileDialog_First

