#ifndef MickyView_First
#define MickyView_First

#include "View.h"

//---- MickyView -------------------------------------------------------------------

class MickyView: public View { 
public:
    MetaDef(MickyView);
    MickyView(Rectangle itsExtent);
    MickyView();
    void Draw(Rectangle);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
    Metric GetMinSize();
};

#endif MickyView_First

