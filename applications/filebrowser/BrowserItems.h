#ifndef BrowserItems_First
#define BrowserItems_First

#include "FileType.h"
#include "FileDialog.h"
#include "Expander.h"


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
    TextItem *title;
    class CollectionView *files;

public:
    MetaDef(FileList);
    
    FileList(int id);
    AbsoluteFileItem *GetSelectedItem();
    void SetContents(char *title, Collection *files);
    void Empty()
	{ SetContents("", 0); }
};

#endif BrowserItems_First
