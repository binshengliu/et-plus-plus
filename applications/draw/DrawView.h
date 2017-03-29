#ifndef DrawView_First
#define DrawView_First

#include "View.h"
#include "ObjList.h"

class Shape;

class DrawView: public View {
    class TextShape *activeText;
    class TextView *activeTextView;
    Shape *currShape;
    int grid;
    Point lastClick;   // position of last click
    bool showGrid;
    ObjList *shapes, *selection;
    
public:
    MetaDef(DrawView);
    
    DrawView(class Document*, Point, ObjList*);
    ~DrawView();
    
    //---- initializing
    void SetShapes(ObjList*);
    ObjList *GetShapes()
	{ return shapes; }
    
    //---- drawing
    void Invalidate(ObjList *ol);
    void Draw(Rectangle);
    
    //---- shape list management
    void Insert(Shape *);
    void Remove(Shape *);
    void InsertShapes(ObjList*, bool tofront= TRUE);
    Shape *FindShape(Point);
    Shape *FindHandle(Point p, int *nh);
     
    //---- misc
    void Point2Grid(Point *);
    void ConstrainScroll(Point *);
    Point LastClick()
	{ return lastClick; }
    void SetTool(Object*);
    void RequestTool(int tix);
    void ShowInfo(TrackPhase tp, char *fmt, ...);

    //---- Selection
    int Selected()
	{ return selection->Size(); }
    Shape *OneSelected();
    Iterator *GetSelectionIter()
	{ return selection->MakeIterator(); }
    ObjList *GetCopyOfSelection();
    ObjList *GetDeepCopyOfSelection();
    ObjList *SetSelection(ObjList *newsel= 0);
    void SetDeleted(ObjList *l, bool b= TRUE);
    void RemoveDeleted();
    void ShowSelection();
    void SelectInRect(Rectangle r);
    Rectangle BoundingBox();
    bool HasSelection();
    
    //---- text
    void SetActiveText(class TextShape *tp);
    class TextShape *GetActiveText()
	{ return activeText; }
    
    //---- event handling
    Command *DoLeftButtonDownCommand(Point, Token, int);
    Command *DoKeyCommand(int, Point, Token);
    Command *DoCursorKeyCommand(EvtCursorDir, Point, Token);
    GrCursor GetCursor(Point lp);
    
    //---- menus
    void DoCreateMenu(class Menu*);
    void DoSetupMenu(class Menu*);
    Command *DoMenuCommand(int);
    
    //---- clipboard
    bool CanPaste(char *type);
    void SelectionToClipboard(char *type, ostream &os);
    Command *PasteData(char *type, istream &is);
};

#endif DrawView_First
