#ifndef DialogView_First
#ifdef __GNUG__
#pragma once
#endif
#define DialogView_First

#include "View.h"

//---- DialogView --------------------------------------------------------------

class DialogView: public View {
protected:
    class VObject *dialogRoot;
    class VObject *kbdFocus;   // VObject with kbdoard focus
    class OrdCollection *kbdFocusItems;
    class Button *defaultButton;
    bool modified;
    class Menu *menu;

    void FindNeighbours(VObject *&prev, VObject *&next, VObject *&first);

public:
    MetaDef(DialogView);

    DialogView(EvtHandler *eh= 0);
    ~DialogView();

    void SetContainer(VObject*);
    void Open(bool mode= TRUE);
    void SetOrigin(Point at);
    void SetExtent(Point e);
    Metric GetMinSize();
    virtual VObject *DoCreateDialog();
    void Update();
    void CalcLayout(bool redraw= TRUE);
    VObject *SetDialog(VObject*, bool redraw= TRUE); // returns old dialogtree
    VObject *GetRoot()
	{ return dialogRoot; }
    bool IsModified()
	{ return modified; }
    void Draw(Rectangle);

    //---- event handling
    Command *DispatchEvents(Point p, Token t, Clipper*);
    void Control(int id, int, void *v);
    virtual void DoTab(Token t);

    //---- menues
    Command *DoMenuCommand(int cmd);
    Menu *GetMenu();
    bool HasSelection();

    virtual void EnableItem(int id, bool b= TRUE);
    void DisableItem(int id)
	{ EnableItem(id, FALSE); }
    VObject *FindItem(int id);

    //---- keyboard focus
    void AddItemWithKbdFocus(VObject *);
    VObject *RemoveItemWithKbdFocus(VObject *);
    VObject *GetKbdFocus()
	{ return kbdFocus; }
    void SetKbdFocus(VObject *);
    void SetDefaultButton (Button*);
    VObject* GetDefaultButton()
	{ return (VObject*) defaultButton; }
    void Parts(class Collection *);
};

//---- Dialog -------------------------------------------------------------------

class Dialog: public DialogView {
protected:
    class BlankWin *dw;
    int actionId;
public:
    MetaDef(Dialog);

    Dialog(char *title, int f= 0, EvtHandler *eh= 0); // BWinFlags f= eBWinDefault
    ~Dialog();

    void SetTitle(char *title);
    virtual int ShowAt(VObject*, Point);
    virtual Point GetInitialPos();
    int Show();
    int ShowOnWindow(VObject *fp);
    void Control(int id, int part, void *v);
    virtual void DoSetDefaults();
    virtual void DoSave();
    virtual void DoRestore();    // called when dialog is cancelled
    virtual void DoStore();      // called when dialog is accepted
    virtual void DoSetup();
    void InspectorId(char *, int);
    void Close();
};

#endif DialogView_First

