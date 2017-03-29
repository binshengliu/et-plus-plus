//$Panner$
 
#include "Panner.h"
#include "Clipper.h"

//---- Panner ------------------------------------------------------------------

Panner::Panner(Clipper *mc, Point extent) : Slider(5555, eVert)
{
    cl= mc;
    cl->AddObserver(this);
    contentRect.extent= extent;
    if (extent.x >= 0)
	SetFlag(eVObjHFixed);
    if (extent.y >= 0)
	SetFlag(eVObjVFixed);
}

Panner::~Panner()
{
    if (cl)
	cl->RemoveObserver(this);
}

Metric Panner::GetMinSize()
{
    return Metric(40, 40); 
}

void Panner::Control(int id, int part, void *val)
{
    if (id == 5555) {
	if (part == eSliderThumb)
	    cl->Scroll(cPartScrollAbs, *((Point*)val));
    } else
	Slider::Control(id, part, val);
}

void Panner::DoObserve(int, int part, void *vp, ObjPtr op)
{
    if (op == cl) {
	switch (part) {
	case cPartExtentChanged:
	    SetThumbRange(*((Point*)vp));
	    break;
	    
	case cPartScrollPos:
	    SetVal(*((Point*)vp), TRUE);
	    Update();
	    break;
	    
	case cPartViewSize:
	    SetMax(*((Point*)vp));
	    Update();
	    break;
	    
	case cPartSenderDied:
	    cl= 0;
	    break;
	}
    }
}

