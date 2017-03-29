//$Shape,BoxShape,OvalShape,PatternCommand,PatternMenuItem$
//$ShapeView,ShapeDocument,twoshapes$

#include "ET++.h"

const int cSETINK = cUSERCMD + 100,
	  cMAXINKS= 50;

static char *aboutMsg= 
"Resize shapes with @Icntl@P and @Ileft mouse button@P";

static Ink *palette[cMAXINKS];

//--- Shape --------------------------------------------------------------------

class Shape: public VObject { 
    int inkid;
public:
    MetaDef(Shape);
    
    Shape(Rectangle r) : (r)
	{ inkid= 3; }

    Metric GetMinSize()
	{ return Metric(5); }
    void Draw(Rectangle)
	{ GrSetPattern(palette[inkid]); }
    void SetInk(int newink) 
	{ inkid= newink; ForceRedraw(); Changed(); }
    int GetInk()
	{ return inkid; }
    ostream &PrintOn(ostream &s)
	{ VObject::PrintOn(s);
	  return s << inkid SP; }
    istream &ReadFrom(istream &s)
	{ VObject::ReadFrom(s);
	  return s >> inkid; }
};

MetaImpl(Shape, (T(inkid), 0));

//---- BoxShape --------------------------------------------------------

class BoxShape: public Shape {
public:
    MetaDef(BoxShape);
    BoxShape(Rectangle r) : Shape(r)
	{ }
    void Draw(Rectangle r)
	{ Shape::Draw(r); GrFillRect(contentRect); }
};

MetaImpl0(BoxShape);

//---- OvalShape --------------------------------------------------------

class OvalShape: public Shape {
public:
    MetaDef(OvalShape);
    OvalShape(Rectangle r) : Shape(r)
	{ }
    void Draw(Rectangle r)
	{ Shape::Draw(r); GrFillOval(contentRect); }
};

MetaImpl0(OvalShape);

//---- PatternCommand --------------------------------------------------------

class PatternCommand: public Command {
    Shape *shape;
    int oldink, newink;
public:
    PatternCommand(Shape *s, int ni) : Command("set ink")
	{ shape= s; newink= ni; oldink= shape->GetInk(); }
    void DoIt()
	{ shape->SetInk(newink); }
    void UndoIt()
	{ shape->SetInk(oldink); }
};

//---- PatternMenuItem ---------------------------------------------------------

class PatternMenuItem : public VObject {
    Ink *ink;
public:
    MetaDef(PatternMenuItem);
    PatternMenuItem(int id, Ink *p) : VObject(id)
	{ ink= p; SetExtent(Point(50, 20)); }
    void Draw(Rectangle)
	{ GrPaintRect(contentRect.Inset(3), ink); }
};

MetaImpl(PatternMenuItem, (TP(ink), 0));

//---- ShapeView ---------------------------------------------------------------

class ShapeView: public View {
    Shape *shape1, *shape2;
public:
    MetaDef(ShapeView);
    
    ShapeView(Document *d, Point ext);
    ~ShapeView();

    void Draw(Rectangle r);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    Command *DoMenuCommand(int);
    void DoCreateMenu(Menu*);
    void DoSetupMenu(Menu*);
    ostream &PrintOn(ostream &s)
	{ return s << shape1 << shape2; }
    istream &ReadFrom(istream &);
};

MetaImpl(ShapeView, (TP(shape1), TP(shape2), 0));

ShapeView::ShapeView(Document *d, Point ext) : View(d, ext)
{
    shape1= new BoxShape(Rectangle(100,100,100,100));
    shape2= new OvalShape(Rectangle(150,150,100,100));
    shape1->SetContainer(this);
    shape2->SetContainer(this);
}
    
ShapeView::~ShapeView()
{   
    SafeDelete(shape1);
    SafeDelete(shape2);
}

void ShapeView::Draw(Rectangle r)
{
    shape1->Draw(r); 
    shape2->Draw(r); 
}

Command *ShapeView::DoLeftButtonDownCommand(Point p, Token t, int cl)
{
    Shape *s= 0;
    
    if (shape2->ContainsPoint(p))
	s= shape2;
    else if (shape1->ContainsPoint(p))
	s= shape1;
    if (s) {
	if (t.Flags & eFlgCntlKey)
	    return s->GetStretcher();
	return s->GetMover();
    }
    return View::DoLeftButtonDownCommand(p, t, cl);
}

istream &ShapeView::ReadFrom(istream &s)
{
    SafeDelete(shape1);
    SafeDelete(shape2);
    s >> shape1 >> shape2;
    shape1->SetContainer(this);
    shape2->SetContainer(this);
    ForceRedraw();
    return s;
}

Command *ShapeView::DoMenuCommand(int cmd)
{
    if (cmd >= cSETINK && cmd <= cSETINK+cMAXINKS)
	return new PatternCommand(shape1, cmd-cSETINK);
    return View::DoMenuCommand(cmd);
}

void ShapeView::DoCreateMenu(Menu *menu)
{
    View::DoCreateMenu(menu);
    menu->AppendItems("About", cABOUT, "-", 0);
    for (int i= 0; palette[i]; i++)
	menu->Append(new PatternMenuItem(cSETINK+i, palette[i]));
}

void ShapeView::DoSetupMenu(Menu *menu)
{
    View::DoSetupMenu(menu);
    for (int p= 0; palette[p]; p++)
	menu->EnableItem(cSETINK+p);
    menu->EnableItem(cABOUT);
}

//---- ShapeDocument -----------------------------------------------------

char *cDocTypeShapes = "TWOSHAPES";

class ShapeDocument: public Document {
    ShapeView *view;
public:
    MetaDef(ShapeDocument);
    ShapeDocument() : Document(cDocTypeShapes) 
	{ }
    Window *DoMakeWindows();
    void DoWrite(ostream &s, int f);
    void DoRead(istream &s, FileType *ft);
};

MetaImpl(ShapeDocument, (TP(view), 0));

void ShapeDocument::DoWrite(ostream &s, int f)
{ 
    Document::DoWrite(s, f); 
    view->PrintOn(s); 
}

void ShapeDocument::DoRead(istream &s, FileType *ft)
{   
    Document::DoRead(s, ft); 
    view->ReadFrom(s); 
}

Window *ShapeDocument::DoMakeWindows()
{
    view= new ShapeView(this, Point(600));
    return new Window(this, Point(400), eWinDefault, new Splitter(view));
}

//---- ShapeApplication --------------------------------------------------

class twoshapes: public Application {
public:
    twoshapes(int argc, char *argv[]);
    Document *DoMakeDocuments(const char*);
    void About()
	{ ShowAlert(eAlertNote, aboutMsg); }
};

twoshapes::twoshapes(int argc, char *argv[]) : Application(argc, argv, cDocTypeShapes)
{
    ApplInit();
    
    int p= 0;
    palette[p++]= ePatWhite;
    palette[p++]= new RGBColor((float)0.75);
    palette[p++]= new RGBColor((float)0.5);
    palette[p++]= new RGBColor((float)0.25);
    palette[p++]= ePatBlack;
    palette[p++]= ePat00;
    palette[p++]= ePat01;
    palette[p++]= ePat02;
    palette[p++]= new RGBColor(255, 0, 0);
    palette[p++]= new RGBColor(0, 255, 0);
    palette[p++]= new RGBColor(0, 0, 255);
}

Document *twoshapes::DoMakeDocuments(const char*)
{ 
    return new ShapeDocument(); 
}

//---- main --------------------------------------------------------------

main(int argc, char *argv[])
{
    twoshapes atwoshapes(argc, argv);
    
    return atwoshapes.Run();
}          
