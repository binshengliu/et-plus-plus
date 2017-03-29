#ifndef RestrTView_First
#ifdef __GNUG__
#pragma once
#endif
#define RestrTView_First

#include "TextView.h"

//---- RestrTextView --------------------------------------------------------
// restricted textview: the contents of a restricted textview will always match
// a regular expression

class RestrTextView: public TextView {
    class RegularExp *rex;

    bool Match(class Text*, RegularExp *);
    class Text *CopyText();

public:
    MetaDef(RestrTextView);
    
    RestrTextView(EvtHandler *eh, RegularExp *, Rectangle r,  
	     Text *t, eTextJust m= eLeft, eSpacing= eOne, bool wrap= TRUE,
		TextViewFlags= (TextViewFlags)eVObjDefault, 
		    Point border= gBorder, int id = -1);    
	    
    virtual void NotAccepted ();
	    // text modification refused due to mismatch with
	    // regular expression, default implementation executes a ring bell
    
    RegularExp *GetRegExp()
	{ return rex; }
    void SetRegExp(RegularExp*);
	    
    //---- overriden to check match against regular expression 
    void Cut();            
    void Paste(Text *insert); 
    
    //---- activation passivation ----------------------------------------------
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
    
};

#endif RestrTView_First  
