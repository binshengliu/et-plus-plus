//$TroffDocument,trofftool,Page,Parser,PageScanner,PrintScanner,TextScanner$
//$PageView,FindScanner,TroffFind$

#include "ET++.h"
#include "font.h"
#include "OrdColl.h"
#include "Picture.h"
#include "PictPort.h"
#include "DialogItems.h"
#include "TextView.h"
#include "PrintDialog.h"
#include "WindowSystem.h"
#include "ClipBoard.h"

const int cMaxFonts = 20,
	  cScaleFac = 8,
	  cIdFind   = cIdFirstUser + 1,
	  cIdDoFind = cIdFirstUser + 2,
	  cIdFindOpt= cIdFirstUser + 3,
	  cNEXT     = cUSERCMD + 1,
	  cPREV     = cUSERCMD + 2;

extern unsigned char upcase[], identitymap[];

// generic scale
#define Scale(x) ((x) / cScaleFac)

//---- find/change dialog --------------------------------------------------

class TroffFind : public Dialog {
    class TroffDocument *doc;
    EditTextItem *ei1;
    bool matchCase, matchWord;
public:
    MetaDef(TroffFind);
    TroffFind(char *title, TroffDocument *d);
    VObject *DoCreateDialog();
    void DoSetup();
    void Control(int id, int, void *v);
    bool DoFind(EditTextItem*, bool msg= TRUE);
};

//---- Page --------------------------------------------------------------------

class Page: public VObject {
    class Parser *parser;
    int pageno;
    long pos;
    Picture *pict;    

private:
    Picture *Load();

public:
    MetaDef(Page);

    Page(Parser *pa, long p, int n);
    ~Page()
	{ SafeDelete(pict); }
    Metric GetMinSize();
    void Draw(Rectangle);
    Rectangle SelRect(Rectangle);
    Picture *GetPict()
	{ return pict ? pict : Load(); }
};

//---- TroffDocument -----------------------------------------------------------

class TroffDocument : public Document {
public:
    class Parser *parser;
    class PageView *view;
    istream *fp;
    OrdCollection *pages;
    TroffFind *findChange;
    Fontmap *fmap;
public:
    MetaDef(TroffDocument);
    TroffDocument();
    ~TroffDocument();
    Window *DoMakeWindows();
    void DoRead(istream&, FileType *ft);
    void DoWrite(ostream &s, int);
    void DoCreateMenu(Menu *mp);
    void DoSetupMenu(Menu *mp);
    Command *DoMenuCommand(int cmd);
};

//---- PageView ----------------------------------------------------------------

class PageView : public View {
    Rectangle selection;
    class FindScanner *fparser;
    Collection *pages;
    int current;
    Page *pg;
    Fontmap *fmap;
public:
    MetaDef(PageView);
    PageView(TroffDocument *d, Fontmap *fm);
    ~PageView();
    void SetSelection(Rectangle);
    void Draw(Rectangle);
    bool Match(char *pattern, bool matchCase, bool matchWord);
    void SetPages(Collection *p, char *n);
    void RevealSelection(int page, Rectangle);
    void ConstrainScroll(Point*)
	{ }
    void SetPage(int n);
    int GetPage()
	{ return current; }
    int GetPageCnt()
	{ return pages ? pages->Size() : 0; }
    void FirstPage()
	{ SetPage(0); }
    void LastPage()
	{ SetPage(GetPageCnt()-1); }
    void NextPage()
	{ SetPage(GetPage()+1); }
    void PrevPage()
	{ SetPage(GetPage()-1); }
    void DoCreateMenu(Menu *mp);
    void DoSetupMenu(Menu *mp);
    Command *DoMenuCommand(int cmd);
    Command *DoFunctionKeyCommand(int code, Point p, Token t);
    bool HasSelection();
    void SelectionToClipboard(char *type, ostream &os);
};

//---- Parser ------------------------------------------------------------------

#define FP (*fp)

class Parser : public Object {
protected:
    int currfont, currsize;
    Point currpos;
    istream *fp;
    bool done;
public:
    Parser(istream *f);
    void SkipLine();
    virtual void PutChar(char*)
	{ }
    virtual void DrawingFunction()
	{ SkipLine(); }
    virtual void NewPage(int)
	{ }
    virtual void DeviceControl()
	{ SkipLine(); } 
    virtual void Space()
	{ }
    virtual void Newline()
	{ }
    virtual bool Parse(long pos= 0);
};

