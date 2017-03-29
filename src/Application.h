#ifndef Application_First
#ifdef __GNUG__
#pragma once
#endif
#define Application_First

#include "Dialog.h"

class Document;
extern const char *cDocTypeUndef;

class Application: public EvtHandler {
protected:
    static class FileDialog *fileDialog;

    char *version, **argv;
    const char *mainDocumentType;
    int argc;
    class ObjList *documents;
    class Menu *menu;
    class Dialog *applDialog;
    class ClipBoard *clipboard;
    Point lastDocPos;
    bool printHierarchy, inited;
    
    //---- globals
    ClassManager *gClassManager;
    class IdDictionary *gObservers;
    bool gBatch;
    bool gDebug;
    class BlankWin *gWindow;
    Rectangle gScreenRect; 
    char **gEnviron;
    
public:
    MetaDef(Application);
    Application(int ac, char** av, const char *mainDocType= cDocTypeUndef);
    ~Application();
    
    void ApplInit();

    //---- dialogs
    virtual FileDialog *MakeFileDialog();

    //---- application attributes
    char *GetName()
	{ return argv[0]; }
    const char *GetMainDocumentType()
	{ return mainDocumentType; }
    EvtHandler *GetNextHandler();

    //---- Command line arguments -------------------
    virtual int DoParseOptions(char *arg0, char *arg1);
    virtual void ParseCommandLine(int argc, char **argv);
    char *ProgramName();

    //---- Document related methods -----------------
    virtual Point GetNewDocumentPos();
    virtual void AddDocument(Document *);
    void RemoveDocument(Document *);
    virtual bool CloseAllDocuments();
	// return TRUE on success
    virtual void OpenDocument(char *);
    virtual Document *DoMakeDocuments(const char *type);
    void NewDocument(const char *type);
    virtual bool CanOpenDocument(class FileType *);
    Document *FindDocument(int id);

    //---- generic application commands
    virtual void About();
    virtual void Quit();
    virtual void Debug(int level);
	// 0 < level < 10
    virtual void Open();
    virtual void DoOnError(int level, char *location, char *msg);

    //---- menu related methods
    class Menu *GetMenu();
    Command *DoMenuCommand(int);
    void Control(int id, int, void*);
    void HandleApplicationCommands(int);
    
    //---- application window
    virtual class VObject *DoCreateDialog();
    virtual void OpenApplicationDialog(Point p);
    virtual void ShowApplicationWindow();

    //---- run the Application
    virtual int Run();

    //---- dynamic loading
    virtual int DynLoad(char *name);
    virtual int DynLoadHook(Object*);
    
    //---- inspector
    void InspectorId(char *buf, int sz);
    void Parts(Collection*);
    void UpdateGlobals();
};

extern bool         gBatch;
extern Application *gApplication;
extern ClipBoard   *gClipBoard;
extern char        *gProgname;

//---- Application Dialog -----------------------------------------------

class ApplDialog : public Dialog {
    Application *appl;
public:
    MetaDef(ApplDialog);
    ApplDialog(Application *eh, char *title= 0);
    VObject *DoCreateDialog();
    void Control(int id, int, void*);
    Point GetInitialPos();
};

#endif Application_First

