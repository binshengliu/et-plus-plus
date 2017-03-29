//$NumItem$

#include "NumItem.h"
#include "CheapText.h"
#include "RestrTView.h"
#include "RegularExp.h"
#include "Alert_e.h"

static RegularExp *rexInt;

//---- NumItem -----------------------------------------------------------------

MetaImpl(NumItem, (T(minVal), T(maxVal), T(inc), 0));

NumItem::NumItem(int id, int val, int min, int max, int ndig, int increment) : 
	EditTextItem(id,
	    new RestrTextView((View*)0,
		rexInt ? rexInt : rexInt= new RegularExp("\\(+\\|-\\)?[0-9]*"),
			gFitRect, new CheapText), gSysFont->Width('0')*ndig)
{
    minVal= min;
    maxVal= max;
    inc= increment;
    SetValue(val, FALSE);
}

void NumItem::Incr()
{
    int newVal, oldVal= GetValue();

    if (oldVal+inc > maxVal)
	newVal= minVal;
    else
	newVal= oldVal+inc;
    SetValue(newVal);
}

void NumItem::Decr()
{
    int newVal, oldVal= GetValue();

    if (oldVal-inc < minVal)
	newVal= maxVal;
    else
	newVal= oldVal-inc;
    SetValue(newVal);
}

void NumItem::SendDown(int id, int part, void *val)
{
    switch (part) {
    case cPartIncr:
	Incr();
	break;
    case cPartDecr:
	Decr();
	break;
    default:
	EditTextItem::SendDown(id, part, val);
	return;
    }
}

int NumItem::GetValue()
{
    return GetText()->AsInt();
}

void NumItem::SetValue(int newVal, bool redraw)
{
    newVal= range(minVal, maxVal, newVal);
    if (newVal != GetValue() || GetTextSize() == 0) {
	SetString((byte*) form("%d", newVal));
	if (redraw)
	    ForceRedraw();
    }
}

void NumItem::GetRange(int &min, int &max)
{
    min= minVal;
    max= maxVal;
}

void NumItem::SetRange(int min, int max)
{
    minVal= min;
    maxVal= max;
    SetValue(GetValue());
}

bool NumItem::Validate()
{
    int oldVal= GetValue();
    
    if (oldVal < minVal || oldVal > maxVal) {
	ShowAlert(eAlertNote, "%d not in range %d-%d", oldVal, minVal, maxVal);
	SetValue(oldVal);
	return FALSE;
    }
    return TRUE;
}

ostream& NumItem::PrintOn(ostream &s)
{
    EditTextItem::PrintOn(s);
    return s << minVal SP << maxVal SP << GetValue() SP << inc SP;
}

istream& NumItem::ReadFrom(istream &s)
{
    int i;
    EditTextItem::ReadFrom(s);
    s >> minVal >> maxVal >> i >> inc;
    SetValue(i, FALSE);
    return s;
}

