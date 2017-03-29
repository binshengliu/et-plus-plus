//$MiniEditDoc$
#include "ET++.h"

#include "CodeTextView.h"
#include "GapText.h"
#include "MiniEditDoc.h"

//---- MiniEditDoc ---------------------------------------------------------------

MetaImpl(MiniEditDoc, (TP(view), TP(text), 0));

MiniEditDoc::MiniEditDoc() : Document(cDocTypeAscii)
{
    text= new GapText(100, gFixedFont);
}

MiniEditDoc::~MiniEditDoc()
{
    SafeDelete(view);
    SafeDelete(text);
}

Window *MiniEditDoc::DoMakeWindows()
{   
    view= new CodeTextView(this, Rectangle(1000,cFit), text);

    return new Window(this, Point(560, 400), (WindowFlags)eBWinDefault,
	new Scroller(view)
    );
}
 
void MiniEditDoc::DoWrite(ostream &s, int)
{
    text->PrintOnAsPureText(s);
}

void MiniEditDoc::DoRead(istream &s, FileType *ft)
{
    text->ReadFromAsPureText(s, ft->SizeHint());
    view->SetText(text);
}
