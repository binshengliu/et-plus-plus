#ifndef BrowserItems_First
#define BrowserItems_First

#include "FileType.h"
#include "FileDialog.h"
#include "Expander.h"
#include "Mark.h"


//---- AbsoluteFileItem -------------------------------------------------

class AbsoluteFileItem: public FileItem {
    char *path;
public:
    MetaDef(AbsoluteFileItem);
    AbsoluteFileItem(char *name, char *path, bool shallow= FALSE);
    
    char *LookupName();
    char *AbsoluteName();
    void UpdateIcon();
};

//---- FileList -----------------------------------------------------------

class FileList: public Expander {
    class TextItem *title;
    class CompositeVObject *icon;
    class CollectionView *files;
public:
    MetaDef(FileList);
    
    FileList(int id);
    AbsoluteFileItem *GetSelectedItem();
    void Control(int id, int part, void *val);
    void SetupIcon();
    void SetContents(char *title, Collection *files);
    void Empty()
	{ SetContents("", 0); }
};

//---- FunctionMark -----------------------------------------------------

class FunctionMark: public Mark {
    char *name;
    bool method;
public:
    MetaDef(FunctionMark);
    FunctionMark(char *name, bool method, int p= 0, int l= 0);
    ~FunctionMark();
    char *GetName()
	{ return name; }
    bool IsMethod()
	{ return method; }
};

#endif BrowserItems_First
