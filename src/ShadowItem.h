#ifndef ShadowItem_First
#ifdef __GNUG__
#pragma once
#endif
#define ShadowItem_First

#include "VObject.h"

//---- ShadowItem --------------------------------------------------------------

class ShadowItem: public CompositeVObject {
    int shadowWidth, lineWidth;
    VObject *inner;
public:
    MetaDef(ShadowItem);
    ShadowItem(int id, VObject *in, int sw= 2, int lw= 1);
    Metric GetMinSize();
    int Base();
    void SetOrigin(Point at);
    void SetExtent(Point e);
    void SetShadowWidth(int sw)
	{ shadowWidth= sw; }
    void SetLineWidth(int lw)
	{ lineWidth= lw; }
    void DrawHighlight(Rectangle r);
    void DrawInner(Rectangle r, bool highlight);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
};

#endif ShadowItem_First

