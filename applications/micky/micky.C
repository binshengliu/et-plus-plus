//$myView,myDocument,micky$

#include "ET++.h"

//---- myView ------------------------------------------------------------------

class myView: public View { 
public:
    MetaDef(myView);
    myView(Document *dp, Rectangle itsExtent) :View (dp, itsExtent)
	{ }
    void Draw(Rectangle);
};

MetaImpl0(myView);
	    
void myView::Draw(Rectangle)
{
    GrFillOval(Rectangle(72, 74, 55, 65));
    GrEraseOval(Rectangle(74, 84, 51, 54));
    GrStrokeOval(Rectangle(84, 109, 31, 20));
    GrEraseOval(Rectangle(84, 108, 31, 14));
    GrStrokeOval(Rectangle(87, 98, 9, 9));
    GrFillOval(Rectangle(90, 101, 3, 3));
    GrStrokeOval(Rectangle(104, 98, 9, 9));
    GrFillOval(Rectangle(107, 101, 3, 3));
    GrFillOval(Rectangle(97, 111, 6, 6));
    GrFillOval(Rectangle(52, 53, 38, 38));      // left ear
    GrFillOval(Rectangle(110, 53, 38, 38));     // right ear
}

//---- myDocument ---------------------------------------------------------------

class myDocument : public Document {
    View *view;
public:
    MetaDef(myDocument);
    myDocument()
	{ }
    ~myDocument()
	{ SafeDelete(view); }
    Window *DoMakeWindows();
};

MetaImpl(myDocument, (TP(view), 0));

Window *myDocument::DoMakeWindows()
{
    return new Window(this, Point(250), eWinDefault,
	new Splitter(new myView(this, Point(400, 256*10))));
}

//---- micky -------------------------------------------------------------------

class micky: public Application {
public:
    MetaDef(micky);
    micky(int argc, char **argv);
    Document *DoMakeDocuments(const char *)
	{ return new myDocument; }
};

MetaImpl0(micky);

micky::micky(int argc, char **argv) : Application(argc, argv)
{
    ApplInit();
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    micky mymicky(argc, argv);
	
    return mymicky.Run();
}
