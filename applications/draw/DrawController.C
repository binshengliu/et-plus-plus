//$DrawController$
#include "ET++.h"

#include "MenuBar.h"
#include "DrawController.h"
#include "Commands.h"
#include "TextShape.h"
#include "ImageShape.h"
#include "TextCmd.h"
#include "MenuItems.h"
#include "ObjList.h"
#include "DrawView.h"
#include "ObjInt.h"

static short SunLogo[]= {
#   include  "images/SunLogo.im"
};

Ink *InkPalette[256];

MetaImpl(DrawController, (TP(drawView), 0));

DrawController::DrawController(DrawView *dv)
{
    drawView= dv;
}

class MenuBar *DrawController::CreateMenuBar(EvtHandler *next)
{
    PullDownMenu *m;
    int i= 0, font, size= gSysFont->Size();
    GrFont fid= (GrFont)gSysFont->Fid();
    char *fontname;
    Point tb(4,0);  // border around textitems
    ObjList *list= new ObjList;
    float grey;
    
    if (InkPalette[0] == 0) {
	InkPalette[i++]= ePatNone;
	InkPalette[i++]= ePatWhite;
	InkPalette[i++]= ePatBlack;
	InkPalette[i++]= ePat00;
	InkPalette[i++]= ePat01;
	InkPalette[i++]= ePat02;
	InkPalette[i++]= ePat03;
	InkPalette[i++]= ePat04;
	InkPalette[i++]= ePat05;
	InkPalette[i++]= ePat06;
	InkPalette[i++]= ePat07;
	InkPalette[i++]= ePat08;
	InkPalette[i++]= ePat09;
	InkPalette[i++]= ePat10;
	InkPalette[i++]= ePat11;
	InkPalette[i++]= ePat12;
	    
	int r, g, b;
	for (r= 0; r < 4; r++)
	    for (g= 0; g < 4; g++)
		for (b= 0; b < 4; b++)
		    InkPalette[i++]= new RGBColor(r*64, g*64, b*64);

	for (grey= 0.0; grey <= 1.0; grey+= 0.1)
	    InkPalette[i++]= new RGBColor(grey);
    }
    
    //---- sun menu
    m= new PullDownMenu(new ImageItem(SunLogo, Point(23)));
    m->AppendItems("About Draw", cABOUT, 0);
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
    
    //---- ink menu
    m= new PullDownMenu("Ink", FALSE, 10, 0);
    m->AppendItem("None", cFIRSTPAT);
    for (i= 1; InkPalette[i]; i++)
	m->Append(new PatternItem(cFIRSTPAT+i, InkPalette[i]));
    list->Add(new PullDownItem(m));
    
    //---- line menu
    m= new PullDownMenu("Lines");
    for (int cap= 0; cap < 4; cap++)
	m->Append(new LineStyleItem(cFIRSTPEN+cap, 1, (GrLineCap)cap));
    m->AppendItems("-", "Hairline",  cFIRSTPENSIZE, 0);
    for (int lw= 1; lw <= 30; lw++)
	m->Append(new LineStyleItem(cFIRSTPENSIZE+lw, lw));
    list->Add(new PullDownItem(m));
    
    //---- pen ink menu
    m= new PullDownMenu("Pen Ink", FALSE, 10, 0);
    m->AppendItem("None", cFIRSTPPAT);
    for (i= 1; InkPalette[i]; i++)
	m->Append(new PatternItem(cFIRSTPPAT+i, InkPalette[i]));
    list->Add(new PullDownItem(m));

    return new MenuBar(next, list);
}

