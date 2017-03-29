#ifndef EditTextItem_First
#ifdef __GNUG__
#pragma once
#endif
#define EditTextItem_First

#include "Clipper.h"
#include "TextView.h"

//---- EditTextItem ------------------------------------------------------------

class EditTextItem: public Clipper {
    int oldTextSize;
    bool freeView;

protected:
    void EditTextItem::FocusChanged(int id, bool dontRedraw);

public:
    MetaDef(EditTextItem);
    
    EditTextItem(int id, char* initText= 0, int width= 0, int lines= 1, Point b= gBorder);
    EditTextItem(int id, class TextView *tv, int w= 0, int l= 1);
    ~EditTextItem();
    
    class TextView *Tv()
	{ return (TextView*) vop; }
    void Init(TextView *tv, int width, int lines, char *it);
    Metric GetMinSize();
    int Base();
    void SetNoSelection(bool redraw= TRUE)
	{ Tv()->SetNoSelection(redraw); }
    void SetSelection(int from= 0, int to= cMaxInt, bool redraw = TRUE)
	{ Tv()->SetSelection(from, to, redraw); }
    class Text *GetText()
	{ return Tv()->GetText(); }
    int GetTextSize()
	{ return GetText()->Size(); }
    virtual bool Validate();
    void Enable(bool b= TRUE, bool redraw= TRUE);
    class Text *SetText(Text*);
    void SetString(byte *str, int len= -1);
    void SetContainer(VObject*);
    void Control(int, int part, void *op);
    void SendDown(int, int, void*);
};

//---- RestrTextItem ----------------------------------------------------------

class RestrTextItem: public EditTextItem {
public:
    MetaDef(RestrTextItem);
    RestrTextItem(int id, class RegularExp *rex, char* initText= 0, int width= 0, 
								  int lines= 1);
};

#endif EditTextItem_First

