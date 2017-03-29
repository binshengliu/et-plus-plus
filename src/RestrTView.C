//$RestrTextView$

#include "RestrTView.h"
#include "RegularExp.h"
#include "BlankWin.h"

MetaImpl(RestrTextView, (TP(rex), 0));

RestrTextView::RestrTextView(EvtHandler *eh, RegularExp *rx, Rectangle r, Text *t,
	    eTextJust m, eSpacing sp, bool w, TextViewFlags fl, Point b, int id)
				    : TextView(eh, r, t, m, sp, w, fl, b, id) 
{
    rex= rx;
}
   
bool RestrTextView::Match(class Text* t, RegularExp *rex)
{
    int n, size= t->Size();
    if (size == 0)  { // zero length text matches all patterns
	delete t;
	return TRUE;
    }
    int pos= t->Search(rex, &n, 0, size);
    delete t;
    return (pos == 0 && n == size);
}

void RestrTextView::NotAccepted ()
{
    gWindow->Bell(20);
}

void RestrTextView::SetRegExp(RegularExp* r)
{
    rex= r;
}

void RestrTextView::Cut() 
{
    int from, to;
    
    GetSelection(&from, &to);
    Text *tmp= CopyText();
    tmp->Cut(from,to);
    if (!Match(tmp,rex)) 
	NotAccepted();
    else
	TextView::Cut();
}          

void RestrTextView::Paste(Text *insert)
{
    int from, to;
    
    GetSelection(&from, &to);
    Text *tmp= CopyText();
    tmp->Paste(insert, from, to);
    if (!Match(tmp, rex)) 
	NotAccepted();
    else
	TextView::Paste(insert);
}

Text *RestrTextView::CopyText() 
{
    Text *ct= 0, *t= GetText();
    
    if (t) {
	t->RemoveObserver(this);
	ct= (Text*) t->DeepClone();
	t->AddObserver(this);
    }
    return ct;   
}

ostream& RestrTextView::PrintOn(ostream &s)
{
    TextView::PrintOn(s);
    return s << rex SP;
}

istream& RestrTextView::ReadFrom(istream &s)
{
    TextView::ReadFrom(s);
    s >> rex;
    return s;
}