Command *DrawController::DoMenuCommand(int cmd)
{
    TextShape *activeText= drawView->GetActiveText();
    TextView *tv= 0;

    if (activeText)
	tv= activeText->GetTextView();
	
    if (drawView->Selected() <= 0)
	return 0;
	
    if (cmd >= cFIRSTPAT && cmd <= cLASTPAT)
	return new SimplePropertyCommand(drawView, eShapePattern, cmd-cFIRSTPAT, "set ink");

    if (cmd >= cFIRSTPPAT && cmd <= cLASTPPAT) {
	if (tv)
	    return new ChangeStyleCommand(tv, cmd, "set penink", eStInk, 
				StyleSpec(eFontDefault,eFacePlain,0,InkPalette[cmd-cFIRSTPPAT]));
	return new SimplePropertyCommand(drawView, eShapePenPattern, cmd-cFIRSTPPAT, "set penink");
    }

    if (cmd >= cFIRSTPEN && cmd <= cLASTPEN)
	return new SimplePropertyCommand(drawView, eShapeArrows, cmd-cFIRSTPEN, "set arrows");

    if (cmd >= cFIRSTPENSIZE && cmd <= cLASTPENSIZE)
	return new SimplePropertyCommand(drawView, eShapePensize, cmd-cFIRSTPENSIZE, "set pensize");

    if (cmd >= cFIRSTSIZE && cmd <= cLASTSIZE) {
	if (tv)
	    return new ChangeStyleCommand(tv,cmd,"set size", eStSize, 
				StyleSpec(eFontDefault,eFacePlain,cmd-cFIRSTSIZE));
	return new PropertyCommand(drawView, eShapeTextSize , new ObjInt(cmd-cFIRSTSIZE), "set size");
    }

    if (cmd >= cFIRSTFONT && cmd <= cLASTFONT) {
	if (tv)
	    return new ChangeStyleCommand(tv,cmd,"set font", eStFont, 
				StyleSpec(GrFont(cmd-cFIRSTFONT),eFacePlain,0));
	return new PropertyCommand(drawView, eShapeTextFont, new ObjInt(cmd-cFIRSTFONT), "set font");
    }

    if (cmd >= cFIRSTFACE && cmd <= cLASTFACE) {
	if (tv)
	    return new ChangeStyleCommand(tv,cmd,"set face", eStFace, 
				StyleSpec(eFontDefault,GrFace(cmd-cFIRSTFACE),0));
	return new PropertyCommand(drawView, eShapeTextFace, new ObjInt(cmd-cFIRSTFACE), "set face");
    }

    if (cmd >= cFIRSTADJUST && cmd <= cLASTADJUST)
	return new SimplePropertyCommand(drawView, eShapeAdjust, cmd-cFIRSTADJUST, "set just");

    if (cmd >= cFIRSTSPACING && cmd <= cLASTSPACING)
	return new SimplePropertyCommand(drawView, eShapeSpacing, cmd-cFIRSTSPACING, "set spacing");
    return 0;
}

Command *DrawController::DoImport(istream &s, FileType *ft)
{
    Shape *ns= 0;
	
    if (strcmp(ft->Type(), cDocTypeAscii) == 0) {
	GapText t;
	t.ReadFromAsPureText(s, ft->SizeHint());
	TextShape *ts;
	if (ts= drawView->GetActiveText()) {
	    Command *cmd= ts->GetTextView()->InsertText(&t);
	    ts->Invalidate();
	    return cmd;
	}
	
	ts= new TextShape;
	ts->Init(drawView->LastClick(), drawView->LastClick()+10);
	ts->GetTextView()->InsertText(&t);
	ns= ts;
	
    } else if (strcmp(ft->Type(), cDocSunRasterFile) == 0
			|| strcmp(ft->Type(), cDocSunRasterFileAscii) == 0) {
	char *name= ft->PathName();
	if (name == 0)
	    return gNoChanges;
	Bitmap *bm= new Bitmap(name);
	if (bm)
	    ns= new ImageShape(bm);
    }
    if (ns)
	return new SPasteCommand(drawView, ns, drawView->LastClick());
    return gNoChanges;
}

ObjList *DrawController::ReadShapes(istream &from)
{
    ObjList *newshapes= 0;    
    from >> newshapes;
    if (newshapes) 
	drawView->SetShapes(newshapes);
    return newshapes;
}

void DrawController::WriteShapes(ostream &to)
{
    drawView->SetSelection(0);
    drawView->SetActiveText(0);
    drawView->RemoveDeleted();    
    to << drawView->GetShapes();
}
