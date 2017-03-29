//$FileList, AbsoluteFileItem$

#include "DialogItems.h"
#include "BrowserItems.h"
#include "CollView.h"
#include "Scroller.h"
#include "System.h"

const int cListMinWidth= 160,
	  cListMaxLines= 7;

//---- bitmap images for file icons --------------------------------------------

static u_short EtBits[]= {
#   include "images/et++.im"
};

static Bitmap *gETIcon;

//---- AbsoluteFileItem -----------------------------------------------------------

MetaImpl(AbsoluteFileItem, (TP(path), 0));

AbsoluteFileItem::AbsoluteFileItem(char *f, char *p, bool s) : FileItem(f, s)
{
    path= p;
}

char *AbsoluteFileItem::AbsoluteName()
{
    if (strcmp(path, "/") != 0) // avoid "//name"
	return form("%s/%s", path, Name());
    return form("/%s", Name());
}

char *AbsoluteFileItem::LookupName()
{
    return AbsoluteName();
}

void AbsoluteFileItem::UpdateIcon()
{
    FileItem::UpdateIcon();
    type->DeepenShallowType();
    Enable(type->IsAscii() || IsDirectory());
    if (type->IsAscii()) 
	icon->SetBitmap(gAsciiIcon, FALSE);
    if (type->IsETFormat()) {
	if (gETIcon == 0)
	    gETIcon= new Bitmap(16, EtBits);
	icon->SetBitmap(gETIcon, FALSE);
    }
}

//---- FileList -----------------------------------------------------------

MetaImpl(FileList, (TP(files), TP(title), 0));

FileList::FileList(int id) : Expander(cIdNone, eVert, gPoint0, (Collection*)0)
{
    Add(title= new TextItem(""));
    files= new CollectionView(this, 0, eCVDontStuckToBorder);
    files->SetId(id);
    // set minimum width of the CollectionView to the width of the scroller
    files->SetMinExtent(Point(cListMinWidth, 0));
    Add(new Scroller(files, Point(cListMinWidth, cListMaxLines*cIconSize.y)));
}

void FileList::SetContents(char *t, Collection *col)
{
    title->SetString(t, TRUE);          // TRUE->redraw title
    files->SetCollection(col, FALSE);   // FALSE->do not free old collection
} 

AbsoluteFileItem *FileList::GetSelectedItem()
{
    int at= files->GetSelection().origin.y;
    
    files->SetNoSelection();
    return Guard(files->GetCollection()->At(at), AbsoluteFileItem);
}
