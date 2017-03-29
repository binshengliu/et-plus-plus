//$Button,ButtonCommand,ActionButton,OnOffItem,ImageButton,RadioButton$
//$ToggleButton,LabeledButton$

#include "Buttons.h"
#include "Dialog.h"
#include "WindowSystem.h"

//---- Button ------------------------------------------------------------------

AbstractMetaImpl0(Button);

Button::Button(int id, VObject *gop) : CompositeVObject(id, gop, 0)
{
}

Command *Button::DoLeftButtonDownCommand(Point, Token, int)
{
    return new ButtonCommand(this, contentRect, TestFlag(eButtonIdle));
}

void Button::Flush(int msec)
{
    if (Enabled()) {
	Highlight(On);
	GraphicDelay(msec);
	Highlight(Off);
	DoOnItem(1, 0, gPoint0);
    }
}

void Button::DoOnItem(int m, VObject*, Point)
{
    if (m != 3)
	Control(GetId(), cPartToggle, (void*) 1);
}

//---- ButtonCommand -----------------------------------------------------------

ButtonCommand::ButtonCommand(Button* bf, Rectangle r, bool b)
{
    item= bf;
    inside= TRUE;
    lastinside= FALSE;
    rect= r;
    SetFlag(eCmdIdleEvents, b);
}

void ButtonCommand::TrackFeedback(Point, Point, bool)
{
    if (inside != lastinside) {
	item->Highlight(inside ? On : Off);
	lastinside= inside;
    }
}

Command *ButtonCommand::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    inside= rect.ContainsPoint(np);
    switch(atp) {
    case eTrackPress:
	if (TestFlag(eCmdIdleEvents))
	    item->DoOnItem(1, 0, np);
	else
	    item->DoOnItem(3, 0, np);
	break;        
    case eTrackIdle:
	if (lastinside && TestFlag(eCmdIdleEvents))
	    item->DoOnItem(2, 0, np);
	break;
    case eTrackExit:
    case eTrackRelease:
	if (lastinside) {
	    item->ForceRedraw();
	    if (! TestFlag(eCmdIdleEvents))
		item->DoOnItem(0, 0, np);
	}
	return gNoChanges;
    case eTrackMove:
	break;
    }
    return this;
}

//---- ImageButton -------------------------------------------------------------

MetaImpl(ImageButton, (TB(idleEvents), TP(bm1), TP(bm2), 0));

ImageButton::ImageButton(int id, Bitmap *b1, Bitmap *b2, bool idle) : Button(id)
{
    bm1= b1;
    bm2= b2;
    idleEvents= idle;
    SetFlag(eVObjHFixed | eVObjVFixed);
}

ImageButton::~ImageButton()
{
}

Metric ImageButton::GetMinSize()
{
    return Metric(Max(bm1->Size(), bm2->Size()));
}

void ImageButton::DrawInner(Rectangle, bool highlight)
{
    GrPaintBitMap(contentRect, highlight ? bm2 : bm1,
			       Enabled() ? ePatBlack : ePatGrey50);
}

Command *ImageButton::DoLeftButtonDownCommand(Point , Token, int)
{
    return new ButtonCommand(this, contentRect, idleEvents);
}

ostream& ImageButton::PrintOn(ostream &s)
{
    Button::PrintOn(s);
    return s << idleEvents SP << bm1 SP << bm2 SP;
}

istream& ImageButton::ReadFrom(istream &s)
{
    Button::ReadFrom(s);
    return s >> Bool(idleEvents) >> bm1 >> bm2;
}

//---- ActionButton ------------------------------------------------------------

MetaImpl0(ActionButton);

ActionButton::ActionButton(int id, VObject *g, bool dflt) : Button(id, g)
{
    SetFlag(eActionDefaultButton, dflt);
    SetFlag(eVObjVFixed);
}

ActionButton::ActionButton(int id, char *t, bool dflt) : Button(id, new TextItem(t))
{
    SetFlag(eActionDefaultButton, dflt);
    SetFlag(eVObjVFixed);
}

