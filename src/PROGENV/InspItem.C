//$InspectorItem,InspectorItem0,ClassItem0,SuperItem0$

#include <stdio.h>

#include "InspItem.h"
#include "String.h"
#include "Alert_e.h"
#include "System.h"
#include "Reference.h"
#include "ClassManager.h"
#include "EtPeCmdNo.h"

//---- InspectorItem0 ---------------------------------------------------------------

MetaImpl0(InspectorItem0);

InspectorItem0::InspectorItem0(char *n, GrFace fc)
			    : TextItem("", gFixedFont->WithFace(fc), Point(2,1))
{
    name= 0;
    died= FALSE;
    if (n)
	strreplace(&name, n);
}

InspectorItem0::~InspectorItem0()
{
    SafeDelete(name);
}

void InspectorItem0::Deref(bool)
{
}

void InspectorItem0::Update()
{
}

void InspectorItem0::Died()
{
    died= TRUE;
}

void InspectorItem0::Draw(Rectangle)
{
    if (text && *text) {
	Point p= contentRect.origin+border;
	p.y+= font->Ascender();
	GrShowString(font, died ? ePatGrey50 : ePatBlack, p, (byte*)text);
    }
}

//---- ClassItem0 ---------------------------------------------------------------

MetaImpl0(ClassItem0);

ClassItem0::ClassItem0(void *a, char *name, GrFace f) : SuperItem0(name, f)
{
    addr= a;
}

void ClassItem0::Update()
{
    SetFString(FALSE, "%s: (0x%x)", name, (unsigned)addr);
}

//---- SuperItem0 ---------------------------------------------------------------

MetaImpl0(SuperItem0);

SuperItem0::SuperItem0(char *name, GrFace f) : InspectorItem0(name, f)
{
}

void SuperItem0::Deref(bool expanded)
{
    int id= expanded? cIdLoadRefNew : cIdLoadRef;
    Object *isa= gClassManager->Find(name);
    if (isa) 
	Control(id, 0, new Ref(*isa));
    else
	ShowAlert(eAlertNote, "no isa link");
}

void SuperItem0::Update()
{
    SetFString(FALSE, "%s:", name);
}

//---- InspectorItem ---------------------------------------------------------------

MetaImpl0(InspectorItem);

InspectorItem::InspectorItem(char *n, Ref *r) : InspectorItem0(n, eFacePlain)
{
    ref= r;
    Enable(r->CanDeref(), FALSE);
}

InspectorItem::~InspectorItem()
{
    SafeDelete(ref);
}

void InspectorItem::Deref(bool expanded)
{
    int id= expanded? cIdLoadRefNew : cIdLoadRef;
    Ref *r= ref->Deref();
    if (r == 0) 
	ShowAlert(eAlertNote, "can't deref instance variable \"%s\"", name);
    else
	Control(id, 0, r);
}

void InspectorItem::Update()
{
    char buf[1000];
    
    sprintf(buf, "  %-14.14s %1s%-18.18s: %s",
		    ref->TypeName(),
		    ref->PreName(),
		    form("%s%s ", name, ref->PostName()),
		    ref->Value());
    SetString(buf);
}
