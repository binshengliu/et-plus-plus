//$ChangeDirDiag$

#include "ChangeDirDiag.h"
#include "DialogItems.h"
#include "BrowserView.h"
#include "BrowserDoc.h"
#include "BrowserCmdNo.h"
#include "Window.h"
#include "Document.h"
#include "CollView.h"
#include "Scroller.h"
#include "Alert.h"
#include "System.h"
#include "TextView.h"
 
const int cPathBuf      =   200,
	  cItemMinWidth =   300,
	  cMaxDirBuf    =     8;

//---- ChangeDirDiag -------------------------------------------------------------

MetaImpl(ChangeDirDiag, (TP(bvp), TP(ei1), TP(mylist), 0));


ChangeDirDiag::ChangeDirDiag(BrowserView *vp)
				    : Dialog("Change Directory", eBWinBlock)
{
    bvp= vp;
    mylist= new OrdCollection;
    collview= new CollectionView(this, mylist, eCVDontStuckToBorder);
    collview->SetMinExtent(Point(cItemMinWidth, 0));
    collview->SetId(cIdHistList);
    collview->SetContainer(this);
}

VObject *ChangeDirDiag::DoCreateDialog()
{
    //---- dialog parts ----
	
    VObject *actions=
	new Cluster (cIdNone, eVObjVBase, 10, 
	    new ActionButton(cIdOk, "Ok", TRUE),
	    new ActionButton (cIdCancel, "Cancel"),
	    0
	);
	
    ei1= new EditTextItem(cIdNone, "", cItemMinWidth);
    
    VObject *name= new BorderItem("New Directory",ei1);
	
    scroller= new Scroller(collview, Point(cItemMinWidth, 16*8), cIdHistList);

    //---- overall layout ----
    return
	new BorderItem(
	    new Cluster(cIdNone, eVObjHCenter, 20,
		scroller,
		name,
		actions,
		0
	    ),
	    20, 0
	);
}

void ChangeDirDiag::DoSetup()
{
    byte sel[30];
    BrowserDocument *bd= (BrowserDocument *) FindNextHandlerOfClass(Meta(BrowserDocument));
    
    EnableItem(cIdCancel);    
    if (bd) {
	bd->GetTextView()->SelectionAsString(sel,sizeof sel);
	if (sel[0]) {
	    ei1->SetString(sel);
	    ei1->SetSelection();
	}
    }
}

void ChangeDirDiag::Control(int id, int p, void *v)
{
    VObject *gop;
    char *fname;
    
    switch (id) {

    case cIdOk:
	if (ChangeDir())
	    break;
	else return;
    
    case cIdHistList:
	switch (p) {
	case cPartCollSelect:
	    gop= (VObject*) collview->GetCollection()->At( (int) v );
	    fname= gop->AsString();
	    if (fname && strlen(fname) > 0) {
		ei1->SetString((byte*)fname);
		ei1->SetSelection();
		DoSetup();
		SafeDelete(fname);
	    }
	    return;
	case cPartCollDoubleSelect:
	    if (ChangeDir()) 
		Dialog::Control(cIdOk, cPartAction, v);
	    else return;
	}
	
    default:
	break;
    }
    Dialog::Control(id, p, v); 
}

bool ChangeDirDiag::ChangeDir()
{
    Text *t;
    char pathname[cPathBuf];
    
    t= ei1->GetText();
    t->CopyInStr((byte*)pathname, cPathBuf, 0, t->Size());

    if (gSystem->ExpandPathName(pathname, cPathBuf-1)) {
	ShowAlert(eAlertNote, "%s (%s)", gSystem->GetErrorStr(), pathname);
	return FALSE;
    }
    if (!gSystem->ChangeDirectory(pathname)) {
	ShowAlert(eAlertNote, "Cannot change directory to @B%s@P", pathname);
	return FALSE;
    }
    UpdateHistory(pathname);
    bvp->ShowDirectory(-1,pathname);
    return TRUE;
}

void ChangeDirDiag::UpdateHistory(char *pathname)
{
    TextItem* ti,*told;

    ti= new TextItem(1,pathname, gSysFont, Point(4,0));
    if (told= (TextItem*)mylist->Find(ti))
	mylist->Remove(told);
    mylist->AddAt(0,ti);
    if (mylist->Size() > cMaxDirBuf)
	mylist->RemoveAt(cMaxDirBuf);
    collview->SetCollection(mylist,FALSE);
    SafeDelete(told);
}
