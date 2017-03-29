//$TextShapeSketcher,TextShapeStretcher,TextShape$
#include "TextShape.h"
#include "BoxShape.h"
#include "DrawView.h"
#include "StyledText.h"
#include "VObjectText.h"
#include "VObjectTView.h"
#include "ObjInt.h"

const int cMinParagraph = 30;       // minimum size of a p

//---- TextShape ---------------------------------------------------------------

static short TextImage[]= {
#   include  "images/TextShape.im"
};

MetaImpl(TextShape, (TP(to), TP(attachedTo), T(captured), 0));

TextShape::TextShape()
{
    attachedTo= 0;
    to= 0;
}

TextShape::~TextShape()
{
    if (attachedTo)
	attachedTo->RemoveObserver(this);
    SafeDelete(to);
}

short *TextShape::GetImage()
{
    return TextImage;
}

bool TextShape::IsGarbage()
{   
    if (GetView()->GetActiveText() == this)
	return FALSE;
    return (bool) (Shape::IsGarbage() || to->Empty());
}

GrCursor TextShape::SketchCursor()
{
    return eCrsIBeam;
}

void TextShape::Init(Point p1, Point p2)
{ 
    Shape::Init(p1, p2);
    ink= 0;
    captured= (bool) (bbox.Width() <= cMinParagraph);
    isEnabled= (bool) !captured;
    int width= captured ? cFit : bbox.extent.x;
    to= new VObjectTextView(0, Rectangle(bbox.origin, Point(width,cFit)),
	    new VObjectText(16, new_Font(eFontHelvetica, 14)),
	    eLeft, eOne, TRUE, eTextViewDefault, gPoint0);
    bbox.extent.y= to->Height();
    to->AddObserver(this);      // register myself as dependent of the TextView
}

void TextShape::Draw(Rectangle r)
{
    if (to) {
	GrFillRect(bbox);
	GrSetTextPattern(InkPalette[penink]);
	if (GetView()->GetActiveText() != this && to->AnySelection())
	    to->SetNoSelection();
	to->Draw(r);
    }
}

void TextShape::Outline(Point p1, Point p2)
{
    int bh;
    Shape::Outline(p1, p2);
    Rectangle r= NormRect(p1, p2);
    GrLine(r.NW(), r.SW());
    GrLine(r.NE(), r.SE());
    if (to)
	bh= to->BaseHeight(1);
    else
	bh= TextShapeDefaultFont()->Ascender(); 
    GrLine(r.NW()+Point(1,bh), r.NE()+Point(-1,bh));
}

void TextShape::SetFont(RunArray *newfont)
{
    StyledText *st= (StyledText*) to->GetText();
    st->ReplaceStyles(newfont, 0, st->Size());
    to->Reformat();
    Changed();
}

void TextShape::ApplyFont(StChangeStyle st, StyleSpec sp)
{
    StyledText *text= (StyledText*) to->GetText();
    text->SetStyle(st, 0, text->Size(), sp);    
    to->Reformat();
    Changed();
}

int TextShape::GetSimpleProperty(ShapeProperties what)
{
    switch (what) {
    case eShapeAdjust:
	return int(to->GetJust());
    case eShapeSpacing:
	return int(to->GetSpacing());
    default:
	return Shape::GetSimpleProperty(what);
    }
}

Object *TextShape::GetProperty(ShapeProperties what)
{
    StyledText *st= (StyledText*) to->GetText();
    RunArray *ra;
    
    switch (what) {
    case eShapeTextFont:
    case eShapeTextSize:
    case eShapeTextFace:
    case eShapePenPattern:
	ra= new RunArray;
	st->CopyStyles(ra, 0, st->Size());
	return ra;
    default:
	return Shape::GetProperty(what);
    }
}

void TextShape::SetProperty(ShapeProperties what, Object *op)
{
    Ink *ip;
    int p;
    switch (what) {
    case eShapeTextFont:
	p= Guard(op, ObjInt)->GetValue();
	ApplyFont(eStFont, StyleSpec((GrFont) p, eFacePlain, 0, ePatNone));
	break;
    case eShapeTextSize:
	p= Guard(op, ObjInt)->GetValue();
	ApplyFont(eStSize, StyleSpec(eFontTimes, eFacePlain, p, ePatNone));
	break;
    case eShapeTextFace:
	p= Guard(op, ObjInt)->GetValue();
	ApplyFont(eStFace, StyleSpec(eFontTimes, (GrFace)p, eFacePlain, ePatNone));
	break;
    case eShapePenPattern:
	ip= Guard(op->Clone(), Ink);
	ApplyFont(eStInk, StyleSpec(eFontTimes, eFacePlain, 0, ip));
	break;
    default:
	Shape::SetProperty(what, op);
	return;
    }
    Changed();
}

void TextShape::SetSimpleProperty(ShapeProperties what, int p)
{
    switch (what) {
    case eShapeAdjust:
	to->SetJust(eTextJust(p));
	break;
    case eShapeSpacing:
	to->SetSpacing(eSpacing(p));
	break;
    default:
	Shape::SetSimpleProperty(what, p);
	return;
    }
    Changed();
}

