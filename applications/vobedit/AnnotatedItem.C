//$AnnotatedItem,EditMessageSheet$
#include "AnnotatedItem.h"
#include "DialogItems.h"
#include "Dialog.h"
#include "Scroller.h"
#include "Window.h"
#include "TextView.h"
#include "VObjectText.h"
#include "VObjectTView.h"

//---- class EditMessageSheet -----------------------------------

class EditMessageSheet: public Dialog {
    AnnotatedItem *mt;
    TextView *tv;
public: 
    MetaDef(EditMessageSheet);
    
    EditMessageSheet(AnnotatedItem *t);
    ~EditMessageSheet();
    void Control(int id, int part, void *v);
    VObject *DoCreateDialog();
};

MetaImpl(EditMessageSheet, (TP(mt), TP(tv), 0));

EditMessageSheet::EditMessageSheet(AnnotatedItem *t) : Dialog((char*)0, eBWinBlock)
{ 
    mt= t; 
}
	    
EditMessageSheet::~EditMessageSheet()
{ 
    SafeDelete(tv); 
}

void EditMessageSheet::Control(int id, int part, void* v)
{
    if (part == cPartAction && id == cIdOk) 
	mt->SetMessage((VObjectText*)tv->GetText());
    Dialog::Control(id,part,v);
}

VObject *EditMessageSheet::DoCreateDialog()
{
    VObject *vop;
    vop= new BorderItem(
	new BorderItem(
	    new Cluster(2, eVObjHLeft, 10,
		new Scroller(
		    tv= new VObjectTextView(this, Rectangle(300,cFit), 
					    mt->GetMessage()), Point(300, 150)
		),
		new ActionButton (cIdOk, "Ok"), 
		0
	    ),
	    Point(10,10),
	    3
	),
	gPoint2,
	1
    );
    tv->SetFlag(eTextNoFind);
    return vop;
}

//---- AnnotatedItem -----------------------------------------------------------

static u_short AnnotatedItemBits[]= {
#   include "images/AnnotatedItem.im"
};

static u_short AnnotatedInvBits[]= {
#   include "images/AnnotatedItemFeedback.im"
};

static Bitmap *AnnotatedItemImage, *AnnotatedItemInv;

MetaImpl(AnnotatedItem, (TP(msg), 0));
  
AnnotatedItem::AnnotatedItem(char *message)
	: ImageButton(0,
		AnnotatedItemImage ? AnnotatedItemImage : AnnotatedItemImage= new Bitmap(Point(20,16), AnnotatedItemBits),
		AnnotatedItemInv ? AnnotatedItemInv : AnnotatedItemInv= new Bitmap(Point(20,16), AnnotatedInvBits),
		FALSE)
{
    Init(message);
}

AnnotatedItem::AnnotatedItem()
{
    Init(0);
}
    
void AnnotatedItem::Init(char *m)
{
    Enable(TRUE); 
    if (m)
	msg= new VObjectText((byte*)m);
    else
	msg= new VObjectText;
}

AnnotatedItem::~AnnotatedItem()
{
    SafeDelete (msg);
}

void AnnotatedItem::SetMessage(VObjectText *m)
{
    msg= m;
} 

VObjectText *AnnotatedItem::GetMessage()
{ 
    return msg; 
}

void AnnotatedItem::DoOnItem(int what, VObject*, Point)
{
    if (! msg)
	return;
    if (what == 0) {
	EditMessageSheet *em= new EditMessageSheet(this);
	em->Show();
	//SafeDelete(em);
    }
}

GrCursor AnnotatedItem::GetCursor(Point)
{
    return eCrsHand;
}

ostream& AnnotatedItem::PrintOn (ostream&s)
{
    ImageButton::PrintOn(s);
    return s << msg;
}

istream& AnnotatedItem::ReadFrom(istream &s)
{
    ImageButton::ReadFrom(s);
    return s >> msg;
}

