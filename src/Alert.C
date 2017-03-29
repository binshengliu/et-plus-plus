#include "Alert.h"
#include "Buttons.h"
#include "BorderItems.h"
#include "CheapText.h"
#include "StaticTView.h"
#include "StyledText.h"
#include "ObjList.h"
#include "WindowSystem.h"
#include "BlankWin.h"
#include "ObjectTable.h"

static u_short NoteBits[]= {
#   include "images/note.image"
};

static u_short CautionBits[]= {
#   include "images/caution.image"
};

static u_short StopBits[]= {
#   include "images/stop.image"
};

static u_short SunBits[]= {
#   include "images/sun.image"
};

static u_short ErrorBits[]= {
#   include "images/error.image"
};

static Alert *noteAlert,
	     *cautionAlert,
	     *stopAlert,
	     *messageAlert,
	     *sunAlert,
	     *errorAlert;
			    
MetaImpl(Alert, (TP(text), TP(image), TP(buttons), 0));

Alert::Alert(AlertType at, byte *message, Bitmap *va_(bm), ...)
					    : Dialog(0, eBWinOverlay+eBWinBlock)
{
    va_list ap;
    ObjList *ol;

    if (message == 0)
	return;

    text= new StaticTextView((View*)0, Rectangle(gSysFont->Width('n')*40,cFit),
						new CheapText(message));

    if (va_(bm))
	image= new ImageItem(va_(bm));

    if (at == eAlertMessage)
	image= text;
    else
	image= new Cluster(2, eVObjVTop, 25, image, text, 0);

    ol= new ObjList;
    buttons= new Cluster(2, eVObjVBase, 20, ol);
 
    va_start(ap,va_(bm));
    char *s;
    for (int i= 0; s= va_arg(ap, char*); i++)
	ol->Add(new ActionButton(va_arg(ap, int), s, i == 0));   
    va_end(ap);
}

Alert::~Alert()
{
    if (text) {
	Text *t= ((StaticTextView*)text)->GetText();
	SafeDelete(t);
	SafeDelete(text);
	this= 0;    // hack
    }
}

VObject *Alert::DoCreateDialog()
{
    return 
	new BorderItem(
	    new BorderItem(
		new Cluster(2, eVObjHLeft, gPoint10, image, buttons, 0),
		Point(15),
		3
	    ),
	    gPoint2,
	    1
	);
}

int Alert::Show(char* va_(fmt), ...)
{
    int code;
    va_list ap;
    
    va_start(ap,va_(fmt));
    code= ShowV(va_(fmt), ap);
    va_end(ap);
    return code;
}

int Alert::ShowV(char *fmt, va_list ap)
{
    int ret;

    if (image->IsKindOf(Cluster))
	((Cluster*)image)->SetModified();
    char *buf= strvprintf(fmt, ap);
    if (gWinInit) {
	Text *tmpp= ((StaticTextView*)text)->SetText(new StyledText(gSysFont, buf));
	delete tmpp;
	CalcLayout(FALSE);
	ret= Dialog::Show();
    } else {
	cerr << buf NL;
	ret= cIdNo;
    }    
    SafeDelete(buf);
    return ret;
}

class Menu *Alert::GetMenu()
{
    return 0;
}

void Alert::InspectorId(char *buf, int sz)
{
    if (text)
	text->InspectorId(buf, sz);
    else
	Dialog::InspectorId(buf, sz);   
}

bool TestInterrupt(char *what)
{
    if (Interrupted())
	return ShowAlert(eAlertStop, "Abort %s?", what) == cIdYes;
    return FALSE;
}

int ShowAlert(AlertType at, char* va_(fmt), ...)
{
    va_list ap;
    Alert *al= 0;
    Object *root= 0;
    
    va_start(ap, va_(fmt));
    
    switch (at) {
    case eAlertNote:
	if (noteAlert == 0)
	    root= noteAlert= new Alert(at, (byte*) "note",
			new Bitmap(Point(64, 52), NoteBits), "OK", cIdOk, 0);
	al= noteAlert;
	break;
    case eAlertCaution:
	if (cautionAlert == 0) {
	    root= cautionAlert= new Alert(at, (byte*) "Caution", 
			new Bitmap(Point(64, 52), CautionBits),
					"Yes",    cIdYes,
					"No",     cIdNo,
					"Cancel", cIdCancel, 0);
	}
	al= cautionAlert;
	break;
    case eAlertStop:
	if (stopAlert == 0)
	    root= stopAlert= new Alert(at, (byte*) "Stop",
			new Bitmap(64, StopBits),
				     "Yes",    cIdYes,
				     "No",     cIdNo,
				     "Cancel", cIdCancel, 0);
	al= stopAlert;
	break;
    case eAlertSun:
	if (sunAlert == 0)
	    root= sunAlert= new Alert(at, (byte*) "Message",
			new Bitmap(64, SunBits), "Ok", cIdOk, 0);
	al= sunAlert;
	break;
    case eAlertError:
	if (errorAlert == 0)
	    root= errorAlert= new Alert(at, (byte*) "Error",
			new Bitmap(64, ErrorBits),
				      "Ignore",     cIdIgnore,
				      "Dump Core",  cIdAbort,
				      "Inspect",    cIdInspect, 0);
	al= errorAlert;
	break;
    case eAlertMessage:
	if (messageAlert == 0)
	    root= messageAlert= new Alert(at, (byte*) "Message", 0, "Ok", cIdOk, 0);
	al= messageAlert;
	break;
    }
    if (root)
	ObjectTable::AddRoot(root);
    int code= al->ShowV(va_(fmt), ap);
    va_end(ap);
    return code;
}
