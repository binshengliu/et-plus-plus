//$FloatItem$

#include "FloatItem.h"
#include "CheapText.h"
#include "RestrTView.h"
#include "RegularExp.h"
#include "Alert_e.h"

static RegularExp *rexDouble;

//---- FloatItem ---------------------------------------------------------------

MetaImpl(FloatItem, (T(minVal), T(maxVal), T(inc), 0));

FloatItem::FloatItem(int id, float val, float min, float max, int nd, float i) : 
	EditTextItem(id,
	    new RestrTextView((View*)0,
		rexDouble ? rexDouble : rexDouble= new RegularExp("\\(+\\|-\\)?[0-9]*\\(\\.[0-9]*\\)?\\([eE]\\(+\\|-\\)?[0-9]*\\)?"),
		    gFitRect, new CheapText), gSysFont->Width('0')*nd)
{
    minVal= min;
    maxVal= max;
    inc= i;
    SetValue(val, FALSE);
}

void FloatItem::Incr()
{
    float newVal, oldVal= GetValue();

    if (oldVal+inc > maxVal)
	newVal= minVal;
    else
	newVal= oldVal+inc;
    SetValue(newVal);
}

void FloatItem::Decr()
{
    float newVal, oldVal= GetValue();

    if (oldVal-inc < minVal)
	newVal= maxVal;
    else
	newVal= oldVal-inc;
    SetValue(newVal);
}

void FloatItem::SendDown(int id, int part, void *val)
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

float FloatItem::GetValue()
{
    return GetText()->AsFloat();
}

void FloatItem::SetValue(float newVal, bool redraw)
{
    newVal= range(minVal, maxVal, newVal);
    if (newVal != GetValue() || GetTextSize() == 0) {
	SetString((byte*)form("%g", newVal));
	if (redraw)
	    ForceRedraw();
    }
}

bool FloatItem::Validate()
{
    float oldVal= GetValue();
    
    if (oldVal < minVal || oldVal > maxVal) {
	ShowAlert(eAlertNote, "%g not in range %g-%g", oldVal, minVal, maxVal);
	SetValue(oldVal);
	return FALSE;
    }
    return TRUE;
}

void FloatItem::GetRange(float &min, float &max)
{
    min= minVal;
    max= maxVal;
}

void FloatItem::SetRange(float min, float max)
{
    minVal= min;
    maxVal= max;
    SetValue(GetValue());
}

ostream& FloatItem::PrintOn(ostream &s)
{
    EditTextItem::PrintOn(s);
    return s << minVal SP << maxVal SP << GetValue() SP << inc SP;
}

istream& FloatItem::ReadFrom(istream &s)
{
    float f;
    EditTextItem::ReadFrom(s);
    s >> minVal >> maxVal >> f >> inc;
    SetValue(f, FALSE);
    return s;
}

