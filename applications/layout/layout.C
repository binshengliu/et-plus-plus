//$Shape,Handle,ShapeView,ShapeDocument,layout,Connection,Connector,HandleMover$

#include "ET++.h"

#include "OrdColl.h"
#include "DialogItems.h" 
#include "Slider.h"
#include "ScrollBar.h"
#include "TextView.h"
#include "CheapText.h"
#include "PopupItem.h"

const int cBorderWidth  = 1,
	  cGap          = 4,
	  cHandleSize   = 7,
	  cAll          = cHandleSize+cGap+cBorderWidth,
	  cAll2         = cAll*2;

static char *msg= "To use help push the button over the item you want help \
To use help push the button over the item you want help To use help pushTo \
use help push the button over the item you want help for. \
Press the left mouse button now etc...";
	  
//---- HandleMover -------------------------------------------------------------

class HandleMover: public VObjectMover {
    Rectangle rr;
public:
    HandleMover(VObject *g, Rectangle r) : VObjectMover(g, r)
	{ rr= constrainRect; rr.extent-= oldRect.extent-1; }
    void TrackConstrain(Point, Point, Point *np);
    virtual Point ConstrainDelta(Point);
    void DoIt();
    void UndoIt();
};

//---- Connector ---------------------------------------------------------------

class Connector: public Command {
    class Handle *starthandle, *endhandle;
    Point start, end;
    class ShapeView *view;
public:
    Connector(View *sv, Handle *h);
    
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase atp, Point ap, Point, Point np);
};

//--- Handle -------------------------------------------------------------------

class Handle: public VObject {
public:
    int angle;
    class Shape *sp;
public:
    MetaDef(Handle);
    
    Handle(int n, Shape *sp, View *vp, int ang);
    
    int Angle()
	{ return angle; }
	
    void SetOrigin(Point at);
    void Draw(Rectangle);
    Point Center()
	{ return contentRect.Center(); }
    Command *DoMiddleButtonDownCommand(Point, Token, int);
    Command *DoLeftButtonDownCommand(Point, Token, int)
	{ return new Connector(GetView(), this); }
    
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
    void SetAngle();
};

//--- Connection ---------------------------------------------------------------

class Connection: public VObject {
    Point startp, endp;
    Handle *starth, *endh;
public:
    MetaDef(Connection);
    
    Connection(View*, Handle*, Handle*);
    ~Connection();
    
    void UpdatePoints();
    void Draw(Rectangle);
    void DoObserve(int id, int part, void*, Object *op);
    
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

//--- Shape --------------------------------------------------------------------

class Shape: public CompositeVObject {
public:
    MetaDef(Shape);
    Shape(VObject *gop= 0, int i1=9999, int i2=9999, int i3=9999, int i4=9999);
    
    void Init(View *vp, Point at);
    void Update(int);
    class Handle *Conn(int n)
	{ return (Handle*)list->At(n+1); }
    
    void SetExtent(Point e);
    void SetOrigin(Point at);
    Metric GetMinSize();
    void AddHandle(int n, int ang);

    void DrawInner(Rectangle, bool);
    
    Command *DoLeftButtonDownCommand(Point p, Token t, int);

    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

//---- ShapeView ---------------------------------------------------------------

class ShapeView: public View {
    SeqCollection *shapes, *connections;
public:
    MetaDef(ShapeView);
    
    ShapeView(Document *d, Point ext);
    ~ShapeView();

    void Draw(Rectangle r);
    Command *DispatchEvents(Point p, Token t, Clipper *vf);
    Shape *FindShape(Point);
    void Add(VObject *gop)
	{ shapes->Add(gop); }
    void AddConn(VObject *gop)
	{ connections->Add(gop); }
    void Open(bool mode= TRUE);
    
    ostream &PrintOn(ostream &);
    istream &ReadFrom(istream &);
};

//---- ShapeDocument -----------------------------------------------------

class ShapeDocument: public Document {
    ShapeView *view;
public:
    MetaDef(ShapeDocument);
    ShapeDocument()
	{ }
    ~ShapeDocument()
	{ SafeDelete(view); }
    