Parser::Parser(istream *f)
{
    fp= f;
    currfont= 1;
    currsize= 10;
}

void Parser::SkipLine()
{
    char c;
    do
	FP.get(c);
    while (c != '\n');
}

bool Parser::Parse(long pos)
{
    int n;
    char c, cc, *s= "?", buf[10];
    
    if (pos >= 0)
	FP.seek(pos);
    done= FALSE;
    
    while ((FP >> c) && !done) {
	switch (c) {
	case 'w':       /* word space, Information only */
	    Space();
	    continue;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    /* two motion digits plus a character */
	    FP.get(cc);
	    currpos.x+= (c-'0')*10 + cc-'0';
	case 'c':       /* single ascii character */
	    FP.get(s[0]);
	    PutChar(s);
	    continue;
	case 'C':
	    FP >> buf;
	    PutChar(buf);
	    continue;
	case 'D':       /* draw function */
	    DrawingFunction();
	    continue;
	case 's':
	    FP >> currsize;
	    continue;
	case 'f':
	    FP >> currfont;
	    continue;
	case 'H':       /* absolute horizontal motion */
	    FP >> currpos.x;
	    continue;
	case 'h':       /* relative horizontal motion */
	    FP >> n;
	    currpos.x+= n;
	    continue;
	case 'V':
	    FP >> currpos.y;
	    continue;
	case 'v':
	    FP >> n;
	    currpos.y+= n;
	    continue;
	case 'p':       /* new page */
	    currpos= gPoint0;
	    FP >> n;
	    NewPage(n);
	    continue;
	case 'n':       /* end of line */
	    currpos.x= 0;
	    Newline();
	    SkipLine();
	    continue;
	case '#':       /* comment */
	case '!':       /* transparent mode */
	    SkipLine();
	    continue;
	case 'x':       // device control
	    DeviceControl();
	    continue;
	default:
	    continue;
	}
    }
    FP.clear();
    return !done;
}

//---- PageScanner -------------------------------------------------------------

class PageScanner : public Parser {
    Collection *pages;
    Parser *parser;
public:
    PageScanner(Collection *p, Parser *pa, istream *f) : Parser(f)
	{ pages= p; parser= pa; }
    void NewPage(int pn)
	{ pages->Add(new Page(parser, FP.tell(), pn)); }
};

//---- TextScanner -------------------------------------------------------------

class TextScanner : public Parser {
    ostream *os;
public:
    TextScanner(istream *f, ostream *o) : Parser(f)
	{ os= o; }
    void PutChar(char *s)
	{ *os << s; }
    void Space()
	{ os->put(' '); }
    void Newline()
	{ os->put('\n'); }
};

//---- FindScanner -------------------------------------------------------------

class FindScanner : public Parser {
public:
    char *str, *cp;
    byte *map;
    int page;
    bool matchWord;
    Fontmap *fmap;
    Rectangle m;
public:
    FindScanner(Fontmap *fm, istream *f) : Parser(f)
	{ fmap= fm; }
    ~FindScanner()
	{ SafeDelete(str); }
    void PutChar(char*);
    void Space()
	{ PutChar(" "); }
    void Newline()
	{ PutChar("\n"); }
    void FindScanner::NewPage(int)
	{ page++; }
    bool Match(PageView *pv, char *s, bool ic, bool mw);
};

void FindScanner::PutChar(char *s)
{
    Fontdesc *font;
    Font *f;
    int c;
    
    int l= strlen(s);
    if (StrCmp((byte*)cp, (byte*)s, l, map) == 0) {
	cp+= l;
	if (*cp == 0)
	    done= TRUE;
	font= fmap->Lookup(c, s, currfont);
	if (font) {
	    f= new_Font(font->Fid(), Scale(currsize*10), font->Face());
	    if (f)
		m.Merge(f->BBox(c, Scale(currpos)));
	}
    } else {
	m= gRect0;
	cp= str;
    }
}

