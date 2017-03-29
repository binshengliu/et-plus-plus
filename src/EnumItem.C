//$EnumItem$

#include "EnumItem.h"
#include "Expander.h"
#include "Buttons.h"
#include "ObjectTable.h"

//---- EnumItem ----------------------------------------------------------------

static u_short SliderUpBits[]= {
#   include "images/SliderButtonUp.image"
};

static u_short SliderDownBits[]= {
#   include "images/SliderButtonDown.image"
};

static u_short SliderUpInvBits[]= {
#   include "images/SliderButtonUpInv.image"
};

static u_short SliderDownInvBits[]= {
#   include "images/SliderButtonDownInv.image"
};

static Bitmap *SliderUp, *SliderDown, *SliderUpInv, *SliderDownInv;

MetaImpl0(EnumItem);

EnumItem::EnumItem(int id, VObjAlign, VObject *ci, Point gap)
		    : Cluster(id, eVObjVCenter, gap, (Collection*)0)
{
    if (SliderUp == 0)
	ObjectTable::AddRoots(
	    SliderUp= new Bitmap(Point(13,10), SliderUpBits),
	    SliderDown= new Bitmap(Point(13,10), SliderDownBits),
	    SliderUpInv= new Bitmap(Point(13,10), SliderUpInvBits),
	    SliderDownInv= new Bitmap(Point(13,10), SliderDownInvBits),
	    0);
    Add(ci);
    Add(new Expander(cIdNone, eVert, gPoint0, 
		new ImageButton(cIdUp, SliderUp, SliderUpInv, TRUE),
		new ImageButton(cIdDown, SliderDown, SliderDownInv, TRUE),
		0
	    )
	);
}

void EnumItem::Control(int id, int part, void *val)
{
    switch (id) {
    case cIdDown:
	At(0)->SendDown(0, cPartDecr, 0);
	UpdateEvent();
	break;
    case cIdUp:
	At(0)->SendDown(0, cPartIncr, 0);
	UpdateEvent();
	break;
    default:
	Cluster::Control(id, part, val);
	break;
    }
}

