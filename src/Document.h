#ifndef Document_First
#ifdef __GNUG__
#pragma once
#endif
#define Document_First

#include "EvtHandler.h"
#include "FileType.h"

enum FileDialogType {
    eFDTypeRead,
    eFDTypeWrite,
    eFDTypeImport
};

class Document : public EvtHandler {
protected:
    class Application *application;
    int changeCount, uniqueId;
    char *docName, *loadDir;
    const char *docType;
    Command *lastCmd;
    class Menu *menu;
    class ObjList *windows;
    bool isUntitled, isOpen, isConverted;
    class Icon *icon;
    bool *wasopen;
    bool makeBackup;
    class Window *window;
    
public:
    MetaDef(Document);

    Document(const char *documentType= cDocTypeUndef);
    ~Document();

    //---- dialogs
    virtual class FileDialog *MakeFileDialog(FileDialogType); 

    //---- document attributes
    int UniqueId()
	{ return uniqueId; }
    const char *GetDocumentType()
	{ return docType; }
    bool IsUntitled()
	{ return isUntitled; }
    char *GetName()
	{ return docName; }
    char *GetLoadDir()
	{ return loadDir; }
    virtual void SetName(char *);
    virtual void SetWindowTitle(char *);

    //---- change management
    virtual void SetChangeCount(int);
    virtual void InitChangeCount();
    int GetChangeCount()
	{ return changeCount; }
    virtual bool Modified();
    virtual bool AlertChanges();    

    //---- misc
    virtual void DoMakeViews();
    virtual bool CanLoadDocument(class FileType *type);
    virtual bool CanImportDocument(class FileType *type);
	// when called with type= 0, returns whether any documents can be imported
    void Control(int id, int part, void *vp);
	
    void SetApplication(class Application *);
    EvtHandler *GetNextHandler();

    void InspectorId(char *buf, int bufSize);
    void Parts(Collection*);

    //---- windows
    virtual void AddWindow(class BlankWin*);
    virtual void OpenWindows();
    class Window *GetWindow()
	{ return window; }
    void ShowDocument(Document *shown);
    virtual class Window *DoMakeWindows();

    //---- icons
    virtual void Toggle();
    virtual Icon *DoMakeIcon(char *name);

    //---- menues
    void DoCreateMenu(class Menu *);
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);
    void PerformCommand(Command*);
    class Menu *GetMenu();

    //---- generic menu commands
    virtual bool Open();
    virtual bool Close();
    virtual void Revert();
    virtual bool Save();
    virtual bool SaveAs();
    virtual Command *Import();
    virtual void Undo();
    virtual void Redo();

    //---- input/output
    virtual bool Load(char *name, bool unique, class FileType *doctype);
    virtual bool DoFileIsAlreadyOpen(Document *shown, char *name);
    virtual void Store(char *name, int option);
    virtual void MakeBackup(char *loadDir, char *docName);
    void EnableBackups(bool on);
    virtual void DoRead(istream &, FileType *);
    virtual class Command *DoImport(istream &, FileType *);
    virtual void DoWrite(ostream &, int option);
};

#endif Document_First

