//$Printer$
#include "Printer.h"
#include "BlankWin.h"
#include "VObject.h"
#include "Cluster.h"
#include "Buttons.h"
#include "BorderItems.h"
#include "PrintPort.h"

extern "C" int atoi(const char*);

//---- Printer -----------------------------------------------------------------

MetaImpl0(Printer);

Printer::Printer(char *n, bool cp) : Dialog(n, eBWinBlock)
{
    name= n;
    options= 0;
    canprint= cp;
}

Printer::~Printer()
{
}

PrintPort *Printer::MakePrintPort(char*)
{
    return new PrintPort;
}

char *Printer::GetName()
{
    return name;
}

VObject *Printer::DoCreatePrintDialog()
{
    return 0;
}

VObject *Printer::GetOptions()
{
    if (options == 0)
	options= DoCreatePrintDialog();
    return options;
}

VObject *Printer::DoCreateDialog()
{
    return new BorderItem(
	new Cluster(cIdNone, (VObjAlign)(eVObjHLeft|eVObjHExpand), 20,
	    GetOptions(),
	    new Cluster(cIdNone, (VObjAlign)(eVObjVBase|eVObjHExpand), 20,
		new ActionButton(cIdYes, "Ok", TRUE),
		new ActionButton(cIdDefault, "Default"),
		new ActionButton(cIdCancel, "Cancel"),
		0
	    ),
	    0
	),
	20, 0
    );
}

void Printer::Control(int id, int p, void *v)
{
    Dialog::Control(id, p, v);
}

Printer *NewPrinter()
{
    return new Printer;
}

Point Printer::GetPageSize()
{
    Point size(570, 780);   // a4 size
    char *w, *h;
    
    if (w= Getenv("ET_PAGE_WIDTH"))
	size.x= atoi(w);
    if (h= Getenv("ET_PAGE_HEIGHT"))
	size.y= atoi(h);
    return size;
}
