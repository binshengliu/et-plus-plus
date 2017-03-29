//$myVObjectTextView$

#include "ET++.h"
#include "ShellTView.h"
#include "VObjectText.h"
#include "DialogItems.h"
#include "PopupItem.h"
#include "ShadowItem.h"

#include "myVObjectTextView.h"
#include "AnnotatedItem.h"
#include "MenuTextView.h"
#include "MickyView.h"

const int cVOBJECTS         = cUSERCMD + 0,
	  cBUTTON           = cUSERCMD + 1,
	  cRADIOBUTTON      = cUSERCMD + 2,
	  cANNOTATEDITEM    = cUSERCMD + 3,
	  cTEXTVIEW         = cUSERCMD + 4,
	  cEDITTEXTITEM     = cUSERCMD + 5,
	  cMICKY            = cUSERCMD + 6,
	  cPOPUPITEM        = cUSERCMD + 7,
	  cGOVIEW           = cUSERCMD + 8,
	  cSHELL            = cUSERCMD + 9;


//----- myVObjectTextView ---------------------------------------------------

MetaImpl0(myVObjectTextView);
 
static char *Msg= 
"Rewrite the section on @Idynamic linking@I. Highlight the use of this feature for object input/output and the clipboard support.";

myVObjectTextView::myVObjectTextView(Document *d, Rectangle r, VObjectText *t)
		   : VObjectTextView (d, r, t, eLeft,eOne,TRUE, 
			(TextViewFlags)(eTextViewDefault | eTextFormPreempt))
{
    notemenu= new Menu("Note", FALSE, 0, 1, FALSE);
    StaticTextView *tv= new MenuTextView(notemenu, Rectangle(Point(250,cFit)), 
					    new StyledText(gSysFont,Msg,0,0,0));
    notemenu->Append(new TextItem(12345, "Note"));
    notemenu->Append(tv);
    notemenu->Append(new MenuLineItem);
    notemenu->Append(new ActionButton(12, "Edit"));
}

void myVObjectTextView::DoCreateMenu(Menu *mp)
{
    VObjectTextView::DoCreateMenu(mp);
    Menu *vobjects= new Menu("vobjects");
    
    vobjects->AppendItems(
		    "Button",           cBUTTON,
		    "RadioButton",      cRADIOBUTTON,
		    "AnnotatedItem",    cANNOTATEDITEM,
		    "TextView",         cTEXTVIEW,
		    "EditTextItem",     cEDITTEXTITEM,
		    "Micky",            cMICKY,
		    "PopupItem",        cPOPUPITEM,
		    "VObject View",     cGOVIEW,
		    "Shell",            cSHELL,
		    0);
    
    mp->AppendMenu(vobjects, cVOBJECTS);
}

void myVObjectTextView::DoSetupMenu(Menu *mp)
{
    VObjectTextView::DoSetupMenu(mp);
    if (AnySelection())
	    mp->EnableItems(cVOBJECTS, cBUTTON, cRADIOBUTTON, 
			    cANNOTATEDITEM, cTEXTVIEW, cEDITTEXTITEM, 
			    cMICKY, cGOVIEW, cSHELL, 0);
}

static char *msg1 =
"\tThis is another instance of a TextView \
inserted into a VObjecttext";

static char *msg2 =
"An EditTextItem showing several lines of text \n\
this is the second line\n\
and this is the last one";

Command *myVObjectTextView::InsertVObject(VObject *gop, bool withWrapper)
{
    VObjectText *text= (VObjectText*) GetText();
    if (withWrapper) 
	return text->InsertVObject(new StretchWrapper(gop,Point(6,4)));
    return text->InsertVObject(gop);
}
 
Command *myVObjectTextView::DoMenuCommand(int cmd)
{  
    switch (cmd) {
    case cBUTTON:
	return InsertVObject(new ActionButton(cIdNone, "an ActionButton"), FALSE);

    case cRADIOBUTTON:
	return InsertVObject(new RadioButton(cIdNone));

    case cANNOTATEDITEM:
	return InsertVObject(new AnnotatedItem(""), FALSE);
	
    case cTEXTVIEW:
	return InsertVObject(new TextView(this, Rectangle(200, cFit),
		    new GapText((byte*)msg1, -1, FALSE), eJustified));

    case cEDITTEXTITEM: 
	return InsertVObject(
	    new BorderItem(
		new EditTextItem(cIdNone, msg2, 60), gPoint0)
	);
	
    case cMICKY:
	return InsertVObject(
	    new Splitter(new MickyView(Point(300)), Point(200))
	);
	
    case cGOVIEW:
	return InsertVObject(
	    new WindowBorderItem(TRUE, 
		new TextItem("vobedit"),
		new Splitter(
		    new myVObjectTextView((Document*)GetNextHandler(),
			Point(500,cFit), new VObjectText),
		    Point(200)
		)
	    )
	);
	
    case cSHELL:
	return InsertVObject(
	    new WindowBorderItem(TRUE, 
		new TextItem("Shell"),
		new Splitter(
		    new ShellTextView((Document*)GetNextHandler(),
			Point(500,cFit), new StyledText(256, gFixedFont)),
		    Point(200)
		)
	    )
	);
		    
    case cPOPUPITEM:
	return InsertVObject(new PopupItem(cIdNone, 12345, notemenu), FALSE);
	
    default:
	return TextView::DoMenuCommand(cmd);
    }
}
