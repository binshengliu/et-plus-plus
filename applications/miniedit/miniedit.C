//$miniedit$
#include "Application.h"
#include "MiniEditDoc.h"

//---- MiniEditApp ---------------------------------------------------------

class miniedit: public Application { 
public:
    miniedit(int argc, char **argv) : Application(argc, argv, cDocTypeAscii)
	{ ApplInit(); }
    Document *DoMakeDocuments(const char *)
	{ return new MiniEditDoc; }
};

//---- main --------------------------------------------------------------------

main(int argc, char *argv[])
{
    miniedit edit(argc, argv);    

    return edit.Run();
}
