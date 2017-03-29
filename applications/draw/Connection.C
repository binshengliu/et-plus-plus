//$Connection$
#include "Connection.h"

//---- Connection ---------------------------------------------------------------

MetaImpl(Connection, (TP(startShape), TP(endShape), T(startCenter), T(endCenter), 0));

Connection::Connection(View *v, Shape* s1, Shape* s2)
{
    SetContainer(v);
    startShape= s1;
    endShape= s2;
    if (s1 && s2) {
	startShape->AddObserver(this);
	endShape->AddObserver(this);
	UpdatePoints();
    }
}

Connection::~Connection()
{
    if (startShape)
	startShape->RemoveObserver(this);
    if (endShape)
	endShape->RemoveObserver(this);
    startShape= endShape= 0;
}

void Connection::DoObserve(int, int part, void*, Object *op)
{
    if (part == cPartSenderDied) {
	startShape= endShape= 0;
	delete this;                // delete myself too
    } else {
	Shape *s= (Shape*)op;
	if (startShape == s || endShape == s) {
	    SetDeleted(s->GetDeleted());
	    Invalidate();
	    UpdatePoints();
	    // SetSpan(Rectangle(startCenter, endCenter));
	}
    }
}

void Connection::UpdatePoints()
{
    Point e= endShape->GetConnectionPoint(0),
	  s= startShape->GetConnectionPoint(0);
    startCenter= startShape->Chop(endShape->GetConnectionPoint(s));
    endCenter= endShape->Chop(startShape->GetConnectionPoint(e));
    Invalidate();
    Init(startCenter, endCenter);
    Invalidate();
}

void Connection::Moveby(Point delta)
{
}

Rectangle Connection::GetTextRect()
{
    return Rectangle(bbox.Center(), 0).Expand(Point(50, 0));
}

void Connection::Highlight(HighlightState h)
{
    GrSetPattern(ePatGrey50);
    LineShape::Highlight(h);
    GrSetPattern(ePatBlack);
}

int Connection::PointOnHandle(Point)
{
    return -1;
}

bool Connection::ContainsPoint(Point p)
{
    if (LineShape::PointOnHandle(p))
	return FALSE;
    return LineShape::ContainsPoint(p);
}

ShapeStretcher *Connection::NewStretcher(class DrawView *, int)
{
    return (ShapeStretcher*) gNoChanges;
}

ostream& Connection::PrintOn(ostream& s)
{
    LineShape::PrintOn(s);
    return s << startShape SP << endShape SP << startCenter SP << endCenter SP;
}

istream& Connection::ReadFrom(istream& s)
{
    LineShape::ReadFrom(s);
    s >> startShape >> endShape >> startCenter >> endCenter;
    Init(startCenter, endCenter);
    return s;
}
