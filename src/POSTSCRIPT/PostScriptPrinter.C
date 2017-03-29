//$PostScriptPrinter$

#include "Port.h"
#include "PostScriptPrinter.h"
#include "OneOfCluster.h"
#include "ManyOfCluster.h"
#include "EditTextItem.h"
#include "Expander.h"
#include "BorderItems.h"
#include "Buttons.h"

#include "System.h"

//---- PostScriptPrinter -------------------------------------------------------

MetaImpl(PostScriptPrinter, (TP(orientationCluster), TP(resolutionCluster),
    TP(optionsCluster), TP(eti),
    T(curr.resolution), TB(curr.portrait), TB(curr.smooth), TB(curr.wysiwyg), 
    TB(curr.prolog), 
    T(old.resolution), TB(old.portrait), TB(old.smooth),
    TB(old.wysiwyg), TB(old.prolog), 0));

const int cIdName       =   cIdFirstUser + 555,
	  cIdOptions    =   cIdFirstUser + 102,
	  cIdResolution =   cIdFirstUser + 103,
	  cIdOrientation=   cIdFirstUser + 104;

static bool hack;

PostScriptPrinter::PostScriptPrinter() : Printer("PostScript", TRUE)
{
    hack= TRUE;
    DoSetDefaults();
    hack= FALSE;
}

class PrintPort *PostScriptPrinter::MakePrintPort(char *name)
{
    return new PostScriptPort(name, &curr);
}

inline void SetMany(ManyOfCluster *c, int x, int y, bool b)
{
    ((ToggleButton*)((Cluster*)c->GetItem(x,y))->GetItem(0,0))->SetState(b);
}

void PostScriptPrinter::DoSetDefaults()
{
    char *lwname;
    
    old.portrait= old.prolog= TRUE;
    old.smooth= old.wysiwyg= FALSE;
    old.resolution= 4;
    lwname= Getenv("PRINTER");
    if (lwname == 0)
	lwname= "lp";
    strcpy(old.prnname, lwname);

    if (!hack)
	DoRestore();
}

void PostScriptPrinter::DoSave()
{
    old= curr;
}

void PostScriptPrinter::DoRestore() 
{
    curr= old;
    /*
    // Update dialog-box
    SetMany(optionsCluster, 0, 0, old.smooth);
    SetMany(optionsCluster, 0, 1, old.wysiwyg);
    SetMany(optionsCluster, 0, 2, old.prolog);
    resolutionCluster->Set(cIdResolution+old.resolution-1);
    orientationCluster->Set(cIdOrientation + (old.portrait ? 0 : 1));
    eti->SetString((byte*)old.prnname);
    */
}

void PostScriptPrinter::DoSetup()
{
    optionsCluster->GetItem(0,1)->Disable();
}

VObject *PostScriptPrinter::DoCreatePrintDialog()
{
    //---- dialog parts ----
    
    orientationCluster= new OneOfCluster(cIdOrientation, eVObjHLeft, 5,
	"Portrait",
	"Landscape",
	0);
	
    optionsCluster= new ManyOfCluster(cIdOptions, eVObjHLeft, 5,
	"smooth",
	"wysiwyg",
	"PS-prolog",
	0);
	
    resolutionCluster= new OneOfCluster(cIdResolution, eVObjHLeft, 5,
	"1/300 inch",
	"2/300 inch",
	"3/300 inch",
	"4/300 inch",
	0);
	
    //---- overall layout ----
    return new Expander(cIdNone, eVert, 20,
		new Expander(cIdNone, eHor, 20,
		    new BorderItem("Options", optionsCluster),
		    new BorderItem("Resolution", resolutionCluster),
		    new BorderItem("Orientation", orientationCluster),
		    0
		),
		new BorderItem("Printer name", 
		    eti= new EditTextItem(cIdName, "", 100)
		),
		0
	    );
}

void PostScriptPrinter::Control(int id, int p, void *v)
{
    switch (id) {
    
    case cIdOptions:
	switch (p) {
	case cIdOptions:
	    curr.smooth= bool(v);
	    break;
	case cIdOptions+1:
	    curr.wysiwyg= bool(v);
	    break;
	case cIdOptions+2:
	    curr.prolog= bool(v);
	    break;
	}
	break;
	
    case cIdResolution:
	curr.resolution= p-cIdResolution+1;
	break;
	
    case cIdOrientation:
	curr.portrait= (p == cIdOrientation);
	break;
	
    case cIdName:
	Printer::Control(id, p, v);
	if (/* p == cPartChangedText && */ v == eti) {
	    strncpy(curr.prnname, eti->GetText()->AsString(), min(SIZE_PRNNAME, eti->GetTextSize()));
	    curr.prnname[eti->GetTextSize()]= 0;
	    EnableItem(cIdYes, eti->GetTextSize() > 0);
	}
	break;

    default:
	Printer::Control(id, p, v);
	break; 
    }
}

Point PostScriptPrinter::GetPageSize()
{
    Point ps= Printer::GetPageSize() * 4 / curr.resolution;
    if (curr.portrait)
	return ps;
    return Point(ps.y, ps.x);
}

Printer *NewPostScriptPrinter()
{
    return new PostScriptPrinter;
}
