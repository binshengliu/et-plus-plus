//$DrawDocument$
#include "ET++.h"

#include "MenuBar.h"
#include "ObjArray.h"
#include "CollView.h"
#include "FileDialog.h"

#include "DrawDocument.h"
#include "DrawController.h"
#include "DrawView.h"
#include "Commands.h"
#include "TextShape.h"
#include "ImageShape.h"
#include "TextCmd.h"
#include "ObjInt.h"
#include "WindowSystem.h"

char *cDocTypeDraw= "DRAW";

const int cIdPalette    =   cIdFirstUser + 11,
	  cIdDrawView   =   cIdFirstUser + 12;

//---- DrawDocument ------------------------------------------------------------

MetaImpl(DrawDocument, (TP(prototypes), TP(tools), TP(info),
			    TP(drawView), TP(drawController), TP(paletteView), TP(shapes), 0));

DrawDocument::DrawDocument(ObjArray *pro, ObjList *too) : Document(cDocTypeDraw)
{
    shapes= new ObjList;
    prototypes= pro;
    tools= too;
}

DrawDocument::~DrawDocument()
{
    SafeDelete(drawView);
    SafeDelete(drawController);
    paletteView->SetCollection(0, FALSE);
    SafeDelete(paletteView);
}

Window *DrawDocument::DoMakeWindows()
{
    drawView= new DrawView(this, Point(2000), shapes);
    drawView->SetId(cIdDrawView);
    
    drawController= new DrawController(drawView);
    
    paletteView= new CollectionView(this, tools, eCVGrid);
    paletteView->SetGap(gPoint1);
    paletteView->SetId(cIdPalette);
    paletteView->SetSelection(Rectangle(1,1));

    VObject *vop= new BorderItem(new Clipper(drawController->CreateMenuBar(drawView)), gPoint0);
    vop->SetFlag(eVObjVFixed);
    
    //info= new TextItem("");
    //info->SetFlag(eVObjVFixed);
 
    return new Window(this, Point(600, 400), eWinDefault,
	new Expander(cIdNone, eVert, gPoint3,
	    vop,
	    new Expander(cIdNone, eHor, gPoint3,
		new Expander(cIdNone, eVert, gPoint3,
		    new BorderItem(new Clipper(paletteView), gPoint0),
		    new Filler(gPoint0),
		    0
		),
		new Splitter(drawView),
		0
	    ),
	    info,
	0)
    );
}

Command *DrawDocument::DoMenuCommand(int cmd)
{
    Command *command= drawController->DoMenuCommand(cmd);
    if (command)
	return command;
    return Document::DoMenuCommand(cmd);
}

void DrawDocument::Control(int id, int part, void *val)
{
    int ix= int(val);
    
    if (id == cIdPalette
		&& (part == cPartCollSelect || part == cPartCollDoubleSelect))
	drawView->SetTool(prototypes->At(ix));
    else if (id == cIdDrawView && part == 0 && (ix == 0 || ix == 1)) {
	paletteView->SetSelection(Rectangle(0, ix, 1, 1));
	drawView->SetTool(prototypes->At(ix));
    } if (info && id == cIdDrawView && part == 123) {
	info->SetString((char*) val, TRUE);
	info->UpdateEvent();
    } else
	Document::Control(id, part, val);
}

bool DrawDocument::CanLoadDocument(FileType *ft)
{
    return strismember(ft->Type(), cDocTypeDraw, cDocTypeAscii, 0);
}
    
bool DrawDocument::CanImportDocument(FileType *ft)
{
    if (!ft)
	return TRUE;
    return strismember(ft->Type(), cDocTypeAscii,
				   cDocSunRasterFile,
				   cDocSunRasterFileAscii,
				   0);
}

void DrawDocument::DoRead(istream &from, FileType *ft)
{    
    if (strcmp(ft->Type(), GetDocumentType()) == 0) {
	Document::DoRead(from, ft);
	shapes= drawController->ReadShapes(from);
    }
}

void DrawDocument::DoWrite(ostream &to, int type)
{
    if (type == 0) {
	Document::DoWrite(to, type);
	drawController->WriteShapes(to);
    }
}

Command *DrawDocument::DoImport(istream &s, FileType *ft)
{
    return drawController->DoImport(s, ft);
}
