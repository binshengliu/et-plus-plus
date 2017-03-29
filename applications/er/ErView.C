//$ErView$

#include "ErShapes.h"
#include "ErCommands.h"
#include "ErView.h"
#include "ErDocument.h"
#include "SeqColl.h"

//---- ErView ------------------------------------------------------------------

MetaImpl(ErView, (TP(doc), 0));

ErView::ErView(ErDocument *d, Point ext) : View(d, ext)
{   
    doc= d;
}

void ErView::Draw(Rectangle r)
{
    Iter next(doc->GetShapes());
    ErShape *s;
    
    // first pass: draw connections
    while (s= (ErShape*) next())
	if (s->IsKindOf(Connection))
	    s->DrawAll(r);
	    
    // second pass: draw shapes
    next.Reset(doc->GetShapes());
    while (s= (ErShape*) next())
	if (! s->IsKindOf(Connection))
	    s->DrawAll(r);
}

ErShape *ErView::FindShape(Point p)
{
    RevIter next(doc->GetShapes());
    ErShape *s;
    
    while (s= (ErShape*) next())
	if (s->ContainsPoint(p))
	    return s;
    return 0;
}

Command *ErView::DispatchEvents(Point p, Token t, Clipper *vf)
{
    RevIter next(doc->GetShapes());
    ErShape *s;
    Command *cmd;
    
    // forward input to textviews
    while (s= (ErShape*) next())
	if (cmd= s->Input(p, t, vf))
	    return cmd;
    return View::DispatchEvents(p, t, vf);
}

Command *ErView::DoLeftButtonDownCommand(Point p, Token t, int clicks)
{
    ErShape *s= FindShape(p);
    if (s) {
	if (t.Flags & eFlgCntlKey)
	    return new ConnectCommand(this, doc, s);
	return new VObjectMover(s, ContentRect());
    }
    return View::DoLeftButtonDownCommand(p, t, clicks);
}   
