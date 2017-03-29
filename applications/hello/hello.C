//
// Hello world with ET++:
// - the string "hello world" is shown in a scrollable window
// - the string can be moved with the left mouse button
// - the current layout can be stored in a file
//

//$HelloView,HelloDocument,Hello$

#include "ET++.h"

//---- HelloView -------------------------------------------------------------------

class HelloView: public View { 
    TextItem *hello;
public:
    MetaDef(HelloView);
    HelloView(Document *dp, Rectangle itsExtent);
    void Draw(Rectangle r);
    void SetText(TextItem *t);
    Command *DoLeftButtonDownCommand(Point, Token, int);
};

MetaImpl(HelloView, (TP(hello), 0));
	    
HelloView::HelloView(Document *dp, Rectangle itsExtent) : View(dp, itsExtent)
{ 
}

void HelloView::SetText(TextItem *t)
{
    hello= t;
    // force the the text item to calculate its extent 
    hello->CalcExtent(); 
    // install the text item in this view
    hello->SetContainer(this);
    ForceRedraw();
}

void HelloView::Draw(Rectangle r)
{ 
    hello->DrawAll(r); 
}

Command *HelloView::DoLeftButtonDownCommand(Point p, Token, int)
{ 
    // return generic VObject mover if "hello world" was hit
    if (hello->ContainsPoint(p)) 
	return hello->GetMover();
    // return "NOP" preserves previous command 
    return gNoChanges;
}

//---- HelloDocument ---------------------------------------------------------------

class HelloDocument : public Document {
    HelloView *view;
    TextItem *text;
public:
    MetaDef(HelloDocument);
    HelloDocument();
    ~HelloDocument();
    Window *DoMakeWindows();
    void DoRead(istream &, class FileType *);
    void DoWrite(ostream &, int);
};

MetaImpl(HelloDocument, (TP(view), TP(text), 0));

HelloDocument::HelloDocument() : Document("HELLO")
{
    static int family= 0;

    family= (family+1) % 10;  // cycle through first 10 font families 
    text= new TextItem("hello world",
	    new_Font(GrFont(family), 24, GrFace(eFaceBold|eFaceShadow))); 
    text->SetOrigin(Point(50));
}

HelloDocument::~HelloDocument()
{
    SafeDelete(view); 
    SafeDelete(text); 
}

Window *HelloDocument::DoMakeWindows()
{
    view= new HelloView(this, Point(1000));
    view->SetText(text);
    return new Window(this, Point(250), eWinDefault, new Scroller(view));
}

void HelloDocument::DoRead(istream &is, class FileType *ft)
{
    Document::DoRead(is, ft);    // read file type information
    SafeDelete(text);
    is >> text;
    view->SetText(text);
}

void HelloDocument::DoWrite(ostream &os, int flag)
{
    Document::DoWrite(os, flag);  // store file type information
    os << text;
}

//---- Hello -------------------------------------------------------------------

class Hello: public Application {
public:
    MetaDef(Hello);
    Hello(int argc, char **argv); 
    Document *DoMakeDocuments(const char *);
};

MetaImpl0(Hello);

Hello::Hello(int argc, char **argv) : Application(argc, argv, "HELLO")
{ 
    ApplInit();
}

Document *Hello::DoMakeDocuments(const char *)
{
    return new HelloDocument; 
}

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    Hello hello(argc, argv);
    
    return hello.Run();
}
