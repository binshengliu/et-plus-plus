//$BrowserApplication$

#include "ET++.h"

#include "BrowserDoc.h"

//---- BrowserApplication --------------------------------------------------------------

class BrowserApplication: public Application { 
    int nFileLists;
public:
    MetaDef(BrowserApplication);
    BrowserApplication(int argc, char **argv) 
				  : Application(argc, argv, cDocTypeAscii)
	// I can handle documents with type cDocTypeAscii
	{ ApplInit(); nFileLists= 3; }
    Document *DoMakeDocuments(const char*)
	{ return new BrowserDocument(nFileLists); }
    int DoParseOptions(char *arg0, char *arg1);
};

MetaImpl0(BrowserApplication);

int BrowserApplication::DoParseOptions(char *arg0, char *arg1)
{
    if (arg0[1] == 'n') {
	if (sscanf(&arg0[2], "%d", &nFileLists) == 1)
	    return 1;
	if (arg1 && sscanf(arg1, "%d", &nFileLists) == 1)
	    return 2;
	return 1;
    }
    return Application::DoParseOptions(arg0, arg1);  
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    BrowserApplication browser(argc, argv);
    
    return browser.Run();
}
