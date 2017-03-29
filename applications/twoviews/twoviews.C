//$TwoViewApp$
#include "Application.h"
#include "TwoViewDoc.h"
#include "Alert_e.h"

//---- TwoViewApp ---------------------------------------------------------

static char *Msg= "An Example of two views rendering the same text";

class TwoViewApp: public Application { 
public:
    TwoViewApp(int argc, char **argv) : Application(argc, argv, cDocTypeAscii)
	{ ApplInit(); }    
    Document *DoMakeDocuments(const char *)
	{ return new TwoViewDoc; }
    void About()
	{ ShowAlert(eAlertNote, Msg); }
};

main(int argc, char *argv[])
{
    TwoViewApp app(argc, argv);    

    return app.Run();
}
