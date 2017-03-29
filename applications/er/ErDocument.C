//$ErDocument$

#include "ET++.h"
#include "ErShapes.h"
#include "ErDocument.h"
#include "ErView.h"
#include "ErCommands.h"
#include "OrdColl.h"

const int cInsertRelation  =   cUSERCMD + 1,
	  cInsertEntity    =   cUSERCMD + 2;

char *cDocTypeER = "ER";

//---- ErDocument --------------------------------------------------------------

MetaImpl(ErDocument, (TP(view), TP(list), 0));

ErDocument::ErDocument() : Document(cDocTypeER)
{
    SetShapes(new OrdCollection, FALSE);
}

ErDocument::~ErDocument()
{
    SafeDelete(view);
    SetShapes(0, FALSE);
}

//---- shapes ------------------------------------------------------------------

void ErDocument::SetShapes(SeqCollection *shapes, bool redraw) 
{
    if (redraw)
	view->ForceRedraw();
    if (list) {
	list->FreeAll();
	SafeDelete(list);
    } 
    list= shapes;
    if (list) {
	list->ForEach(ErShape,Init)(view);
    }
}

void ErDocument::AddShape(ErShape *c)
{
    list->Add(c);
    c->Init(view);
}

void ErDocument::RemoveShape(ErShape *c)
{
    c->ForceRedraw();
    list->RemovePtr(c);
}

Window *ErDocument::DoMakeWindows()
{
    // create the ErView I show
    view= new ErView(this, Point(2000));
    // create the window layout
    return new Window(this, Point(500), eWinDefault, new Scroller(view));
}

//---- menus -------------------------------------------------------------------

void ErDocument::DoCreateMenu(Menu *menu)
{
    Document::DoCreateMenu(menu);
    menu->InsertItemsAfter(cLASTEDIT, "-",
				      "new Relation",   cInsertRelation,
				      "new Entity",     cInsertEntity,
				      0);
}

void ErDocument::DoSetupMenu(Menu *menu)
{
    Document::DoSetupMenu(menu);
    menu->EnableItems(cInsertRelation, cInsertEntity, 0);
}

Command *ErDocument::DoMenuCommand(int cmd)
{
    switch (cmd) {
    case cInsertRelation:
	return new InsertCommand(this, new Relation);
    case cInsertEntity:
	return new InsertCommand(this, new Entity);
    default:
	return Document::DoMenuCommand(cmd);
    }
}

//---- input/output ------------------------------------------------------------

void ErDocument::DoWrite(ostream &s, int o)
{
    Document::DoWrite(s, o);
    s << list SP;
}

void ErDocument::DoRead(istream &s, FileType *ft)
{
    SeqCollection *newlist;
    
    Document::DoRead(s, ft);
    s >> newlist;
    SetShapes(newlist, TRUE);
}
