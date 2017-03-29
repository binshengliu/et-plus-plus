//$vobedit$
#include "Application.h"
#include "Alert_e.h"
#include "VobEditDoc.h"
#include "String.h"
#include "About.h"

//---- vobedit -----------------------------------------------------------------

class vobedit: public Application { 
public:
    MetaDef(vobedit);
    vobedit(int argc, char **argv);
    Document *DoMakeDocuments(const char *);
    bool CanOpenDocument(class FileType *);
    void About();
};

MetaImpl0(vobedit);

vobedit::vobedit(int argc, char **argv) : Application(argc, argv, cVobDocType)
{
    ApplInit();
}

Document *vobedit::DoMakeDocuments(const char *)
{ 
    return new VobEditDoc; 
}

void vobedit::About()
{ 
    ShowAlert(eAlertNote, aboutMsg); 
}

bool vobedit::CanOpenDocument(class FileType *ft)
{
    return strismember(ft->Type(), cVobDocType, cDocTypeAscii, 0);    
}

main(int argc, char *argv[])
{
    vobedit myAppl(argc, argv);    

    return myAppl.Run();
}