Metric ActionButton::GetMinSize()
{
    Metric m= Button::GetMinSize().Expand(Point(CalcBorder()));
    m.extent.x= max(m.extent.x, (m.extent.y*3)/2);
    return m;
}

void ActionButton::SetOrigin(Point at)
{
    VObject *inner= At(0);

    VObject::SetOrigin(at);
    at.y+= CalcBorder();
    at.x+= (Width() - inner->Width())/2;
    inner->SetOrigin(at);
}

void ActionButton::SetExtent(Point e)
{
    VObject::SetExtent(e);
    At(0)->CalcExtent();
}

int ActionButton::Base()
{
    return At(0)->Base()+CalcBorder();
}

void ActionButton::DrawHighlight(Rectangle)
{
    GrFillRoundRect(contentRect.Inset(CalcBorder()-gPoint2), 14);
}

void ActionButton::Draw(Rectangle r)
{
    GrSetPenNormal();
    if (! Enabled())
	GrSetPenPattern(ePatGrey50);
    GrStrokeRoundRect(contentRect.Inset(CalcBorder()-gPoint2), 14);

    if (TestFlag(eActionDefaultButton)) {
	GrSetPenSize(2);
	GrStrokeRoundRect(contentRect, 18);
    }
    Button::Draw(r);
}

void ActionButton::Control(int id, int part, void *val)
{
    if (part == cPartToggle)
	part= cPartAction;
    Button::Control(id, part, val);
}

void ActionButton::SetContainer(VObject *vop)
{
    Button::SetContainer(vop);
    if (TestFlag(eActionDefaultButton)) {
	DialogView *vp= (DialogView*) FindContainerOfClass(Meta(DialogView));
	if (vp)
	    vp->SetDefaultButton(this);
    }
}

//---- OnOffItem ---------------------------------------------------------------

AbstractMetaImpl(OnOffItem, (TB(state), 0));

OnOffItem::OnOffItem(int id, Bitmap *b1, Bitmap *b2, Bitmap *b3, bool s) : Button(id)
{
    on= b1;
    off= b2;
    highlight= b3;
    state= s;
}

void OnOffItem::DoOnItem(int m, VObject*, Point)
{
    if (m != 3) {
	SendDown(GetId(), cPartSetState, (void*) !state);
	Control(GetId(), cPartToggle, (void*) state);
	Changed();
    }
}

Metric OnOffItem::GetMinSize()
{
    return Metric(gPoint16, 14);
}

int OnOffItem::Base()
{
    return (contentRect.extent.y/8)*7;
}

void OnOffItem::Draw(Rectangle)
{
    GrPaintBitMap(contentRect, state ? on : off, Enabled() ? ePatBlack : ePatGrey50);
}

void OnOffItem::SetState(bool s, bool redraw)
{
    if (s != state) {
	state= s;
	Changed();
	if (redraw)
	    ForceRedraw();
    }
}

void OnOffItem::SendDown(int, int part, void *val)
{   
    switch (part) {
    case cPartStateIsOn:
	bool *b= (bool*)val;
	if (state) 
	    *b= state;
	break;
    case cPartSetState:
	SetState((bool) val);
	break;
    case cPartIncr:
	SetState(TRUE);
	break;
    case cPartDecr:
	SetState(FALSE);
	break;
    }
}

ostream& OnOffItem::PrintOn(ostream &s)
{
    Button::PrintOn(s);
    return s << state SP << on SP << off SP << highlight SP;
}

istream& OnOffItem::ReadFrom(istream &s)
{
    Button::ReadFrom(s);
    return s >> Bool(state) >> on >> off >> highlight;
}

//---- RadioButton -------------------------------------------------------------

static u_short RadioButtonOnBits[]= {
#   include "images/RadioButtonOn.image"
};

static u_short RadioButtonOffBits[]= {
#   include "images/RadioButtonOff.image"
};

static u_short RadioButtonFeedbackBits[]= {
#   include "images/RadioButtonFeedback.image"
};

