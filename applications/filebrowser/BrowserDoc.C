//$BrowserDocument$
#include "ET++.h"
#include "BrowserView.h"
#include "BrowserItems.h"
#include "BrowserDoc.h"
#include "CodeTextView.h"
#include "GapText.h"

//---- BrowserDocument --------------------------------------------------

const int cNumFilelists= 3;

MetaImpl(BrowserDocument, (TP(browserView), TP(textView), TP(text), T(nFileLists), 0));

BrowserDocument::BrowserDocument(int n) : Document(cDocTypeAscii)
{   
    // my external type is cDocTypeAscii, e.g. ordinary ascii files    
    text= new GapText(1000, gFixedFont); 
    nFileLists= range(1, 6, n);
}

BrowserDocument::~BrowserDocument()
{
    SafeDelete(textView);
    SafeDelete(text);
}

Window *BrowserDocument::DoMakeWindows()
{
    // create the Views I show
    browserView= new BrowserView(this, nFileLists);
    textView= new CodeTextView(this, Rectangle(Point(2000,cFit)), text);
   
    // create the window layout, the window's size is set to 0 and will
    // be set to its minimum width by ET++     
    return
	new Window(this, gPoint0, eWinDefault, 
	    new Expander(cIdNone, eVert, gPoint2,
		new BorderItem(browserView, gPoint8, 1, eVObjHCenter),
		new Scroller(textView, Point(500, 350)),
	    0),
	    "FileBrowser"
	);
}

void BrowserDocument::DoWrite(ostream &s, int)
{
    text->PrintOnAsPureText(s);
}

void BrowserDocument::DoRead(istream &s, FileType *ft)
{
    text->ReadFromAsPureText(s, ft->SizeHint());
    textView->SetText(text);
}

void BrowserDocument::Control(int id, int detail, void *data)
{
    // react on extern requests to load a file, the file to be loaded is
    // retrieved from the "v" parameter
    
    if (id == cIdLoadFile) {
	AbsoluteFileItem *fi= (AbsoluteFileItem*)data;
	if (Modified())
	    if (ShowAlert(eAlertCaution, "Save changes to @B%s@P ?", GetName()) == cIdYes) 
		if (!Save())
		    return;
	Load(fi->AbsoluteName(), TRUE, fi->GetType());
    }
    Document::Control(id, detail, data);
}
