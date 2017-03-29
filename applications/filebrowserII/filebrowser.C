//$BrowserApplication$

#include "Application.h"
#include "String.h"
#include "BrowserDoc.h"
#include "BrowserCmdNo.h"
#include "Buttons.h"
#include "Cluster.h"
#include "BorderItems.h"
#include "Preferences_e.h"

//---- BrowserApplication --------------------------------------------------------------vbcvbxcb

class BrowserApplication: public Application { 
public:
    MetaDef(BrowserApplication);
    
    BrowserApplication(int argc, char **argv);
    ~BrowserApplication();
    Document *DoMakeDocuments(const char*);
    void Control(int id, int, void*);
    VObject *DoCreateDialog();
};

MetaImpl0(BrowserApplication);

BrowserApplication::BrowserApplication(int argc, char **argv) 
					: Application(argc, argv, cDocTypeAscii)
{
    ApplInit();
    Preferences::Load();
}

BrowserApplication::~BrowserApplication()
{
    Preferences::Save();
}

Document *BrowserApplication::DoMakeDocuments(const char*)
{ 
    return new BrowserDocument(); 
}

VObject *BrowserApplication::DoCreateDialog()
{
    return new BorderItem(
			    new Cluster (cIdNone, eVObjVBase, gPoint4,
				new ActionButton (cNEW,  "new"),
				new ActionButton (cOPEN, "open"),
				new ActionButton (cQUIT, "quit"),
				new ActionButton (cETBROWSER, "ET++"),
				0
			    ),
			    gPoint4
		    ); 
}

void BrowserApplication::Control(int cmd, int p, void *v)
{
    if (cmd == cETBROWSER) 
	EditSource(TRUE);
    else
	Application::Control(cmd, p, v);
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    BrowserApplication browser(argc, argv);
    return browser.Run();
}
