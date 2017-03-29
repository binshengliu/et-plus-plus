//$ErShape,Entity,Relation,Connection$

#include "ErShapes.h"
#include "TextView.h"
#include "CheapText.h"
#include "CmdNo.h"

const Point cBorder(10);

//--- ErShape --------------------------------------------------------------------

MetaImpl(ErShape, (TP(textview), 0));

ErShape::ErShape(char *label) : CompositeVObject(0, (Collection*)0)
{
    SetLabel(new CheapText((byte*)label));
}

ErShape::~ErShape()
{   
    if (textview)
	textview->RemoveObserver(this);
    FreeAll();
}

void ErShape::Init(View *vp)
{
    SetContainer(vp);
    if (textview)
	textview->SetContainer(this);
    CalcExtent();
    ForceRedraw();
}

void ErShape::SetLabel(Text *t)
{
    textview= new TextView(this, Rectangle(cFit,cFit), t, eCenter);
    // add textview to CompositeVObject
    Add(textview);
    // register myself as dependent of a textview
    textview->AddObserver(this);
    SetOrigin(GetOrigin());
}

void ErShape::DoObserve(int, int part, void*, Object *op)
{
    Rectangle r;
    
    if (op == textview) {
	switch (part) {
	case cPartSenderDied:
	    textview= 0;
	    break;
	case cPartExtentChanged:
	    r= textview->ContentRect();
	    r.origin.x= contentRect.Center().x-r.extent.x/2;
	    r.extent.y*= 2;
	    SetContentRect(r.Expand(cBorder), TRUE);
	    break;
	}
    }
}

void ErShape::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    textview->SetOrigin(at+cBorder);
}

Metric ErShape::GetMinSize()
{
    Point e= textview->GetMinSize().extent;
    return Point(e.x, 2*e.y) + 2*cBorder;
}
	
void ErShape::Draw(Rectangle r)
{
    CompositeVObject::Draw(r);
    GrLine(contentRect.W(), contentRect.E());
}

Command *ErShape::Input(Point p, Token t, Clipper *vf)
{
    if (textview->ContainsPoint(p))
	return textview->Input(p, t, vf);  // forward input
    return 0;
}

ostream &ErShape::PrintOn(ostream &s)
{
    VObject::PrintOn(s);
    return s << textview->GetText() SP;
}

istream &ErShape::ReadFrom(istream &s)
{
    Text *t;
    VObject::ReadFrom(s);
    s >> t;
    SetLabel(t);
    return s;
}

//---- Entity ------------------------------------------------------------------

MetaImpl0(Entity);

void Entity::Draw(Rectangle r)
{
    RGBColor c((float) 0.9);
    GrPaintRect(contentRect, &c);
    GrStrokeRect(contentRect);
    ErShape::Draw(r);
}

bool Entity::CanConnectWith(ErShape *s)
{
    return s && s->IsKindOf(Relation);
}

//---- Relation ----------------------------------------------------------------

MetaImpl0(Relation);

void Relation::Draw(Rectangle r)
{
    RGBColor c((float) 0.8);
    GrPaintOval(contentRect, &c);
    GrStrokeOval(contentRect);
    ErShape::Draw(r);
}

bool Relation::CanConnectWith(ErShape *s)
{
    return s && s->IsKindOf(Entity);
}

//--- Connection ---------------------------------------------------------------

MetaImpl(Connection, (TP(startShape), TP(endShape), 0));

Connection::Connection(ErShape *sh, ErShape *eh) : ErShape("1,n")
{
    startShape= sh;
    endShape= eh;
    startShape->AddObserver(this);
    endShape->AddObserver(this);
    UpdatePoints();
}

Connection::~Connection()
{
    if (startShape)
	startShape->RemoveObserver(this);
    if (endShape)
	endShape->RemoveObserver(this);
}

void Connection::DoObserve(int id, int part, void *vp, Object *op)
{
    switch (part) {
    case cPartSenderDied:
	if (op == startShape)
	    startShape= 0;
	else if (op == endShape)
	    endShape= 0;
	else
	    ErShape::DoObserve(id, part, vp, op);
	break;
    case cPartExtentChanged:
    case cPartOriginChanged:
	UpdatePoints();
	break;
    }
}

bool Connection::ContainsPoint(Point)
{
    return FALSE;   // can't select Connections
}

void Connection::UpdatePoints()
{
    if (startShape && endShape && textview) {  // calculate my new bounding box
	Rectangle r= NormRect(endShape->Center(), startShape->Center());
	Rectangle tr= textview->ContentRect();
	tr.origin= r.origin + (r.extent - tr.extent)/2;
	SetContentRect(Union(r, tr), TRUE); // update bounding box
    }
}

void Connection::SetOrigin(Point at)
{
    VObject::SetOrigin(at);
    textview->SetOrigin(Center()-textview->GetExtent()/2); // center label
}

Metric Connection::GetMinSize()
{
    Rectangle r= NormRect(endShape->Center(), startShape->Center());
    Point e= Union(r, textview->ContentRect()).extent;
    return Point(e.x, 2*e.y) + 2*cBorder;
}
        
void Connection::Draw(Rectangle r)
{
    GrLine(endShape->Center(), startShape->Center());
    GrEraseRect(textview->contentRect);
    CompositeVObject::Draw(r);
}

ostream &Connection::PrintOn(ostream &s)
{
    ErShape::PrintOn(s);
    return s << startShape SP << endShape SP;
}

istream &Connection::ReadFrom(istream &s)
{
    ErShape::ReadFrom(s);
    return s >> startShape >> endShape;
}
