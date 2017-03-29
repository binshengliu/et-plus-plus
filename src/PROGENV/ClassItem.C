//$ClassItem,ObjectItem,$

#include "ClassItem.h"
#include "Class.h"
#include "String.h"

//---- ClassItem -----------------------------------------------------

MetaImpl(ClassItem, (TP(cl), 0));

ClassItem::ClassItem(int id, Class *c, char *label) 
	: TextItem(id, c->Name(), 
	    gFixedFont->WithFace(c->IsAbstract() ? eFaceItalic : eFacePlain),
		Point(2,1))
{
    cl= c;
    if (label)
	SetString(label);
}

int ClassItem::Compare(ObjPtr op)
{
    return StrCmp((byte*)ClassName(), (byte*)Guard(op, ClassItem)->ClassName(), -1, sortmap);
}

bool ClassItem::IsEqual(ObjPtr b)
{
    return b->IsKindOf(ClassItem) &&
	    StrCmp((byte*)ClassName(), (byte*)((ClassItem*)b)->ClassName()) == 0;
}

Metric ClassItem::GetMinSize()
{
    if (TestFlag(eClItemCollapsed))
	return Metric(Point(8));
    return TextItem::GetMinSize();
}

void ClassItem::Draw(Rectangle r)
{
    if (TestFlag(eClItemCollapsed))
	GrPaintRect(ContentRect().Inset(2), ePatGrey50);
    else
	TextItem::Draw(r);
}

//---- ObjectItem --------------------------------------------------------------

MetaImpl(ObjectItem, (TP(op), 0));

ObjectItem::ObjectItem(int id, Object *o)
				    : TextItem(id, 0, gFixedFont, Point(2,1))
{
    char buf[200];
    buf[0]= '\0';
    op= o;
    op->InspectorId(buf, sizeof(buf));
    if (strlen(buf))
	SetFString(FALSE, "0x%x %s", int(op), buf);
    else
	SetFString(FALSE, "0x%x", int(op));
}

ObjectItem::ObjectItem(int id, char *msg, Object *o)
				    : TextItem(id, 0, gFixedFont, Point(2,1))
{
    op= o;
    SetString(msg);
}

ObjectItem::ObjectItem(char *iv, Object *o)
				    : TextItem(cIdNone, 0, gFixedFont, Point(4,0))
{
    op= o;
    SetFString(FALSE, "%s 0x%x", iv, int(op));    
}
