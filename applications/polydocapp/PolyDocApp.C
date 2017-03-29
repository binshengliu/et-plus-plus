//$PolyDocApp$

#include "ET++.h"
#include "DialogItems.h"

#include "EditDoc.h"
#include "TwoShapesDoc.h"

const int cNEWSHAPES= 1000,
	  cNEWTEXT=   1001;

//---- PolyDocApp ----------------------------------------------------------

class PolyDocApp: public Application { 
public:
    PolyDocApp(int argc, char **argv) : Application(argc, argv) 
	{ ApplInit(); } // there is no main document type
    Document *DoMakeDocuments(const char *);
    bool CanOpenDocument(class FileType *);
    void Control(int id, int, void*);
    class VObject *DoCreateDialog();
};

void PolyDocApp::Control(int cmd, int p, void *v)
{
    if (cmd == cNEWSHAPES) 
	NewDocument(cDocTypeShapes);
    else if (cmd == cNEWTEXT)
	NewDocument(cDocTypeAscii);
    else
	Application::Control(cmd, p, v);
}

Document *PolyDocApp::DoMakeDocuments(const char *type)
{ 
    if (strcmp(type, cDocTypeShapes) == 0)
	return new ShapeDocument;
    if (strcmp(type, cDocTypeAscii) == 0)
	return new EditDoc;
}

bool PolyDocApp::CanOpenDocument(FileType *ft)
{
    return strismember(ft->Type(), cDocTypeAscii, cDocTypeShapes, 0);
}

VObject *PolyDocApp::DoCreateDialog() 
{
    return
	new Cluster(1, eVObjVBase,
	    5,                      
	    new ActionButton(cNEWTEXT,      "new text"),
	    new ActionButton(cNEWSHAPES,    "new shapes"),
	    new ActionButton(cOPEN,         "open"),
	    new ActionButton(cQUIT,         "quit"),
	    0
	);
}

//---------------------------------------------------------------------------

main(int argc, char *argv[])
{
    PolyDocApp myAppl(argc, argv);    

    return myAppl.Run();
}
