#ifndef Button_First
#ifdef __GNUG__
#pragma once
#endif
#define Button_First

#include "VObject.h"
#include "VObjectPair.h"

//---- Button ------------------------------------------------------------------
  
enum ButtonFlags {
    eButtonIdle     =   BIT(eCompVObjLast+1),
    eButtonLast     =   eCompVObjLast + 1,
    eButtonDefault  =   eCompVObjDefault
};

class Button: public CompositeVObject {
public:
    MetaDef(Button);
    Button(int id= cIdNone, VObject *gop= 0);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void DoOnItem(int, VObject*, Point);
    virtual void Flush(int msec= 200);
};

//---- ButtonCommand -----------------------------------------------------------

class ButtonCommand: public Command {
    Button *item;
    bool lastinside, inside;
    Rectangle rect;
public:
    ButtonCommand(Button*, Rectangle, bool b= FALSE);
    void TrackFeedback(Point, Point, bool);
    Command *TrackMouse(TrackPhase, Point, Point, Point);
};

//---- ActionButton ------------------------------------------------------------

enum ActionButtonFlags {
    eActionDefaultButton  = BIT(eButtonLast+1),
    eActionButtonLast     = eButtonLast+1
};

class ActionButton: public Button {
public:
    MetaDef(ActionButton);
    ActionButton(int id= cIdNone, VObject *g= 0, bool dflt= FALSE);
    ActionButton(int id, char *t, bool dflt= FALSE);
    
    int CalcBorder()
	{ return TestFlag(eActionDefaultButton) ? 6 : 2; }
    Metric GetMinSize();
    int Base();
    void SetOrigin(Point);
    void SetExtent(Point e);
    void Draw(Rectangle);
    void DrawHighlight(Rectangle);
    void Control(int, int, void*);
    void SetContainer(VObject*);
};

//---- ImageButton -------------------------------------------------------------

class ImageButton: public Button {
protected:
    bool idleEvents;
    Bitmap *bm1, *bm2;
public:
    MetaDef(ImageButton);
    ImageButton(int id= cIdNone, Bitmap *b1= 0, Bitmap *b2= 0, bool idle= FALSE);
    ~ImageButton();
    Metric GetMinSize();
    void DrawInner(Rectangle r, bool highlight);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- OnOffItem ---------------------------------------------------------------
  
class OnOffItem: public Button {
protected:
    Bitmap *on, *off, *highlight;
    bool state;
public:
    MetaDef(OnOffItem);
    OnOffItem(int id, Bitmap*, Bitmap*, Bitmap*, bool);
    void SetState(bool, bool redraw= TRUE);
    bool GetValue()
	{ return state; }
    void SendDown(int, int, void*);
    void DoOnItem(int, VObject*, Point);
    Metric GetMinSize();
    int Base();
    void Draw(Rectangle);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- RadioButton -------------------------------------------------------------

class RadioButton: public OnOffItem {
public:
    MetaDef(RadioButton);
    RadioButton(int id= cIdNone, bool state = FALSE);
    void InitNew();
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- ToggleButton ------------------------------------------------------------

class ToggleButton: public OnOffItem {
public:
    MetaDef(ToggleButton);
    ToggleButton(int id= cIdNone, bool state = FALSE);
    void InitNew();
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

//---- LabeledButton -----------------------------------------------------------

class LabeledButton: public VObjectPair {
public:
    MetaDef(LabeledButton);
    LabeledButton(int id= cIdNone, OnOffItem *b= 0, VObject *g= 0,
				Point gap= gPoint10, VObjAlign g= eVObjVBase);
    LabeledButton(int id, char *t, bool w= TRUE);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void Control(int, int, void*);
};

#endif Button_First

