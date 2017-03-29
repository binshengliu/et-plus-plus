//$VobEditDoc$
#include "ET++.h"

#include "VObjectText.h"
#include "myVObjectTextView.h"
#include "StyledText.h"
#include "ObjList.h"
#include "Dialog.h"
#include "VobEditDoc.h"
#include "AnnotatedItem.h"
#include "MenuBar.h"
#include "TextCmd.h"

char *cVobDocType= "VOBTEXT";

const int   cCOLOUR         = 1010,
	    cTEXTSIZE       = 1017,
	    cTEXTFONT       = 1018,
	    cTEXTFACE       = 1019,
	    cTEXTSPACING    = 1020,
	    cFIRSTADJUST    = 1030,
	    cLASTADJUST     = 1039,
	    cFIRSTFONT      = 1100,
	    cLASTFONT       = 1199,
	    cFIRSTSIZE      = 1550,
	    cLASTSIZE       = 1599,
	    cFIRSTFACE      = 1600,
	    cLASTFACE       = 1649,
	    cFIRSTSPACING   = 1650,
	    cLASTSPACING    = 1699,
	    cFIRSTSPLINE    = 1700,
	    cLASTSPLINE     = 1799,            
	    cINCSIZE        = 1901,
	    cDECSIZE        = 1902;

//---- VobEditDoc -------------------------------------------------------------

MetaImpl(VobEditDoc, (TP(view), TP(text), 0));

VobEditDoc::VobEditDoc() : Document(cVobDocType)
{
    text= new VObjectText;
}

VobEditDoc::~VobEditDoc()
{
    SafeDelete(view);
    SafeDelete(text);
}

static short SunImage[]= {
#   include  "images/sun.im"
};

MenuBar *VobEditDoc::CreateMenuBar()
{
    PullDownMenu *m;
    int font, size= gSysFont->Size();
    GrFont fid= (GrFont)gSysFont->Fid();
    char *fontname;
    Point tb(4,0);  // border around textitems
    
    ObjList *list= new ObjList;
    
    //---- sun menu
    m= new PullDownMenu(new ImageItem(SunImage, Point(23)));
    m->AppendItems("About", cABOUT, 0);
    list->Add(new PullDownItem(m));
    
    //---- font menu
    m= new PullDownMenu("Fonts", TRUE);
    for (font= 0; fontname= gFontManager->IdToName((GrFont)font); font++)
	m->Append(new TextItem(cFIRSTFONT+font, fontname, gSysFont, tb)); 
    list->Add(new PullDownItem(m));
	
    //---- styles menu
    m= new PullDownMenu("Styles");
    m->Append(new TextItem(cFIRSTFACE+eFacePlain, "Plain", 
		    new_Font(fid, size, eFacePlain), tb));
    m->Append(new TextItem(cFIRSTFACE+eFaceBold, "Bold", 
		    new_Font(fid, size, eFaceBold), tb));
    m->Append(new TextItem(cFIRSTFACE+eFaceItalic, "Italic", 
		    new_Font(fid, size, eFaceItalic), tb));
    m->Append(new TextItem(cFIRSTFACE+eFaceUnderline, "Underline", 
		    new_Font(fid, size, eFaceUnderline), tb));
    m->Append(new TextItem(cFIRSTFACE+eFaceOutline, "Outline", 
		    new_Font(fid, size, eFaceOutline), tb));
    m->Append(new TextItem(cFIRSTFACE+eFaceShadow, "Shadow", 
		    new_Font(fid, size, eFaceShadow), tb));
    list->Add(new PullDownItem(m));

    //---- sizes menu
    m= new PullDownMenu("Sizes");
    for (int sz= 9; sz <= 24; sz++)
	m->Append(new TextItem(cFIRSTSIZE+sz, form("%d  ", sz), gSysFont, tb)); 
    m->Append(new TextItem(cINCSIZE, "Size + 2", gSysFont, tb));
    m->Append(new TextItem(cDECSIZE, "Size - 2", gSysFont, tb));
    list->Add(new PullDownItem(m));

    //---- format menu
    m= new PullDownMenu("Format");
    m->AppendItems("Align Left",     cFIRSTADJUST,
		   "Align Right",    cFIRSTADJUST + 1,
		   "Align Center",   cFIRSTADJUST + 2,
		   "Justify",        cFIRSTADJUST + 3,
		   "-",
		   "Single Spacing", cFIRSTSPACING + eOne,
		   "1-1/2 Spacing",  cFIRSTSPACING + eOneHalf,
		   "Double Spacing", cFIRSTSPACING + eTwo,
		   0);
    list->Add(new PullDownItem(m));
    
    return new MenuBar(this, list);
}

