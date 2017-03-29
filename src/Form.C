//$Form$

#include "Form.h"
#include "BorderItems.h"
#include "OrdColl.h"

MetaImpl0(Form);

Form::Form(int id, VObjAlign a, Point g, Collection *cp) : Cluster(id, a, g, cp)
{
    transposed= FALSE;
}

Form::Form(int id, VObjAlign a, Point g, VObject *va_(vop), ...)
					    : Cluster(id, a, g, (Collection*)0)
{
    transposed= FALSE;
    va_list ap;
    va_start(ap, va_(vop));
    Add(va_(vop));
    SetItems(ap);
    Init(a, g);
    va_end(ap);
}

Form::Form(int id, VObjAlign a, Point g, char* va_(label), ...) 
					    : Cluster(id, a, g, (Collection*)0)
{
    transposed= FALSE;
    va_list ap;
    va_start(ap, va_(label));
    char *s;
    VObject *vop;
    Add(new TextItem(va_(label)));
    for (int i= 1; ; i++) {
	if (EVEN(i)) 
	    if (s= va_arg(ap, char*))
		Add(new TextItem(s));
	    else
		break;
	else
	    if (vop= va_arg(ap, VObject*))
		Add(new BorderItem(vop,1));
	    else
		break;
    }
    Init((VObjAlign)(a|eVObjVBase), g);
    va_end(ap);
}

void Form::Transpose()
{
    Collection *tmp= new OrdCollection(list->Size());
    int i,j;
    for (i= 0; i < cols; i++)
	for (j= 0; j < rows; j++) 
	    tmp->Add(At(i+j*cols));
    SafeDelete(list);
    list= tmp;
    transposed= TRUE;
}

short Form::CalcCols()
{
    return 2;
}

void Form::CacheMinSize()
{
    if (!transposed)
	Transpose();
    Cluster::CacheMinSize();
}

void Form::SetOrigin(Point at)
{
    register VObject *dip;
    register int x, y;
    Point a;
    VObjAlign va,ta;
    
    VObject::SetOrigin(at);
    
    a.x= at.x;
    ta= (VObjAlign)(align & ~(eVObjHLeft|eVObjHCenter|eVObjHRight));
    for (x= 0; x < cols; x++) {
	a.y= at.y;
	for (y= 0; y < rows; y++) {
	    if (dip= GetItem(x,y)) {
		va= ta;
		switch(x) {
		case 0:
		    switch(align & eVObjH) {
		    case eVObjHLeft:
			va= (VObjAlign)(va | eVObjHLeft); 
			break;
		    case eVObjHCenter:
		    case eVObjHRight:
			va= (VObjAlign)(va | eVObjHRight); 
			break;
		    }
		    break;

		case 1:   
		    switch(align & eVObjH) {
		    case eVObjHLeft:
		    case eVObjHCenter:
			va= (VObjAlign)(va | eVObjHLeft); 
			break;
		    case eVObjHRight: 
			va= (VObjAlign)(va | eVObjHRight); 
			break;
		    }
		    break;
		}
		dip->Align(a, Metric(wd[x], ht[y], bs[y]), va);
	    }
	    a.y+= ht[y] + actGap.y;
	}
	a.x+= wd[x] + actGap.x;
    }
}