    Window *DoMakeWindows();
    void DoWrite(ostream &s, int)
	{ view->PrintOn(s); }
    void DoRead(istream &s, FileType *)
	{ view->ReadFrom(s); }
};

//--- HandleMover --------------------------------------------------------------

void HandleMover::TrackConstrain(Point ap, Point, Point *np)
{
    *np= ConstrainDelta(*np-ap)+ap;
    //VObjectMover::TrackConstrain(ap, pp, np);
    // *np= rr.AngleToPoint(constrainRect.PointToAngle(*np));
}

Point HandleMover::ConstrainDelta(Point d)
{
    int a= constrainRect.PointToAngle(vop->contentRect.origin+d);
    return constrainRect.AngleToPoint(a)-vop->contentRect.origin;
}

void HandleMover::DoIt()
{
    VObjectMover::DoIt();
    ((Handle*)vop)->SetAngle();
}

void HandleMover::UndoIt()
{
    VObjectMover::UndoIt();
    ((Handle*)vop)->SetAngle();
}

//--- Connection ---------------------------------------------------------------

MetaImpl(Connection, (T(startp), T(endp), TP(starth), TP(endh), 0));

Connection::Connection(View *vp, Handle *sh, Handle *eh) : VObject(vp, gRect0)
{
    starth= sh;
    endh= eh;
    if (sh && eh) {
	starth->AddObserver(this);
	endh->AddObserver(this);
	UpdatePoints();   
    }
}

Connection::~Connection()
{
    if (starth)
	starth->RemoveObserver(this);
    if (endh)
	endh->RemoveObserver(this);
}

void Connection::DoObserve(int, int part, void*, Object *op)
{
    if (starth == (Handle*) op || endh == (Handle*) op) {
	switch (part) {
	case cPartSenderDied:
	    break;
	case cPartOriginChanged:
	case cPartExtentChanged:
	    UpdatePoints();
	    break;
	}
    }
}

void Connection::UpdatePoints()
{
    ForceRedraw();
    startp= endh->Center();
    endp= starth->Center();
    contentRect= NormRect(startp, endp);
    ForceRedraw();
}

void Connection::Draw(Rectangle)
{
    GrPaintLine(ePatBlack, 1, eDefaultCap, startp, endp);
}

ostream &Connection::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << contentRect SP << startp SP << endp SP << starth SP << endh SP;
}

istream &Connection::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    return s >> contentRect >> startp >> endp >> starth >> endh;
}

//--- Shape --------------------------------------------------------------------

MetaImpl0(Shape);

Shape::Shape(VObject *gop, int i1, int i2, int i3, int i4) : CompositeVObject(cIdNone, (Collection*)0)
{
    if (gop)
	list->Add(gop);
    if (i1 != 9999)
	AddHandle(0, i1);
    if (i2 != 9999)
	AddHandle(1, i2);
    if (i3 != 9999)
	AddHandle(2, i3);
    if (i4 != 9999)
	AddHandle(3, i4);
}

void Shape::Init(View *vp, Point at)
{
    SetContainer(vp);
    CalcExtent();
    SetOrigin(at);
}

void Shape::AddHandle(int n, int ang)
{
    list->Add(new Handle(n, this, GetView(), ang));
}

Command *Shape::DoLeftButtonDownCommand(Point, Token t, int)
{
    if (t.Flags & eFlgCntlKey)
	return GetStretcher();
    return GetMover();
}

void Shape::SetOrigin(Point at)
{
    Iter next(list);
    register VObject *s= (VObject*) next();
    VObject::SetOrigin(at);
    s->SetOrigin(at+cAll);
    Rectangle rr= contentRect;
    rr.extent-= Point(cHandleSize);
    rr.origin= at;
    while (s= (VObject*) next())
	s->SetOrigin(rr.AngleToPoint(((Handle*)s)->Angle()));
}

void Shape::SetExtent(Point e)
{
    VObject::SetExtent(e);
    At(0)->SetExtent(e-cAll2);
}

Metric Shape::GetMinSize()
{
    return At(0)->GetMinSize().Expand(cAll);
}

