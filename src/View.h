#ifndef View_First
#ifdef __GNUG__
#pragma once
#endif
#define View_First

#include "VObject.h"

extern bool gPrinting;

enum ViewFlags {
    eViewNoPrint=   BIT(eVObjLast+1),
    eViewLast   =   eVObjLast + 1
};

enum ViewSizeDet {
    eViewHFit   = BIT(0),
    eViewVFit   = BIT(1),
    eViewDefault= 0
};

class View: public VObject {
    class SeqCollection *clippers;
    class EvtHandler *nexthandler;
protected:
    class Clipper *focus;
public:
    MetaDef(View);
    
    View(class EvtHandler *eh, Rectangle itsExtent, int id= -1);
    ~View();
    
    void SetExtent(Point);
    SeqCollection *GetClipperList()
	{ return clippers; }
    void CheckOpen();
    EvtHandler *GetNextHandler();
    void SetNextHandler(EvtHandler *);
    class Document *GetDocument();
    Rectangle GetViewedRect();
    
    //---- Drawing ---------------------------------
    void DrawAll(Rectangle, bool);
    void InvalidateRect(Rectangle);
    
    virtual void ShowInAllClippers(VoidObjMemberFunc, Object *op,
				void* =0, void* =0, void* =0, void* =0); 
				// obsolete
    virtual void Update();
    
    //---- Scrolling --------------------------------
    void AddToClipper(class Clipper*);
    void RemoveFromClipper(class Clipper*);
    virtual void ConstrainScroll(Point*);
    void RevealRect(Rectangle r, Point e);
    void RevealAlign(Rectangle, VObjAlign al= (VObjAlign)(eVObjHLeft+eVObjVTop));
    void Scroll(int mode, Point scroll, bool redraw= TRUE);
    
    //---- Clipboard --------------------------------
    virtual bool CanPaste(char*);
    virtual void SelectionToClipboard(char*, ostream &os);
    virtual Command *PasteData(char*, istream &s);
    virtual bool HasSelection();
    
    //---- Menu related methods ---------------------
    void DoCreateMenu(class Menu*);
    Command *DoMenuCommand(int cmd);
    void DoSetupMenu(class Menu*);

    //---- Events -----------------------------------
    Command *Input(Point, Token, Clipper*);
    Command *DoCursorKeyCommand(EvtCursorDir, Point, Token);
    Command *DoFunctionKeyCommand(int, Point, Token);
	
    //---- Inspecting -------------------------------
    void InspectorId(char *buf, int sz);
};

#endif View_First