bool FindScanner::Match(PageView *pv, char *s, bool matchCase, bool mw)
{
    bool rcode;
    
    map= matchCase ? identitymap : upcase;
    matchWord= mw;
    SafeDelete(str);
    cp= str= strsave(s);
    m= gRect0;
    if (rcode= Parse(-1)) {
	FP.rewind();
	page= 0;
    }
    pv->RevealSelection(page, m);
    return rcode;
}

//---- PrintScanner -------------------------------------------------------------

class PrintScanner : public Parser {
    Fontmap *fmap;
    int res;
public:
    PrintScanner(Fontmap *fm, istream *f);
    void NewPage(int)
	{ done= TRUE; }
    void DrawingFunction();
    void PutChar(char*);
    void DeviceControl();
};

PrintScanner::PrintScanner(Fontmap *fm, istream *f) : Parser(f)
{
    res= 10;
    fmap= fm;
}

void PrintScanner::PutChar(char *s)
{
    register Fontdesc *font;
    int c;
    
    if (font= fmap->Lookup(c, s, currfont))
	port->ShowChar(new_Font(font->Fid(), Scale(currsize*res), font->Face()),
						ePatBlack, Scale(currpos), c);
}

void PrintScanner::DrawingFunction()
{
    int nargs;
    char *p, buf[300];
    Point a[50];
    
    FP.get(buf, sizeof(buf));
    GrSetPenSize(Scale(currsize*res/12));
    for (p= buf+1, nargs= 0; *p; nargs++) { // collect arguments
	while (Isspace(*p))
	    p++;
	if (*p == '\0')
	    break;
	a[nargs/2][nargs%2]= atoi(p);
	while (!Isspace(*p))
	    p++;
    }
    
    switch (buf[0]) {
    case 'l':       // draw a line
	GrLine(Scale(currpos), Scale(currpos+a[0]));
	currpos+= a[0];
	break;
    case 'i':       // PostScript box
    case 'b':       // draw a box
	currpos.x+= a[0].x;
	break;
    case 'r':       // round corner box
	currpos.x+= a[0].x;
	break;
    case 'c':       // circle
	a[0].y= a[0].x;
    case 'e':       // ellipse
	GrStrokeOval(Rectangle(Scale(Point(currpos.x, currpos.y-a[0].y/2)), Scale(a[0])));
	currpos.x+= a[0].x;
	break;
    case 'a':       // arc
	currpos+= a[0]+a[1];
	break;
    case '~':       // spline
    case 'p':       // polygon
    case 'R':       // draw a rule
    default:
	break;
    }
}

void PrintScanner::DeviceControl()
{
    char str[100], str1[50], buf[300];
    int n;
    
    FP >> str;
    
    switch (str[0]) {
    case 'r':       // resolution assumed when prepared
	FP >> n;
	res= n / 72;
	break;
    case 'f':       // font used
	FP >> n >> str;
	//FP.get(buf, sizeof buf);   // in case there's a filename
	//FP.putback('\n');            // fgets goes too far 
	str1[0]= '\0';
	sscanf(buf, "%s", str1);
	fmap->Load(n, str, str1);
	break;
    default:
	break;
    }
    Parser::DeviceControl();
}

//---- TroffFind ---------------------------------------------------------------

MetaImpl(TroffFind, (I_O(ei1), I_O(doc), I_B(matchCase), I_B(matchWord), 0));

TroffFind::TroffFind(char *title, TroffDocument *d) : Dialog(title, eWinDefault)
{
    doc= d;
    doc->AddWindow(GetWindow());
    matchCase= matchWord= FALSE;
}

VObject *TroffFind::DoCreateDialog()
{
    //---- dialog parts ----
    VObject *Find=
	new BorderItem ("Find What", ei1= new EditTextItem(cIdFind, "", 300));

    VObject *Options=
	new BorderItem ("Options",
	    new ManyOfCluster(cIdFindOpt, eVObjHLeft, 5,
					"Match Case", "Match Whole Word", 0),
	    Point(6,4)
	);

    VObject *Actions=
	new Cluster (cIdNone, eVObjVBase, 10, 
	    new ActionButton (cIdDoFind, "Find Next", TRUE), 
	    0
	);

    //---- overall layout ----
    return
	new BorderItem(
	    new Cluster(cIdNone, VObjAlign(eVObjHLeft|eVObjHExpand), 10,
		Find,
		new Cluster (cIdNone, eVObjVCenter, 10,
		    Options,
		    0 
		),
		Actions,
		0
	    ),
	    10, 0
	);
}

