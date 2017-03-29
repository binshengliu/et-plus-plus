//$TwoViewDoc$
#include "ET++.h"

#include "CodeTextView.h"
#include "GapText.h"

#include "TwoViewDoc.h"

//---- TwoViewDoc ---------------------------------------------------------------

MetaImpl(TwoViewDoc, (TP(view1), TP(view2), TP(text), 0));

TwoViewDoc::TwoViewDoc() : Document(cDocTypeAscii)
{
    text= new GapText((byte*)"One text shown in two\nviews");
}

TwoViewDoc::~TwoViewDoc()
{
    SafeDelete(view1);
    SafeDelete(view2);
    SafeDelete(text);
}

Window *TwoViewDoc::DoMakeWindows()
{   
    view1= new TextView(this, Rectangle(Point(400,cFit)), text, eCenter);
    view2= new TextView(this, Rectangle(Point(1000,cFit)), text);
    
    return new Window(this, Point(700, 400), (WindowFlags)(eBWinDefault),
	new Expander(cIdNone, eHor, gPoint2, 
	    new Scroller(view1),
	    new Splitter(view2),
	    0
	)
    );
}
 
void TwoViewDoc::DoRead(istream &s, FileType *ft)
{ 
    text->ReadFromAsPureText(s, ft->SizeHint());
    view1->SetText(text);
    view2->SetText(text);
}

void TwoViewDoc::DoWrite(ostream &s, int)
{ 
    text->PrintOnAsPureText(s);
}
