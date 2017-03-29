//$EditDoc$

#include "ET++.h"

#include "CodeTextView.h"
#include "GapText.h"
#include "EditDoc.h"

//---- EditDoc ----------------------------------------------------------

MetaImpl(EditDoc, (TP(view), TP(text), 0));

EditDoc::EditDoc() : Document(cDocTypeAscii)
{
    text= new GapText;
}

EditDoc::~EditDoc()
{
    SafeDelete(view);
    SafeDelete(text);
}

Window *EditDoc::DoMakeWindows()
{   
    view= new CodeTextView(this, Rectangle(1000,cFit), text);
    return new Window(this, Point(400), eWinDefault, new Scroller(view));
}

void EditDoc::DoRead(istream &s, FileType *ft)
{ 
      text->ReadFromAsPureText(s, ft->SizeHint());
      view->SetText(text);
}

void EditDoc::DoWrite(ostream &s, int)
{ 
    text->PrintOnAsPureText(s);
}

bool EditDoc::CanImportDocument(FileType *ft)
{
    if (ft == 0) // peek whether document can import any document types
	return TRUE;
    return strismember(ft->Type(), cDocTypeAscii, 0);
}

Command *EditDoc::DoImport(istream& s, FileType *ft)
{
    GapText t;
    
    t.ReadFromAsPureText(s, ft->SizeHint());
    return view->InsertText(&t);
}

