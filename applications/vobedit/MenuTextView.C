//$MenuTextView$
#include "MenuTextView.h"

//---- MenuTextView ----------------------------------------------------------

MetaImpl0(MenuTextView);

MenuTextView::MenuTextView(View *v, Rectangle r, Text *t) : StaticTextView(v,r,t)
{
}

MenuTextView::MenuTextView() : StaticTextView((View*)0, gRect0, (Text*)0)
{
}

void MenuTextView::Highlight(HighlightState)
{
}