void Shape::DrawInner(Rectangle r, bool highlight)
{
    Rectangle rr= contentRect.Inset(cHandleSize);
    
    GrEraseRect(rr);
    GrSetPenNormal();
    GrStrokeRect(rr);
    CompositeVObject::DrawInner(r, highlight);
}

ostream &Shape::PrintOn(ostream &s)
{
    CompositeVObject::PrintOn(s);
    return s << contentRect SP;
}

istream &Shape::ReadFrom(istream &s)
{
    CompositeVObject::ReadFrom(s);
    return s >> contentRect;
}

//--- Handle --------------------------------------------------------------------

MetaImpl(Handle, (TP(sp), T(angle), 0));

Handle::Handle(int, Shape *s, View *vp, int ang) : VObject(vp, Point(cHandleSize))
{
    angle= ang;
    sp= s;
}

void Handle::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    Changed();
}

void Handle::SetAngle()
{
    Rectangle rr= sp->contentRect;
    rr.extent-= Point(cHandleSize);
    angle= rr.PointToAngle(GetOrigin());
}

void Handle::Draw(Rectangle)
{
    GrPaintRect(contentRect, ePatBlack);
}

Command *Handle::DoMiddleButtonDownCommand(Point, Token, int)
{
    return new HandleMover(this, sp->contentRect);
}

ostream &Handle::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << contentRect SP << sp SP;
}

istream &Handle::ReadFrom(istream &s)
{
    VObject::ReadFrom(s);
    return s >> contentRect >> sp;
}

//---- ShapeView ---------------------------------------------------------------

MetaImpl(ShapeView, (TP(shapes), 0));

static int level;

ShapeView::ShapeView(Document *d, Point ext) : View(d, ext)
{
    Shape *s;
    Menu *menu;
    VObject *gop;
    Point at;
    level++;
    
    menu= new Menu("Parity", FALSE, 0, 1, FALSE);
    menu->AppendItems("Even", 50, "Odd", 51, "Mark", 52, "Space", 53, 0);

    connections= new OrdCollection;
    shapes= new OrdCollection;
    
    for (int i= 0; i < 12; i++) {
	gop= 0;
	s= 0;
	switch (i) {
	case 0:
	    gop= new ActionButton(i, "action");
	    s= new Shape(gop, 180);
	    break;
	    
	case 1:
	    gop= new ActionButton(i, "default", TRUE);
	    s= new Shape(gop, 180);
	    break;
	    
	case 2:
	    gop= new ScrollBar(cIdNone, eVert);
	    break;
	    
	case 3:
	    gop= new PopupItem(i, 50, menu);
	    break;
	    
	case 4:
	    gop= new BorderItem("Baudrate",
		new OneOfCluster(i, VObjAlign(eVObjHLeft|eVObjVTop), Point(15, 5),
		    "50", "110", "150", "300", "600", "1200", "2400",
		    "3600", "4800", "7200", "9600", "19200", 0
		),
		gBorder,
		1,
		eVObjHCenter
	    );
	    break;
	    
	case 5:
	    gop= new Scroller(
		    new TextView(GetNextHandler(), Rectangle(Point(100,cFit)),
			    new CheapText((byte*)msg), eJustified), Point(200,100));
	    s= new Shape(gop, 270);
	    break;
	    
	case 6:
	    gop= new RadioButton;
	    s= new Shape(gop, 180);
	    break;
	    
	case 7:
	    gop= new Cluster(i, VObjAlign(eVObjHExpand+eVObjVCenter), gPoint0,
		new ActionButton(i, "a"),
		new ActionButton(i, "abc"),
		new ActionButton(i, "12345678"),
		new ActionButton(i, "bla", TRUE),
		new ActionButton(i, "a"),
		new ActionButton(i, "abc"),
		new ActionButton(i, "12345678"),
		new ActionButton(i, "bla", TRUE),
		new ActionButton(i, "a"),
		new ActionButton(i, "abc"),
		new ActionButton(i, "12345678"),
		new ActionButton(i, "bla", TRUE),
		new ActionButton(i, "12345678"),
		new ActionButton(i, "bla", TRUE),
		0
	    );
	    break;
	    
	case 8:
	    gop= new Cluster(i, eVObjHCenter, Point(10),
		new ActionButton(i, "a"),
		new RadioButton,
		new BorderItem(
		    new Cluster(i, eVObjVBase, Point(10, 0),
			new ActionButton(i+3, "default", TRUE),
			new RadioButton,
			new ActionButton(i+2, "ab"),
			0
		    )
		),
		new ActionButton(i+3, "default", TRUE),
		0
	    );
	    break;
	    
	case 9:
	    gop= new Cluster(i, eVObjVBase, Point(10, 0),
		new ActionButton(i, "a"),
		new RadioButton,
		new BorderItem(
		    new Cluster(i, eVObjVBase, Point(10, 0),
			new ActionButton(i+3, "default", TRUE),
			new RadioButton,
			new ActionButton(i+2, "ab"),
			0
		    )
		),
		new ActionButton(i+3, "default", TRUE),
		0
	    );
	    break;
	    
	case 10:
	    if (level < 3)
		gop= new Scroller(
		new ShapeView((Document*) GetNextHandler(), Point(1000)), Point(200));
	    else
		gop= new Slider;
	    s= new Shape(gop, 270);
	    break;
	    
	default:
	    break;
	}
	
	if (gop) {
	    if (s == 0)
		s= new Shape(gop, -30, 0, 30, 180);
		
	    s->Init(this, at);
	    at+= Point(20,40);
	    Add(s);
	}
    }
}

