#ifndef NumItem_First
#ifdef __GNUG__
#pragma once
#endif
#define NumItem_First

#include "EditTextItem.h"

//---- NumItem -----------------------------------------------------------------

class NumItem: public EditTextItem {
    int minVal, maxVal, inc;
public:
    MetaDef(NumItem);
    
    NumItem(int id, int val, int minval= -cMaxInt, int maxval= cMaxInt,
						     int ndigits= 4, int inc=1);
    void SendDown(int, int, void*);
    int GetValue();
    void SetValue(int, bool redraw= TRUE);
    void SetRange(int min, int max);
    void GetRange(int &min, int &max);
    void SetInc(int);
    int GetInc();
    void Incr();
    void Decr();
    bool Validate();
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif NumItem_First

