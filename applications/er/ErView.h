#ifndef ErView_First
#define ErView_First

#include "View.h"

//---- ErView ------------------------------------------------------------------

class ErView: public View {
    class ErDocument *doc;
    class ErShape *focus;

public:
    MetaDef(ErView);
    
    ErView(ErDocument *d, Point ext);

    void Draw(Rectangle r);
    class ErShape *FindShape(Point p);
    
    //---- event handling
    Command *DispatchEvents(Point p, Token t, Clipper *vf);
    Command *DoLeftButtonDownCommand(Point, Token, int);
};

#endif ErView_First