void ShapeView::Open(bool mode)
{
    shapes->ForEach(Shape,Open)(mode);
}

ShapeView::~ShapeView()
{   
    if (shapes) {
	shapes->FreeAll();
	SafeDelete(shapes);
    }
    if (connections) {
	connections->FreeAll();
	SafeDelete(connections);
    }
}

void ShapeView::Draw(Rectangle r)
{
    shapes->ForEach(Shape,DrawAll)(r);
    connections->ForEach(Handle,DrawAll)(r);
}

Shape *ShapeView::FindShape(Point p)
{
    RevIter next(shapes);
    Shape *s;
    
    while (s= (Shape*) next())
	if (s->ContainsPoint(p))
	    return s;
    return 0;
}

Command *ShapeView::DispatchEvents(Point p, Token t, Clipper *vf)
{
    Shape *s;
    
    if (s= FindShape(p))
	return s->Input(p, t, vf);
    return View::DispatchEvents(p, t, vf);
}

ostream &ShapeView::PrintOn(ostream &s)
{
    return s << shapes SP;
}

istream &ShapeView::ReadFrom(istream &s)
{
    if (shapes)
	shapes->FreeAll();
    s >> shapes;
    shapes->ForEach(Shape,SetContainer)(this);  
    ForceRedraw();
    return s;
}

//---- Connector ---------------------------------------------------------------

Connector::Connector(View *sv, Handle *h)
{
    view= (ShapeView*) sv;
    starthandle= h;
    endhandle= 0;
    start= starthandle->Center();
}

void Connector::TrackFeedback(Point, Point, bool)
{
    GrLine(start, end);
}

Command *Connector::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    Handle *h;
    Shape *s;
    
    end= np;
    endhandle= 0;
    if (s= view->FindShape(np)) {
	if ((h= (Handle*)s->FindItem(np)) && h->IsKindOf(Handle)) {
	    endhandle= h;
	    end= endhandle->Center();
	}
    }
	    
    if (atp == eTrackRelease) {
	if (endhandle)
	    view->AddConn(new Connection(view, starthandle, endhandle));
	return gNoChanges;
    }
    return this;
}

//---- ShapeDocument -----------------------------------------------------

MetaImpl(ShapeDocument, (TP(view), 0));

Window *ShapeDocument::DoMakeWindows()
{
    view= new ShapeView(this, Point(2000, 1200));
    return new Window(this, Point(600, 400), eWinDefault, new Splitter(view));
}

//---- layout ------------------------------------------------------------------

class layout: public Application {
public:
    layout(int argc, char *argv[]) : Application(argc, argv)
	{ ApplInit(); }
    Document *DoMakeDocuments(const char*)
	{ return new ShapeDocument(); }
};

//---- main --------------------------------------------------------------------

main(int argc, char *argv[])
{
    layout alayout(argc, argv);

    return alayout.Run();
}          