void TroffFind::DoSetup()
{
    EnableItem(cIdDoFind, ei1->GetTextSize() > 0);
}

void TroffFind::Control(int id, int p, void *v)
{
    char *pattern;
    
    switch (id) {

    case cIdFind:
	Dialog::Control(id, p, v);
	if (p == cPartChangedText)
	    DoSetup();
	break;

    case cIdDoFind:
	pattern= ei1->GetText()->AsString();
	if (doc->view->Match(pattern, matchCase, matchWord))
	    ShowAlert(eAlertNote, "\"%s\" not found", pattern);
	break;

    case cIdFindOpt:
	switch(p) {
	case cIdFindOpt:
	    matchCase= !matchCase;
	    break;
	case cIdFindOpt+1:
	    matchWord= !matchWord;
	    break;
	}   
	break;

    default:
	Dialog::Control(id, p, v); 
	break;
    }
}

//---- Page --------------------------------------------------------------------

MetaImpl(Page, (TP(parser), T(pageno), T(pos), TP(pict), 0));

Page::Page(Parser *pa, long p, int n)
{
    parser= pa;
    pos= p;
    pageno= n;
    pict= 0;
}

Rectangle Page::SelRect(Rectangle r)
{
    //  r.origin+= GetOrigin()+20-GetPict()->bbox.origin;
    r.origin+= GetOrigin()+20;
    return r;
}

Picture *Page::Load()
{
    if (pict == 0) {
	GrShowWaitCursor wc;
	pict= new Picture;
	PictPort *pp= new PictPort(pict);
	Port *oldport= port;
	GrSetPort(pp);
	parser->Parse(pos);
	SafeDelete(pp);
	GrSetPort(oldport);
    }
    return pict;
}

Metric Page::GetMinSize()
{
    return Metric(1000, 1000);
    // return Metric(GetPict()->bbox.extent+40);
}

void Page::Draw(Rectangle)
{
    GrShowPicture(contentRect.Inset(20), GetPict());
}

//---- PageView ----------------------------------------------------------------

MetaImpl(PageView, (T(selection), TP(pages), T(current), TP(pg), 0));

PageView::PageView(TroffDocument *d, Fontmap *fm) : View(d, Point(100))
{
    current= -1;
    pages= 0;
    pg= 0;
    fmap= fm;
}

PageView::~PageView()
{
    SafeDelete(fparser);
}

void PageView::SetSelection(Rectangle s)
{
    if (selection.IsNotEmpty())
	InvalidateRect(selection);
    selection= s;
    if (selection.IsNotEmpty()) {
	RevealRect(selection.Expand(Point(5, 20)), selection.extent+20);
	InvalidateRect(selection);
	UpdateEvent();
    }
}

void PageView::Draw(Rectangle r)
{
    if (pg) {
	if (gColor && !gPrinting && selection.IsNotEmpty())
	    GrPaintRect(selection, gHighlightColor);
	pg->Draw(r);
	if (!gColor && !gPrinting && selection.IsNotEmpty())
	    GrInvertRect(selection);
    } else
	GrPaintRect(r, ePatGrey12);
}

bool PageView::Match(char *pattern, bool ic, bool mw)
{
    if (fparser)
	return fparser->Match(this, pattern, ic, mw);
    return FALSE;
}

void PageView::SetPages(Collection *p, char *n)
{
    pages= p;
    FirstPage();
    fparser= new FindScanner(fmap, new istream(n));
}

void PageView::SetPage(int page)
{
    page= range(0, GetPageCnt()-1, page);
    if ((page != current) && pages) {
	pg= (Page*) pages->At(current= page);
	if (pg) {
	    pg->SetContainer(this);
	    pg->CalcExtent();
	    pg->SetOrigin(gPoint0);
	    selection= gRect0;
	    SetExtent(pg->GetMinSize().extent);
	    ForceRedraw();
	}
    }
}

