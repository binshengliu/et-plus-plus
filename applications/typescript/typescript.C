//$TypescriptApp$
#include "Application.h"
#include "TypeScriptDoc.h"

//---- TypescriptApp --------------------------------------------------------------

class TypescriptApp: public Application {
    int shellcnt;
public:
    MetaDef(TypescriptApp);
    TypescriptApp(int argc, char **argv) : Application(argc, argv, cDocTypeAscii)
	{ ApplInit(); shellcnt= 0; }
    Document *DoMakeDocuments(const char *)
	{ return new TypeScriptDoc(shellcnt++); }
    bool CloseAllDocuments()
	{ return TRUE; }
};

MetaImpl(TypescriptApp, (T(shellcnt), 0));

main(int argc, char *argv[])
{
    TypescriptApp typescript(argc, argv);    

    return typescript.Run();
}
