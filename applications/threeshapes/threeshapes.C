//$Shape,BoxShape,OvalShape,TextShape,PatternCommand,PatternMenuItem$
//$ShapeView,ShapeDocument,threeshapes$

#include "ET++.h"

#include "OrdColl.h"
#include "TextView.h"
#include "GapText.h"

const int cPATTERNMENU  =   cUSERCMD,
	  cSETPATTERN   =   cUSERCMD + 100,
	  cMAXINKS      =   50;

static char *aboutMsg= 
"Extended version of @Btwoshapes@P illustrating how to integrate text\n\
(resize shapes with @Icntl@P and @Ileft mouse button@P)";

static Ink *palette[cMAXINKS];

//--- Shape --------------------------------------------------------------------

class Shape: public VObject { 
protected:
    int ink;
public:
    MetaDef(Shape);
    
    Shape(Rectangle r) : VObject(r)
	{ ink= 8; }
    Metric GetMinSize()
	{ return Metric(20); }
    void Draw(Rectangle)
	{ GrSetPattern(palette[ink]); }
    void SetPattern(int newpat);
    int GetPattern()
	{ return ink; }
    Command *DoLeftButtonDownCommand(Point p, Token t, int cl);
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

MetaImpl(Shape, (T(ink), 0));

void Shape::SetPattern(int newpat) 
{
    ink= newpat;
    ForceRedraw();
    Changed();
}

Command *Shape::DoLeftButtonDownCommand(Point, Token t, int)
{
    if (t.Flags & eFlgCntlKey)
	return GetStretcher();
    return GetMover();
}

ostream &Shape::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << ink SP;
}

istream &Shape::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    return s >> ink;
}

//---- BoxShape --------------------------------------------------------

class BoxShape: public Shape {
public:
    MetaDef(BoxShape);
    BoxShape(Rectangle r) : Shape(r)
	{ }
    void Draw(Rectangle r)
	{ Shape::Draw(r); GrFillRect(contentRect); GrStrokeRect(contentRect); }
};

MetaImpl0(BoxShape);

//---- OvalShape --------------------------------------------------------

class OvalShape: public Shape {
public:
    MetaDef(OvalShape);
    OvalShape(Rectangle r) : Shape(r)
	{ }
    void Draw(Rectangle r)
	{ Shape::Draw(r); GrFillOval(contentRect); GrStrokeOval(contentRect); }
};

MetaImpl0(OvalShape);

//---- TextShape --------------------------------------------------------

class TextShape: public BoxShape {
    TextView *tv;
public:
    MetaDef(TextShape);
    TextShape(Rectangle);
    ~TextShape();

    void Init(Text*);
    void SetOrigin(Point at);
    void SetExtent(Point e);
    Metric GetMinSize()
	{ return Metric(40, 10); }
    void Draw(Rectangle r);
    void Control(int, int part, void*);
    TextView *GetTextView()
	{ return (TextView*) tv; }
    Command *DispatchEvents(Point p, Token t, Clipper *vf);
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
    void Parts(Collection*);
};

MetaImpl(TextShape, (TP(tv), 0));

TextShape::TextShape(Rectangle r) : BoxShape(r)
{
    Init(new GapText((byte*)"an example of a textshape"));
}

TextShape::~TextShape()
{ 
    SafeDelete(tv); 
}

void TextShape::Init(Text *t)
{
    Rectangle rr= contentRect.Inset(4);
    rr.extent.y= cFit;
    SafeDelete(tv);
    tv= new TextView(this, rr, t);
    Shape::SetExtent(tv->GetExtent() + gPoint4*2);
    tv->SetContainer(this);
    tv->SetFlag(eVObjLayoutCntl);
}

void TextShape::SetOrigin(Point at)
{
    Shape::SetOrigin(at);
    tv->SetOrigin(at+gPoint4);
}

void TextShape::SetExtent(Point e)
{
    tv->SetExtent(e-2*gPoint4);
}

void TextShape::Draw(Rectangle r)
{ 
    BoxShape::Draw(r);
    tv->DrawAll(r, FALSE);
}

void TextShape::Control(int, int part, void *op)
{
    if (op == tv && (part == cPartExtentChanged || part == cPartOriginChanged)) {
	ForceRedraw();
	contentRect= tv->ContentRect().Expand(4);
	ForceRedraw();
    }
}

Command *TextShape::DispatchEvents(Point p, Token t, Clipper *vf)
{
    Command *cmd= tv->VObject::Input(p, t, vf);
    if (cmd)
	return cmd;
    return Shape::DispatchEvents(p, t, vf);
}

ostream &TextShape::PrintOn(ostream &s)
{ 
    BoxShape::PrintOn(s);
    return s << tv->GetText() SP;
}

istream &TextShape::ReadFrom(istream &s)
{ 
    Text *t;
    BoxShape::ReadFrom(s);
    s >> t;
    Init(t);
    return s;
}

void TextShape::Parts(Collection* col)
{
    Shape::Parts(col);
    col->Add(tv);
}

//---- PatternCommand --------------------------------------------------------

class PatternCommand: public Command {
    Shape *shape;
    int oldPattern, newPattern;
public:
    PatternCommand(Shape *s, int newpat);

    void DoIt()
	{ shape->SetPattern(newPattern); }
    void UndoIt()
	{ shape->SetPattern(oldPattern); }
};

PatternCommand::PatternCommand(Shape *s, int newpat) : Command("set ink")
{
    shape= s; 
    newPattern= newpat; 
    oldPattern= shape->GetPattern();
}

//---- PatternMenuItem ---------------------------------------------------------

