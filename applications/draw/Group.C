//$Group$
#include "Group.h"
#include "DrawView.h"
#include "ObjList.h"

//---- Group -------------------------------------------------------------------

MetaImpl(Group, (TP(group), 0));

Group::Group(ObjList *gr)
{
    ObjListIter next(gr);
    Shape *s;
    Rectangle b;
    group= gr;

    while (s= (Shape*)next())
	b.Merge(s->bbox);
    next.Reset(group);
    while (s= (Shape*)next())
	s->bbox-= b.origin;   
    Init(b.NW(), b.SE());
}

Group::~Group()
{
    SafeDelete(group);
}

void Group::SetContainer(VObject *vop)
{
    Shape::SetContainer(vop);
    group->ForEach(Shape,SetContainer)(vop);
}
	
ObjList *Group::Ungroup()
{
    ObjList *copy= (ObjList*)group->DeepClone();
    ObjListIter next(copy);
    Shape *s;
    
    while (s= (Shape*)next())
	s->bbox+= bbox.origin;   
    return copy;
}

void Group::Draw(Rectangle r)
{
    RevObjListIter previous(group);
    register Shape *s;

    Point oldorigin= port->origin;
    port->origin+= bbox.origin;
    r.origin-= bbox.origin;
    while (s= (Shape*) previous())
	s->DrawAll(r);
    port->origin= oldorigin;
}

void Group::Highlight(HighlightState h)
{
    GrSetPattern(ePatGrey50);
    Shape::Highlight(h);
    GrSetPattern(ePatBlack);
}

ShapeStretcher *Group::NewStretcher(class DrawView *, int)
{
    return (ShapeStretcher*) gNoChanges;
}

ostream& Group::PrintOn(ostream& s)
{
    Shape::PrintOn(s);
    return s << group SP;
}

istream& Group::ReadFrom(istream& s)
{
    Shape::ReadFrom(s);
    return s >> group;
}
