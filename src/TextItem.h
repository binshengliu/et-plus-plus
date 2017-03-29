#ifndef TextItem_First
#ifdef __GNUG__
#pragma once
#endif
#define TextItem_First

#include "VObject.h"

//---- TextItem ----------------------------------------------------------------

extern Point gBorder;

class TextItem : public VObject {
protected:
    char *text;
    FontPtr font;
    Point border;

public:
    MetaDef(TextItem);

    TextItem(char *t, FontPtr f= gSysFont, Point b= gBorder);
    TextItem(int id, char *t, FontPtr f= gSysFont, Point b= gBorder);
    ~TextItem();

    void SetString(char*, bool redraw= FALSE);
    void SetFString(bool redraw, char *fmt, ...);
    void SetBorder(Point b)
	{ border= b; }
    int Base();
    Metric GetMinSize();
    void SetExtent(Point e);
    FontPtr GetFont()
	{ return font; }
    void SetFont(FontPtr fp);
    void Draw(Rectangle);
    char *AsString();
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
    void InspectorId(char *buf, int bufSize);
};

#endif TextItem_First

