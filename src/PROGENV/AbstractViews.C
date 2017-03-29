//$DefaultViews$

#include "ProgEnv.h"
#include "Window.h"
#include "Scroller.h"
#include "ImageItem.h"
#include "String.h"

#include "CollTblView.h"
#include "Collection.h"

#include "Dictionary.h"
#include "OrdColl.h"

class DefaultViews: public AbstractViewPolicy {
    Window *collWindow, *dictWindow, *bitmapWindow;
    CollTableView *collView, *dictView;
    ImageItem *bitmap;
public:
    DefaultViews();
    ~DefaultViews();
    char *ViewName(Class *cl, void *addr, Object *op);
    bool ShowView(Class *cl, void *addr, Object *op, Object *);
    void ShowVObjectTree(VObject*);
    void ShowDictionaryView(Dictionary*);
    void ShowCollectionView(Collection*);   
    void ShowBitmap(Bitmap *bm);
};

DefaultViews::DefaultViews()
{
    collWindow= dictWindow= bitmapWindow =0;
    static u_short dummydata[3];
    Bitmap *dummy= new Bitmap(1, dummydata);
    bitmap= new ImageItem(dummy);
}

DefaultViews::~DefaultViews()
{
}

char *DefaultViews::ViewName(Class *cl, void *, Object *op)
{
    if (!op)
	return 0;
    if (op->IsKindOf(VObject)) 
	return "VObject-Tree view";
    if (op->IsKindOf(Dictionary) && ((Dictionary*)op)->Size()) 
	return "Dictionary view";
    if (op->IsKindOf(Collection) && ((Collection*)op)->Size()) 
	return "Collection view";
    if (cl && strcmp(cl->Name(), "Bitmap") == 0)
	return "BitmapView";
    return 0;
}

bool DefaultViews::ShowView(Class *cl, void *addr, Object *op, Object *)
{
    if (op->IsKindOf(VObject)) {
	ShowVObjectTree((VObject*)op);
	return TRUE;
    }
    if (op->IsKindOf(Dictionary)) {
	ShowDictionaryView((Dictionary*)op);
	return TRUE;
    } 
    if (op->IsKindOf(Collection)) {
	ShowCollectionView((Collection*)op); 
	return TRUE;
    }
    else if (cl && strcmp(cl->Name(), "Bitmap") == 0) {
	ShowBitmap((Bitmap*)addr);
	return TRUE;
    }
    return FALSE;  
}

void DefaultViews::ShowVObjectTree(VObject*vop)
{
    VObject *root= 0;
    while (vop) {
	root= vop;
	vop= vop->GetContainer();
    }
    if (root)
        gProgEnv->ShowInObjStruBrowser(root);            
}

void DefaultViews::ShowCollectionView(Collection* col)
{
    if (collWindow == 0) {
	collView= new CollTableView();        
	collWindow= new Window(0, Point(300, 100),
	    (WindowFlags)(eWinDefault+eWinCanClose),
		    new Scroller(collView), "Collection");
    }
    collWindow->OpenAt(Point(800,500));
    collWindow->SetTitle(form("Collection: 0x%x (%s)", (int)col, col->ClassName()), TRUE);
    collView->ShowCollection(col);
}
   
void DefaultViews::ShowDictionaryView(Dictionary* col)
{
    if (dictWindow == 0) { 
	dictView= new DictionaryView();        
	dictWindow= new Window(0, Point(600, 100),
	    (WindowFlags)(eWinDefault+eWinCanClose),
		    new Scroller(dictView), "Dictionary");
    } 
    dictWindow->OpenAt(Point(800,500));
    dictWindow->SetTitle(form("Dictionary: 0x%x (%s)", (int)col, col->ClassName()), TRUE);
    dictView->ShowCollection(col);
}
   
void DefaultViews::ShowBitmap(Bitmap *bm)
{
    Bitmap *bm2= (Bitmap*)bm->DeepClone();
    if (bitmapWindow == 0) { 
	bitmapWindow= new Window(0, Point(250, 200),
	    (WindowFlags)(eWinDefault+eWinCanClose),
		    new Scroller(bitmap), "");
    } 
    bitmapWindow->OpenAt(Point(800,500));
    bm= bitmap->GetBitmap();
    SafeDelete(bm);
    bitmap->SetBitmap(bm2);
    bitmapWindow->SetTitle(form("Bitmap: w: %d h: %d d: %d", 
				     bm2->Size().x, bm2->Size().y, bm2->GetDepth()));
    bitmapWindow->UpdateEvent();
}

void AbstractViewOnEntry()
{
    if (gProgEnv)
	gProgEnv->AddAbstractViewPolicy(new DefaultViews);
}

