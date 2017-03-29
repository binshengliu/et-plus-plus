#ifndef MenuTextView_First
#define MenuTextView_First

#include "StaticTView.h"

//---- MenuTextView ----------------------------------------------------------

class MenuTextView: public StaticTextView {
public:
    MetaDef(MenuTextView);
    MenuTextView(View *v, Rectangle r, Text *t);
    MenuTextView();
    void Highlight(HighlightState);
};

#endif MenuTextView_First

