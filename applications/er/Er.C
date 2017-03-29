//$ErApplication$

#include "ET++.h"
#include "ErDocument.h"
#include "ErView.h"

//---- ErApplication -----------------------------------------------------------

class ErApplication: public Application {
public:
    MetaDef(ErApplication);
    
    ErApplication(int argc, char *argv[]) : Application(argc, argv, cDocTypeER)
	// the types of documents I can handle is cDocTypeER
	{ ApplInit(); }
    Document *DoMakeDocuments(const char*)
	{ return new ErDocument(); }
};

MetaImpl0(ErApplication);
    
//---- main --------------------------------------------------------------------

main(int argc, char *argv[])
{
    ErApplication er(argc, argv);
    
    return er.Run();
}          
