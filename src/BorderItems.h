#ifndef BorderItems_First
#ifdef __GNUG__
#pragma once
#endif
#define BorderItems_First

#include "VObject.h"
#include "CmdNo.h"

const int cBorderTitleGap= 4;
const int cHTitleGap    = 10;
const int cBorder       = 9,
	  cTitleGap     = 5;        // gap between titlebar and contents

//---- AbstractBorderItem ------------------------------------------------------

class AbstractBorderItem: public CompositeVObject {
protected:
    Point interiorOffset, extentDiff, border;
    VObject *interior, *titleBar;
    short lineWidth, titleGap, shadowWidth;
    
public:
    MetaDef(AbstractBorderItem);
    
    AbstractBorderItem(VObject *title, VObject *inner, int lw= 1, int id= cIdNone);
    AbstractBorderItem(char *title, VObject *inner, int lw= 1, int id= cIdNone);

    VObject* GetTitleBar()
	{ return titleBar; }
    VObject *GetInterior()
	{ return interior; }
    void SetInterior(VObject *invob)
	{ interior= invob; }
    Point GetInteriorOffset()
	{ return interiorOffset; }
    short GetShadowWidth()
	{ return shadowWidth; }
    short GetLineWidth()
	{ return lineWidth; }
    void SetShadowWidth(int sw)
	{ shadowWidth= sw; }
    void SetLineWidth(int lw)
	{ lineWidth= lw; }
    Point GetBorder()
	{ return border; }
    void SetBorder(Point b)
	{ border= b; }

    void SetTitle(char *itsTitle, bool redraw);
    char *GetTitle();
    void SetOrigin(Point at);
    void SetExtent(Point e);
    Metric GetMinSize();
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &);
    void InspectorId(char *buf, int sz);
};

//---- BorderItem --------------------------------------------------------------

class BorderItem: public AbstractBorderItem {
    VObjAlign titleAlign;
public:
    MetaDef(BorderItem);
    BorderItem(VObject *inner, Point b= gBorder, int lw= 1, VObjAlign a= eVObjHLeft);
    BorderItem(VObject *title, VObject *inner, Point b= gBorder, int lw= 1, VObjAlign a= eVObjHLeft);
    BorderItem(char *title, VObject *inner, Point b= gBorder, int lw= 1, VObjAlign a= eVObjHLeft);
    BorderItem(int id, char *title, VObject *inner, Point b= gBorder, int lw= 1, VObjAlign a= eVObjHLeft);

    void SetOrigin(Point at);
    void SetExtent(Point e);
    int Base();
    Metric GetMinSize();
    void Draw(Rectangle);
    ostream& PrintOn(ostream&s);
    istream& ReadFrom(istream &);
};

#endif BorderItems_First

