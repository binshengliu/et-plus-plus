//$myView,myDocument,color,ColorCell2$

#include "ET++.h"
#include "ColorPicker.h"
#include "BorderItems.h"

//---- ColorCell2 ---------------------------------------------------------------

class ColorCell2: public VObject {
    RGBColor rc;
public:
    ColorCell2(Point e, RGBColor c) : rc(&c), VObject(e)
	{ }
    void Draw(Rectangle r)
	{ GrPaintRect(r, &rc); }
    void SetColor(RGBColor c)   
	{ if (rc.SetRGB(c.GetRed(), c.GetGreen(), c.GetBlue(), rc.GetPrec()))
		ForceRedraw(); }
    Command *DoLeftButtonDownCommand(Point, Token, int)
	{ SetColor(PickColor(rc, this)); return gNoChanges; }
};

//---- myView ------------------------------------------------------------------

class myView: public DialogView {
    Cluster *bmp;
    CollectionView *cv;
public:
    MetaDef(myView);
    myView(Document *dp) : DialogView(dp)
	{ }
    VObject *DoCreateDialog();
};

MetaImpl0(myView);
	    
VObject *myView::DoCreateDialog()
{
    CompositeVObject *v= new Expander(cIdNone, eHor, gPoint8, (Collection*)0);
    for (int i= 0; i < 256; i+= 16) {
	RGBColor rc((short)i);
	v->Add(new ColorCell2(Point(20), rc));
    }
    return new BorderItem("Palette", v);
}

//---- myDocument ---------------------------------------------------------------

class myDocument : public Document {
    View *view;
public:
    MetaDef(myDocument);
    myDocument()
	{ }
    ~myDocument()
	{ /* SafeDelete(view); */ }
    Window *DoMakeWindows();
};

MetaImpl(myDocument, (TP(view), 0));

Window *myDocument::DoMakeWindows()
{
    view= new myView(this);
    return new Window(this, gPoint0, eWinDefault, view);
}

//---- color -------------------------------------------------------------------

class color: public Application {
public:
    MetaDef(color);
    color(int argc, char **argv);
    Document *DoMakeDocuments(const char *)
	{ return new myDocument; }
};

MetaImpl0(color);

color::color(int argc, char **argv) : Application(argc, argv)
{
    ApplInit();
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    color mycolor(argc, argv);
    
    return mycolor.Run();
}