static Bitmap *radioButtonOn, *radioButtonOff, *radioButtonFeedback;

MetaImpl0(RadioButton);

RadioButton::RadioButton(int id, bool state)
	: OnOffItem(id,
	    radioButtonOn ? radioButtonOn
			  : radioButtonOn= new Bitmap(16, RadioButtonOnBits),
	    radioButtonOff ? radioButtonOff
			  : radioButtonOff= new Bitmap(16, RadioButtonOffBits),
	    radioButtonFeedback ? radioButtonFeedback
			  : radioButtonFeedback= new Bitmap(16, RadioButtonFeedbackBits),
	    state)
{
}

void RadioButton::InitNew()
{
    on= radioButtonOn ? radioButtonOn
		      : radioButtonOn= new Bitmap(16, RadioButtonOnBits);
    off= radioButtonOff ? radioButtonOff
			: radioButtonOff= new Bitmap(16, RadioButtonOffBits);
    highlight= radioButtonFeedback ? radioButtonFeedback
		: radioButtonFeedback= new Bitmap(16, RadioButtonFeedbackBits);
}

ostream& RadioButton::PrintOn(ostream &s)
{
    Button::PrintOn(s);
    return s SP << state SP;
}

istream& RadioButton::ReadFrom(istream &s)
{
    Button::ReadFrom(s);
    return s >> Bool(state);
}

//---- ToggleButton ------------------------------------------------------------

static u_short CheckMarkOnBits[]= {
#   include "images/CheckmarkOn.image"
};

static u_short CheckMarkOffBits[]= {
#   include "images/CheckmarkOff.image"
};

static u_short CheckMarkFeedbackBits[]= {
#   include "images/CheckmarkInv.image"
};

static Bitmap *CheckMarkOn, *CheckMarkOff, *CheckMarkFeedback;

MetaImpl0(ToggleButton);

ToggleButton::ToggleButton(int id, bool state)
		: OnOffItem(id,
		    CheckMarkOn ? CheckMarkOn
				: CheckMarkOn= new Bitmap(16, CheckMarkOnBits),
		    CheckMarkOff ? CheckMarkOff
				 : CheckMarkOff= new Bitmap(16, CheckMarkOffBits),
		    CheckMarkFeedback ? CheckMarkFeedback
				      : CheckMarkFeedback= new Bitmap(16, CheckMarkFeedbackBits),
		    state)
{
}

void ToggleButton::InitNew()
{
    on= CheckMarkOn ? CheckMarkOn : CheckMarkOn= new Bitmap(16, CheckMarkOnBits);
    off= CheckMarkOff ? CheckMarkOff : CheckMarkOff= new Bitmap(16, CheckMarkOffBits);
    highlight= CheckMarkFeedback ? CheckMarkFeedback : CheckMarkFeedback= new Bitmap(16, CheckMarkFeedbackBits);
}

ostream& ToggleButton::PrintOn(ostream &s)
{
    Button::PrintOn(s);
    return s << state SP;
}

istream& ToggleButton::ReadFrom(istream &s)
{
    Button::ReadFrom(s);
    return s >> Bool(state);
}

//---- LabeledButton -----------------------------------------------------------

MetaImpl0(LabeledButton);

LabeledButton::LabeledButton(int id, OnOffItem *b, VObject *g, Point gap,
					VObjAlign) : VObjectPair(b, g, gap)
{
    SetId(id);
}

LabeledButton::LabeledButton(int id, char *t, bool w)
	       : VObjectPair(w ? (VObject*) new RadioButton
			       : (VObject*) new ToggleButton,
			     new TextItem(t),
			     gPoint10)
{
    SetId(id);
}

Command *LabeledButton::DoLeftButtonDownCommand(Point, Token, int)
{
    if (left->Enabled())
	return new ButtonCommand((Button*)left, contentRect);
    return gNoChanges;
}

void LabeledButton::Control(int, int part, void *val)
{
    VObjectPair::Control(GetId(), part, val);
}
