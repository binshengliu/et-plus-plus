#ifndef VObjectTView_First
#ifdef __GNUG__
#pragma once
#endif
#define VObjectTView_First

#include "TextView.h"

//---- VObjectTextView --------------------------------------------------------

class VObjectTextView: public TextView {
    VObject *kbdFocus;
    void InvalidateVObjects(int from, int to);
public:
    MetaDef(VObjectTextView);

    VObjectTextView(class EvtHandler*, Rectangle r, class VObjectText *t,
	    eTextJust m= eLeft, eSpacing= eOne, bool wrap= TRUE,
	    TextViewFlags= (TextViewFlags)eVObjDefault, Point border= gBorder,
	    int id= cIdNone);    
	    // contentRect.width/height can be set to cFit

    //---- overriden to update VObjects 
    void InvalidateRange(int from, int to);
    void InvalidateRange(int from, Point fp, int to, Point tp);
    void SetOrigin(Point);
    void Control(int id, int part, void *val);
    Text *SetText(Text *newText);

    //---- distribute input events to VObjects
    Command *DispatchEvents(Point lp, Token, Clipper*);

    GrCursor GetCursor(Point);    
    istream& ReadFrom(istream &s);
};

//---- StretchWrapper ---------------------------------------------------------
//     a VObject installed in a StretchWrapper and inserted in a 
//     VObjectText can be resized interactively

class StretchWrapper: public CompositeVObject {
protected:
    Point interiorOffset, extentDiff, border;
    VObject *interior;
    bool kbdFocus;
public:
    MetaDef(StretchWrapper);
    
    StretchWrapper(VObject *inner, Point border, int= cIdNone);
    VObject *GetInterior()
	{ return interior; }
    Point GetInteriorOffset()
	{ return interiorOffset; }
    void SetOrigin(Point at);
    void SetExtent(Point e);
    Metric GetMinSize();
    int Base();
    Command *DoLeftButtonDownCommand(Point, Token, int);
    Command *DoKeyCommand(int ch, Point lp, Token t);
    void Control(int id, int part, void *val);
    void SendDown(int id, int part, void *val);
    GrCursor GetCursor(Point);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
    void Draw(Rectangle);
};

#endif VObjectTView_First
