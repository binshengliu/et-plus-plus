//$CalcButton,CalcDialogView,CalcDocument,myApplication$
#include "ET++.h"
#include "Dialog.h"
#include "DialogItems.h"
#include "Math.h"

#ifdef __GNUG__
extern "C" double sqrt();
#else
#include <math.h>
#endif

#ifndef MAXFLOAT
#define MAXFLOAT 9.9999999e20
#endif MAXFLOAT

const int cOp = cIdFirstUser + 100;

class CalcButton : public ActionButton {
public:
    CalcButton(char *s, int c= 0) : ActionButton(cOp+ (c ? c : *s), s)
	{ }
    void Control(int id, int, void *v)
	{ ActionButton::Control(cOp, id-cOp, v); }
    Command *DoLeftButtonDownCommand(Point , Token, int)
	{ return new ButtonCommand(this, contentRect, TRUE); }
};

//---- CalcDialogView ---------------------------------------------------------

class CalcDialogView: public DialogView {
    int lastop;
    bool evaluated;
    FloatItem *indicator;
    double operand1;
    
public:
    MetaDef(CalcDialogView);
    CalcDialogView(Document *dp) : DialogView(dp)
	{ UpdateVal('=', 0.0); }
    VObject *DoCreateDialog();
    void Control(int id, int p, void *v);
    Command *DispatchEvents(Point lp, Token t, Clipper *vf);
    void UpdateVal(char c, double d);
};

MetaImpl (CalcDialogView, (TP(indicator), T(operand1), 0));

VObject *CalcDialogView::DoCreateDialog()
{
    return new BorderItem (
	new Cluster (1, eVObjHLeft, 15,
	    new BorderItem(
		indicator= new FloatItem(555, 0.0, -MAXFLOAT, MAXFLOAT, 16),
		Point(5,0), 2
	    ),
	    new Cluster(cOp, (VObjAlign)(eVObjHLeft|eVObjVTop), Point(10,10),
		new CalcButton("C", 'c'),
		new CalcButton("7"),
		new CalcButton("4"),
		new CalcButton("1"),
		new CalcButton("0"),
		
		new CalcButton("\261", 's'),
		new CalcButton("8"),
		new CalcButton("5"),
		new CalcButton("2"),
		new CalcButton("."),
		
		new CalcButton("\303", 'r'),
		new CalcButton("9"),
		new CalcButton("6"),
		new CalcButton("3"),
		new CalcButton("="),
		
		new CalcButton("Off", 'q'),
		new CalcButton("/"),
		new CalcButton("*"),
		new CalcButton("-"),
		new CalcButton("+"),
		0
	    ),
	    0
	),
	Point(5)
    );
 }
 
Command *CalcDialogView::DispatchEvents(Point lp, Token t, Clipper *vf)
{    
    if (t.IsKey()) {
	VObject *vop= FindItem(cOp+t.Code);
	if (vop && vop->IsKindOf(Button)) {
	    ((Button*)vop)->Flush(50);
	    return gNoChanges;
	}
    }
    return DialogView::DispatchEvents(lp, t, vf);
}

void CalcDialogView::UpdateVal(char c, double d)
{
    operand1= d;
    if (indicator)
	indicator->SetValue(d);
    lastop= c;
    evaluated= TRUE;
}

void CalcDialogView::Control(int id, int p, void *v)
{
    Token t;
    
    if (id != cOp)
	DialogView::Control(id, p, v);
	
    switch (p) {
    
    case 'q':
	DoMenuCommand(cCLOSE);
	break;
	
    case 'c':
	UpdateVal('=', 0.0);
	break;
	
    case '1': case '2': case '3': case '4': case '5': 
    case '6': case '7': case '8': case '9': case '0': 
    case '.':
	if (evaluated) {
	    indicator->SetString((byte*)"");
	    evaluated= FALSE;
	}
	t.Code= p;
	DialogView::DispatchEvents(gPoint0, t, 0);
	break;
	
    case '+': case '-': case '*': case '/': 
    case '=': case 's': case 'r':
	switch (lastop) {
	case '=':
	    UpdateVal(p, indicator->GetValue());
	    break;
	case '+':
	    UpdateVal(p, operand1+indicator->GetValue());
	    break;
	case '-':
	    UpdateVal(p, operand1-indicator->GetValue());
	    break;
	case '*':
	    UpdateVal(p, operand1*indicator->GetValue());
	    break;
	case '/':
	    UpdateVal(p, operand1/indicator->GetValue());
	    break;
	}
	if (p == 's')
	    UpdateVal('=', -operand1);
	if (p == 'r')
	    UpdateVal('=', sqrt(operand1));
	break;
    }
}

//---- CalcDocument --------------------------------------------------------------

class CalcDocument : public Document {
    View *view;
    int cnt;
public:
    MetaDef(CalcDocument);
    CalcDocument(int c)
	{ cnt= c; }
    ~CalcDocument()
	{ SafeDelete(view); }
    Window *DoMakeWindows(); 
};

MetaImpl(CalcDocument, (TP(view), T(cnt)));

Window *CalcDocument::DoMakeWindows()
{
    return new Window(this, Point(20), (WindowFlags)(eWinDefault+eBWinFixed),
	new Clipper(view= new CalcDialogView(this)),
	form("calculator-%d", cnt)
    );
}

//---- myApplication ------------------------------------------------------------

class myApplication: public Application {
    int cnt;
public:
    MetaDef(myApplication);
    myApplication(int argc, char **argv) : Application(argc, argv)
	{ ApplInit(); cnt= 0; }
    Document *DoMakeDocuments(const char*)
	{ return new CalcDocument(cnt++); }
};

MetaImpl(myApplication, (T(cnt), 0));

//---- main ---------------------------------------------------------------------

main(int argc, char **argv)
{
    myApplication myAppl(argc, argv);
    
    return myAppl.Run();
}
