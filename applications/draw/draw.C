//$draw$
#include "Application.h"
#include "ObjList.h"
#include "ObjArray.h"
#include "DrawDocument.h"

#include "BoxShape.h"
#include "RcBoxShape.h"
#include "OvalShape.h"
#include "LineShape.h"
#include "ArcShape.h"
#include "PolyShape.h"
#include "RegionShape.h"
#include "TextShape.h"
#include "BezierShape.h"

static short SelectionImage[]= {
#   include  "images/Selection.im"
};

Point MinShapeSize(5,5);

//---- draw ------------------------------------------------------------

class draw: public Application { 
    class ObjArray *prototypes;
    class ObjList *tools;
public:
    MetaDef(draw);
    draw(int argc, char **argv);
    ~draw();
    
    class Document *DoMakeDocuments(const char *);
    int DynLoadHook(Object*);
};

MetaImpl(draw, (TP(prototypes), TP(tools), 0));

draw::draw(int argc, char **argv) : Application(argc, argv, cDocTypeDraw)
{
    ApplInit();
    
    //---- create prototypes
    prototypes= new ObjArray;
    
    prototypes->Add(new Object);    // must be first element
    prototypes->Add(new TextShape); // must be second element
    
    prototypes->Add(new BoxShape);
    prototypes->Add(new RcBoxShape);
    prototypes->Add(new OvalShape);
    prototypes->Add(new LineShape);
    prototypes->Add(new ArcShape);
    prototypes->Add(new PolyShape);
    prototypes->Add(new RegionShape);
    prototypes->Add(new BezierShape);

    //---- create tool palette
    Point p(40, 28);
    short *image;
    Shape *sp;
    Iter next(prototypes);
    
    tools= new ObjList;
    tools->Add(new ImageItem(SelectionImage, p));
    
    while (sp= (Shape*) next())
	if (sp->IsKindOf(Shape))
	    if (image= sp->GetImage())
		tools->Add(new ImageItem(image, p));
}

draw::~draw()
{
    if (prototypes)
	prototypes->FreeAll();
    SafeDelete(prototypes);
    if (tools)
	tools->FreeAll();
    SafeDelete(tools);
}

Document *draw::DoMakeDocuments(const char *)
{
    return new DrawDocument(prototypes, tools);
}

int draw::DynLoadHook(Object *op)
{
    short *image;
    
    if (op->IsKindOf(Shape) && (image= ((Shape*)op)->GetImage())) {
	prototypes->Add(op);
	tools->Add(new ImageItem(image, Point(40, 28)));
    }
    return 1;
}

//---- main ---------------------------------------------------------------------

main(int argc, char **argv)
{
    draw adraw(argc, argv);
	
    return adraw.Run();
}