void PageView::RevealSelection(int page, Rectangle r)
{
    if (--page >= 0 && pages) {
	SetPage(page);
	if (pg)
	    SetSelection(pg->SelRect(r));
    }
}

void PageView::DoCreateMenu(Menu *mp)
{
    View::DoCreateMenu(mp);
    mp->AppendItems("-",
		    "next", cNEXT,
		    "prev", cPREV,
		    0);
}

void PageView::DoSetupMenu(Menu *mp)
{
    View::DoSetupMenu(mp);
    if (GetPage() < GetPageCnt()-1)
	mp->EnableItem(cNEXT);
    if (GetPage() > 0)
	mp->EnableItem(cPREV);
}

Command *PageView::DoMenuCommand(int cmd)
{
    switch (cmd) {
    case cPREV: PrevPage(); break;
    case cNEXT: NextPage(); break;
    case cCUT:
    case cCOPY: gClipBoard->SetType((char*) cDocTypeET);
    default:    return View::DoMenuCommand(cmd);
    }
    return gNoChanges;
}

Command *PageView::DoFunctionKeyCommand(int code, Point p, Token t)
{
    switch (t.FunctionCode()) {
    case 28:    LastPage();     break;
    case 22:    FirstPage();    break;
    case 24:    PrevPage();     break;
    case 30:    NextPage();     break;
    default:    return View::DoFunctionKeyCommand(code, p, t);
    }
    return gNoChanges;
}

bool PageView::HasSelection()
{
    return pg != 0;
}

void PageView::SelectionToClipboard(char *type, ostream &os)
{
    if (strcmp(type, cDocTypeET) == 0)
	os << pg->GetPict() NL;
}

//---- TroffDocument -----------------------------------------------------------

MetaImpl(TroffDocument, (TP(view), TP(findChange), 0));

TroffDocument::TroffDocument() : Document(cDocTypeDitroff)
{
    fmap= new Fontmap;
    pages= new OrdCollection;
}

TroffDocument::~TroffDocument()
{
    SafeDelete(view);
    SafeDelete(findChange);
    SafeDelete(fmap);
    SafeDelete(pages);
}

Window *TroffDocument::DoMakeWindows()
{            
    view= new PageView(this, fmap);
    return new Window(this, Point(600, 850), eWinDefault, new Scroller(view));
}

void TroffDocument::DoRead(istream&, FileType *ft)
{
    fp= new istream(ft->FileName());
    parser= new PrintScanner(fmap, fp);
    Parser *pp= new PageScanner(pages, parser, fp);
    pp->Parse();
    SafeDelete(pp);
    parser->Parse();
    view->SetPages(pages, ft->FileName());
}

void TroffDocument::DoWrite(ostream &s, int)
{
    Parser *pp= new TextScanner(fp, &s);
    pp->Parse();
    SafeDelete(pp);
}

void TroffDocument::DoCreateMenu(Menu *mp)
{
    Document::DoCreateMenu(mp);
    mp->InsertItemsAfter(cLASTEDIT, "find...", cFIND, 0);
}

void TroffDocument::DoSetupMenu(Menu *mp)
{
    Document::DoSetupMenu(mp);
    if (parser)
	mp->EnableItem(cFIND);
}

Command *TroffDocument::DoMenuCommand(int cmd)
{
    if (cmd == cFIND) {
	if (findChange == 0)
	    findChange= new TroffFind("Find", this);
	findChange->ShowOnWindow(GetWindow());
	return gNoChanges;
    }
    return Document::DoMenuCommand(cmd);
}

//---- trofftool ---------------------------------------------------------------

class trofftool: public Application {
    TroffFontManager *tfm;
public:
    MetaDef(trofftool);
    trofftool(int argc, char **argv) : Application(argc, argv, cDocTypeDitroff)
	{ ApplInit(); ::tfm= tfm= new TroffFontManager("sun"); }
    Document *DoMakeDocuments(const char *)
	{ return new TroffDocument; }
};

MetaImpl(trofftool, (TP(tfm), 0));

//---- main --------------------------------------------------------------------

main(int argc, char **argv)
{
    trofftool trofftool(argc, argv);
    
    return trofftool.Run();
}
