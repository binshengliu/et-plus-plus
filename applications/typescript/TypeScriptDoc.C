//$TypeScriptDoc, myScroller$

#include "ET++.h"
#include "System.h"
#include "ShellTView.h"
#include "StyledText.h"

#include "TypeScriptDoc.h"

const int cTYPESCRIPTMENU= cUSERCMD+100;

//---- TypeScriptDoc ----------------------------------------------------------

MetaImpl(TypeScriptDoc, (TP(view), TP(text), T(cnt), 0));

TypeScriptDoc::TypeScriptDoc(int c): Document(cDocTypeAscii)
{
    cnt= c;
    fd= gFixedFont;
    if (Getenv("ET_NO_STYLEDCODE"))
	text= new GapText(256, fd);
    else
	text= new StyledText(256, fd);
}

TypeScriptDoc::~TypeScriptDoc()
{
    SafeDelete(view); 
    SafeDelete(text);
}

Window *TypeScriptDoc::DoMakeWindows()
{   
    static char *argv[] = { "/bin/csh", 0 };
    char *shell= Getenv("SHELL");
    if (shell)
	argv[0]= shell;
    view= new ShellTextView(this, Rectangle(Point(1000,cFit)), text, 
							       argv[0], argv);
    shScroller= new Scroller(view);
    shScroller->SetFlag(eVObjLayoutCntl);
    return new Window(this, Point(650, 440), eWinDefault,
	shScroller,
	form("%s (%d)", view->GetArgv()[0], cnt)
    );
}
 
void TypeScriptDoc::Control(int id, int detail, void *data)
{
    if (detail == cPartExtentChanged && data == (void*) shScroller)
	view->SetTtySize(shScroller->ContentRect(), fd);
    else
	Document::Control(id, detail, data);
}

void TypeScriptDoc::DoCreateMenu(class Menu *m)
{
    Document::DoCreateMenu(m);
    
    Menu *typescript= new Menu("typescript");

    typescript->AppendItems(
		      "become console",     cBECOMECONSOLE,
		      "auto reveal ",       cAUTOREVEAL,
		      "reconnect",          cRECONNECT,
		      "clear transcript ",  cCLEAR,
		      0);

    m->InsertItemAfter(cLASTEDIT, "doit", cDOIT);
    m->AppendMenu(typescript, cTYPESCRIPTMENU);
}

void TypeScriptDoc::DoSetupMenu(Menu *m)
{
    Document::DoSetupMenu(m);
    m->DisableItem(cSAVE);    
    m->DisableItem(cREVERT);    
    m->EnableItem(cTYPESCRIPTMENU);    
}

void TypeScriptDoc::DoWrite(ostream &s, int)
{
    text->PrintOnAsPureText(s);
}

void TypeScriptDoc::DoRead(istream &s, class FileType *)
{
    text->ReadFromAsPureText(s);
    if (text->IsKindOf(StyledText)) {
	StyledText *styledtext= (StyledText*) text;
	TextRunArray *st= new TextRunArray(styledtext);
	fd= gFixedFont;
	st->Insert(new_Style(fd), 0, 0, text->Size());
	TextRunArray *tmp= styledtext->SetStyles(st);
	delete tmp;
    }
    view->SetText(text);
}

bool TypeScriptDoc::Modified()
{
    return FALSE;
}

bool TypeScriptDoc::CanLoadDocument(FileType *ft)
{
    return ft->IsAscii();
}