class PatternMenuItem : public VObject {
    int ink;
public:
    MetaDef(PatternMenuItem);
    PatternMenuItem(int id) : VObject(id+cSETPATTERN)
	{ ink= id; SetExtent(Point(50, 20)); }
    void Draw(Rectangle)
	{ GrPaintRect(contentRect.Inset(3), palette[ink]); }
};

MetaImpl(PatternMenuItem, (T(ink), 0));

//---- ShapeView ---------------------------------------------------------

class ShapeView: public View {
    SeqCollection *list;
    Shape *selected;
    TextShape *textshape;
    Bitmap *bm;
public:
    MetaDef(ShapeView);
    
    ShapeView(Document *d, Point ext);
    ~ShapeView();

    void Draw(Rectangle r)
	{ list->ForEach(Shape,DrawAll)(r); }
    Command *DispatchEvents(Point, Token, Clipper*);
    Command *DoMenuCommand(int);
    void DoCreateMenu(Menu*);
    void DoSetupMenu(Menu*);
    void SetShapes(SeqCollection *shapes);
    ostream &PrintOn(ostream &s)
	{ return s << list; }
    istream &ReadFrom(istream &);
    void Parts(Collection*);
};

MetaImpl(ShapeView, (TP(list), TP(selected), TP(textshape), 0));

ShapeView::ShapeView(Document *d, Point ext) : View(d, ext)
{   
    if (palette[0] == 0)
	for (int i= 0; i < 32; i++)
	    palette[i]= new RGBColor((short)(i*8));

    SeqCollection *l= new OrdCollection;
    l->Add(new BoxShape(Rectangle(100,100,100,100))); 
    l->Add(new OvalShape(Rectangle(150,150,100,100)));
    l->Add(new TextShape(Rectangle(20,20,100,100)));
    SetShapes(l);
}

ShapeView::~ShapeView()
{
    if (list) {
	list->FreeAll();
	SafeDelete(list);
    }
}

Command *ShapeView::DispatchEvents(Point p, Token t, Clipper *vf)
{
    Iter next(list);
    Shape *s;
    
    selected= 0;
    while (s= (Shape*) next())
	if (s->ContainsPoint(p))
	    selected= s;
    if (selected) 
	return selected->Input(p, t, vf);
    return View::DispatchEvents(p, t, vf);
}

void ShapeView::SetShapes(SeqCollection *shapes) 
{
    if (list) {
	list->FreeAll();
	SafeDelete(list);
    } 
    list= shapes;
    Iter next(list);
    Shape *shape;
    
    while (shape= (Shape*) next()) {
	shape->SetContainer(this);
	if (shape->IsKindOf(TextShape))
	    textshape= (TextShape*)shape;
    }   
}

istream &ShapeView::ReadFrom(istream &s)
{
    SeqCollection *newlist= 0;
    s >> newlist;
    SetShapes(newlist);
    ForceRedraw();
    return s;
}

void ShapeView::DoCreateMenu(Menu *menu)
{
    Menu *submenu= new Menu("patterns", FALSE, 0, 2);

    for (int i= 0; palette[i]; i++)
	submenu->Append(new PatternMenuItem(i));
    
    View::DoCreateMenu(menu);
    menu->AppendItems("-", "About", cABOUT, 0);
    menu->AppendMenu(submenu, cPATTERNMENU);
}

void ShapeView::DoSetupMenu(Menu *menu)
{
    View::DoSetupMenu(menu);
    for (int p= 0; palette[p]; p++)
	menu->EnableItem(cSETPATTERN+p);
    if (! textshape->GetTextView()->Caret())
	menu->EnableItems(cCUT, cCOPY, 0);
    menu->EnableItems(cPATTERNMENU, cABOUT, cPASTE, 0);
}

Command *ShapeView::DoMenuCommand(int cmd)
{
    if (cmd >= cSETPATTERN && cmd <= cSETPATTERN+cMAXINKS && selected)
	return new PatternCommand(selected, cmd-cSETPATTERN);
    if (cmd == cCUT || cmd == cCOPY || cmd == cPASTE)
	return textshape->GetTextView()->DoMenuCommand(cmd);
    return View::DoMenuCommand(cmd);
}

void ShapeView::Parts(Collection* col)
{
    View::Parts(col);
    col->Add(list);
}

//---- ShapeDocument -----------------------------------------------------

char *cDocTypeShapes = "THREESHAPES";

class ShapeDocument: public Document {
    ShapeView *view;
public:
    MetaDef(ShapeDocument);
    ShapeDocument() : Document(cDocTypeShapes)
	{ }
    ~ShapeDocument()
	{ SafeDelete(view); }
    
    Window *DoMakeWindows();
    void DoWrite(ostream &s, int o)
	{ Document::DoWrite(s, o); view->PrintOn(s); }
    void DoRead(istream &s, FileType *ft)
	{ Document::DoRead(s, ft); view->ReadFrom(s); }
};

MetaImpl(ShapeDocument, (TP(view), 0));

Window *ShapeDocument::DoMakeWindows()
{
    view= new ShapeView(this, Point(1000));
    return new Window(this, Point(400), eWinDefault, new Splitter(view));
}

//---- ShapeApplication --------------------------------------------------

class threeshapes: public Application {
public:
    threeshapes(int argc, char *argv[]);
    Document *DoMakeDocuments(const char *)
	{ return new ShapeDocument(); }
    void About()
	{ ShowAlert(eAlertNote, aboutMsg); }
};

threeshapes::threeshapes(int argc, char *argv[]) : Application(argc, argv, cDocTypeShapes)
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

//---- main --------------------------------------------------------------

main(int argc, char *argv[])
{
    threeshapes myapp(argc, argv);
    
    return myapp.Run();
}          