void TextShape::ResetProperty(ShapeProperties what, Object *op)
{
    switch (what) {
    case eShapeTextFont:
    case eShapeTextSize:
    case eShapePenPattern:
    case eShapeTextFace:
	SetFont((RunArray*)op);
	break;
    default:
	Shape::ResetProperty(what, op);
    }
}

Rectangle TextShape::InvalRect()
{
    return Shape::InvalRect().Expand(2);
}

void TextShape::Moveby(Point delta)
{
    Shape::Moveby(delta);
    to->SetOrigin(bbox.origin);
}

void TextShape::InvalDiff(Rectangle r2)
{
    if (GetContainer()) {
	Rectangle r[4];
	int n= Difference(r, r2, bbox);
	
	for (int i= 0; i < n; i++)
	    GetContainer()->InvalidateRect(r[i]);
    }
}

void TextShape::Highlight(HighlightState h)
{
    if (GetView()) {
	// if there is no active Text highlight the object as a shape
	if (GetView()->GetActiveText() != this)
	    Shape::Highlight(h);
	else if (to && !to->InTextSelector()) // ???
	    to->ForceRedraw();
    }
}

void TextShape::SetSpan(Rectangle r)
{
    if (! captured) {
	bbox= NormRect(r.origin, r.extent);
	to->SetExtent(Point(bbox.extent.x, cFit));
	to->SetOrigin(bbox.origin);
	Changed();
    }
}

void TextShape::MakeDependentOn(Shape *s)
{
    s->AddObserver(this);
    attachedTo= s;    
}

void TextShape::DoObserve(int, int part, void*, Object *op)
{
    if (to == 0)
	return;
    if (op == to) { // text obj changed 
	switch (part) {
	case cPartExtentChanged:
	    if (ink != 0)
		InvalDiff(Rectangle(bbox.origin, to->GetExtent()));
	    bbox.extent= to->GetExtent();
	    break;
	    
	case cPartOriginChanged:
	    if (ink != 0)
		InvalDiff(Rectangle(bbox.origin, to->GetOrigin()));
	    bbox.origin= to->GetOrigin();
	    break;

	case cPartSenderDied:
	    to= 0;
	    break;
	}
    } else if (op == attachedTo) {   
	if (part == cPartSenderDied)
	    attachedTo= 0;
	else {
	    Rectangle r= Guard(op,Shape)->GetTextRect();
	    SetSpan(Rectangle(r.NW(), r.SE()));
	}
    }
}

void TextShape::SetContainer(VObject *vop)
{
    Shape::SetContainer(vop);
    if (to)
	to->SetContainer(vop);
}

ostream& TextShape::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    s << attachedTo SP << to SP << captured SP;
    return s;
}

istream& TextShape::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    s >> attachedTo >> to >> Bool(captured);
    to->SetOrigin(bbox.origin);
    to->SetNoSelection(FALSE);
    return s;
}

ShapeSketcher *TextShape::NewSketcher(class DrawView *dv, SketchModes m)
{
    return new TextShapeSketcher(dv, this, m); 
}

ShapeStretcher *TextShape::NewStretcher(class DrawView *dv, int handle)
{
    if (captured)
	return (ShapeStretcher*) gNoChanges;
    return new TextShapeStretcher(dv, this, handle);
}

Font *TextShapeDefaultFont()
{
    if (TextShape::defaultFont)
	return TextShape::defaultFont;
    return TextShape::defaultFont= new_Font(eFontHelvetica, 14);
}

//---- TextShapeSketcher -------------------------------------------------------

TextShapeSketcher::TextShapeSketcher(DrawView *dv, Shape *pro, SketchModes m)
						    : ShapeSketcher(dv, pro, m) 
{
    Font *f= TextShapeDefaultFont();
    lineHeight= f->Spacing();
    baseHeight= f->Ascender(); 
}

Command *TextShapeSketcher::TrackMouse(TrackPhase tp, Point ap, Point pp, Point np)
{
    ShapeSketcher::TrackMouse(tp, ap, pp, np);
    if (tp == eTrackRelease) {
	newshape= (Shape*)proto->Clone();
	newshape->SetContainer(view);
	newshape->Init(ap, np);
	view->Insert(newshape);
	view->SetActiveText((TextShape*)newshape);
	return gNoChanges;
    }
    return this;
}

void TextShapeSketcher::TrackConstrain(Point ap, Point pp, Point *np)
{
    if (ap == pp) {     // constrain left/top corner
	ShapeSketcher::TrackConstrain(ap, pp, np);
	np->y-= baseHeight;
    } else              // constrain right/bottom corner
	np->y= ap.y + lineHeight;
}

//---- TextShapeStretcher ------------------------------------------------------

TextShapeStretcher::TextShapeStretcher(DrawView *dv, Shape *p, int h)
						    : ShapeStretcher(dv, p, h) 
{
}

void TextShapeStretcher::TrackConstrain(Point, Point, Point *np)
{
    np->x= max(sp->bbox.origin.x + cMinParagraph, np->x);
}

