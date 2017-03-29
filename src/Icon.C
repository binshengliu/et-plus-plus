//$Icon$
#include "Icon.h"
#include "Menu.h"
#include "String.h"
#include "CmdNo.h"

//---- Icon --------------------------------------------------------------------
  
MetaImpl(Icon, (TP(menu), 0));

Icon::Icon(EvtHandler *eh, VObject *v) : BlankWin(eh, v, gPoint_1)
{
}

Icon::~Icon()
{
    SafeDelete(menu);
}

Command *Icon::DoLeftButtonDownCommand(Point lp, Token t, int clicks)
{
    if (clicks >= 2)
	return DoMenuCommand(cCOLLAPSE);
    return BlankWin::DoLeftButtonDownCommand(lp, t, clicks);
}

class Menu *Icon::GetMenu()
{
    if (menu == 0)
	menu= new Menu("Icon");
    return menu;
}

void Icon::DoCreateMenu(class Menu *m)
{
    BlankWin::DoCreateMenu(m);    
    m->ReplaceItem(cCOLLAPSE, "expand");
}

void Icon::UpdateIconLabel(char *label)
{
    if (!label)
	return;
    VObject *vop= FindItem(cIdIconLabel);
    if (!vop || !vop->IsKindOf(TextItem))
	return;
    TextItem *tip= (TextItem *)vop;
    
    label= BaseName(label);
    if (strcmp(tip->AsString(), label) != 0) {
	tip->SetString(label, TRUE);
	SetMinExtent(GetViewedVObject()->GetMinSize());
	CalcExtent();
	SetOrigin(GetOrigin());
	ForceRedraw();
	UpdateEvent();
    }
}
