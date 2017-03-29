#ifndef FloatItem_First
#ifdef __GNUG__
#pragma once
#endif
#define FloatItem_First

#include "EditTextItem.h"

//---- FloatItem ---------------------------------------------------------------

class FloatItem: public EditTextItem {
    float minVal, maxVal, inc;
public:
    MetaDef(FloatItem);
    
    FloatItem(int id, float val, float minval, float maxval,
						    int ndigits= 5, float inc=1);
    void SendDown(int, int, void*);
    float GetValue();
    void SetValue(float, bool redraw= TRUE);
    void SetRange(float min, float max);
    void GetRange(float &min, float &max);
    void SetInc(float);
    float GetInc();
    void Incr();
    void Decr();
    bool Validate();
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif FloatItem_First