Window *VobEditDoc::DoMakeWindows()
{   
    view= new myVObjectTextView(this, Rectangle(500, cFit), text);

    VObject *vop= new BorderItem(new Clipper(CreateMenuBar()), gPoint0);
    vop->SetFlag(eVObjVFixed);

    return new Window(this, Point(540, 500), eWinDefault,
	new Expander(cIdNone, eVert, gPoint2, 
	    vop,
	    new Splitter(view),
	    0
	)
    );
}
 
bool VobEditDoc::CanLoadDocument(FileType *ft)
{
    return strismember(ft->Type(), cVobDocType, cDocTypeAscii, 0);
}

void VobEditDoc::DoWrite(ostream &to, int option)
{
    Document::DoWrite(to, option);
    to << text;
}
 
void VobEditDoc::DoRead(istream &from, FileType *ft)
{
    VObjectText *t = new VObjectText(1024);
    Text *oldtext;
    
    if (strcmp(ft->Type(), cDocTypeAscii) != 0) {
	Document::DoRead(from, ft);
	from >> t;
    } else {
	t->ReadFromAsPureText(from);
	TextRunArray *st= new TextRunArray(t);
	st->Insert(new_Style(gSysFont), 0, 0, t->Size());
	TextRunArray *tmp= t->SetStyles(st);
	delete tmp;
    }
    oldtext= view->SetText(text= t);
    SafeDelete(oldtext);
}

Command *VobEditDoc::DoMenuCommand(int cmd) 
{
    if (cmd == cINCSIZE) 
	return new ChangeStyleCommand (view, cINCSIZE, "Change Size", 
					eStAddSize, StyleSpec(eFontDefault,eFacePlain,2));
    if (cmd == cDECSIZE) 
	return new ChangeStyleCommand (view, cDECSIZE, "Change Size", 
					eStAddSize, StyleSpec(eFontDefault,eFacePlain,-2));
    if (cmd >= cFIRSTSIZE && cmd <= cLASTSIZE) 
	return new ChangeStyleCommand (view, cmd, "change size", eStSize, 
					StyleSpec(eFontDefault,eFacePlain,(GrFace)(cmd-cFIRSTSIZE)));
    if (cmd >= cFIRSTFONT && cmd <= cLASTFONT) 
	return new ChangeStyleCommand (view, cmd, "change font", eStFont, 
					 StyleSpec((GrFont)(cmd-cFIRSTFONT),eFacePlain,0));
    if (cmd >= cFIRSTFACE && cmd <= cLASTFACE) 
	return new ChangeStyleCommand (view, cmd, "change face", 
					 eStFace, StyleSpec(eFontDefault,(GrFace)(cmd-cFIRSTFACE),0));
    if (cmd >= cFIRSTADJUST && cmd <= cLASTADJUST) {
	view->SetJust((eTextJust)(cmd-cFIRSTADJUST));
	return gNoChanges;
    }
    if (cmd >= cFIRSTSPACING && cmd <= cLASTSPACING) {
	view->SetSpacing((eSpacing)(cmd-cFIRSTSPACING));
	return gNoChanges;
    }
    return Document::DoMenuCommand(cmd);
}
