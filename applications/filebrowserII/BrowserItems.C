//$FileList, AbsoluteFileItem, FunctionMark$

#include "DialogItems.h"
#include "VObjectPair.h"
#include "BrowserItems.h"
#include "Preferences_e.h"
#include "CollView.h"
#include "Scroller.h"
#include "System.h"
#include "BrowserCmdNo.h"
#include "Menu.h"

const int cListMinWidth= 160,
	  cListMaxLines= 7;

//---- bitmap images for file icons --------------------------------------------

static u_short EtBits[]= {
#   include "images/et++.im"
};

static Bitmap *gETIcon;

//---- AbsoluteFileItem --------------------------------------------------------

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
    if (!Preferences::UseFastIcons()) {
	type->DeepenShallowType();
	Enable(type->IsAscii() || IsDirectory());
	if (type->IsAscii()) 
	    icon->SetBitmap(gAsciiIcon, FALSE);
	if (type->IsETFormat()) {
	    if (gETIcon == 0)
		gETIcon= new Bitmap(cIconSize, EtBits);
	    icon->SetBitmap(gETIcon, FALSE);
	}
    }
}

//---- FileList ----------------------------------------------------------------

static u_short shellBits[]= {
#   include "images/shell.im"
};
static u_short noneBits[]= {
#   include "images/none.im"
};

static Bitmap *shellIcon, *noneIcon; 

MetaImpl(FileList, (TP(files), TP(title), TP(icon), 0));

FileList::FileList(int id) : Expander(cIdNone, eVert, gPoint0, (Collection*)0)
{
    if (shellIcon == 0)
	shellIcon= new Bitmap(16, shellBits);
    VObject *vop= new VObjectPair(
	icon= new Button(cIdShowShell, new ImageItem(shellIcon, 13)),
	title= title= new TextItem("")
	);
    vop->SetFlag(eVObjVFixed);

    Add(vop);
    files= new CollectionView(this, 0, eCVDontStuckToBorder);
    files->SetId(id);
    
    files->SetMinExtent(Point(cListMinWidth, 0));
    Add(new Scroller(files, Point(cListMinWidth, cListMaxLines*cIconSize.y)));
}

void FileList::Control(int id, int part, void *val)
{
    if (id == cIdShowShell) { 
	Control(files->GetId(), cIdShowShell, 0);
    }
    else
	Expander::Control(id, part, val);
}

void FileList::SetContents(char *t, Collection *col)
{
    icon->Enable(col != 0);
    title->SetString(t, TRUE);          // TRUE->redraw title
    files->SetCollection(col, FALSE);   // FALSE->do not free old collection
} 
    
void FileList::SetupIcon()
{
    bool empty= files->GetCollection() == 0;
    ImageItem *im= (ImageItem *)icon->At(0);
    icon->Enable(!empty);
    if (empty) {
	if (noneIcon == 0)
	    noneIcon= new Bitmap(16, noneBits);
	im->SetBitmap(noneIcon);
    } else
	im->SetBitmap(shellIcon);
}
    
AbsoluteFileItem *FileList::GetSelectedItem()
{
    int at= files->GetSelection().origin.y;
    
    files->SetNoSelection();
    return Guard(files->GetCollection()->At(at), AbsoluteFileItem);
}

//---- FunctionMark -------------------------------------------------------

MetaImpl(FunctionMark, (TP(name), TB(method), 0));
    
FunctionMark::FunctionMark(char *n, bool m, int p, int l) : Mark(p, l)
{
    name= 0;
    strreplace(&name, n);
    method= m;
}

FunctionMark::~FunctionMark()
{
    SafeDelete(name);
}

