#ifndef Picture_First
#ifdef __GNUG__
#pragma once
#endif
#define Picture_First

#include "Port.h"
#include "Object.h"

extern byte *bufp;

class Picture: public Object {
    byte *buf, *bp;
    int length, bmcnt, ftcnt;
    class ObjArray *bms;
    class Font *fts[30];
public:
    Rectangle bbox;

public:
    MetaDef(Picture);
    Picture();
    Picture(char *filename);
    ~Picture();

    void Close();
    void Show(Rectangle *r, Port *port);
    
    void StrokeLine(int, GrLineCap, Point, Point);
    void PutGlyph(int c, Point pos, Font *fp);
    void DrawObject(char, Rectangle*, Point, int, GrLineCap);
    void PutBitmap(Rectangle *r, Bitmap *bmp);
    void PutPat(int pid);
    void Merge(Rectangle *r)
	{ bbox.Merge(*r); }
    int Length()
	{ return length; }
    ostream &PrintOn(ostream &os);
    istream &ReadFrom(istream &is);
    
protected:
    void Init();
    byte GetByte()
	{ return *bp++; }
    char GetChar()
	{ return (char) *bp++; }
    short GetShort();
    Point GetPoint();

    void PutByte(byte b)
	{ *bufp++= b; }
    void PutCode(int c)    
	{ PutByte(c); }
    void PutChar(char c)
	{ *bufp++= c; }
    void PutShort(short s)
	{ PutByte(s >> 8); PutByte(s & 0xff); }
    void PutPoint(Point p)
	{ PutShort(p.x); PutShort(p.y); }
    void PutExtent(Point e);
    void Goto(Point pos);
    void PutFont(Font *fp);
    void PutSize(int s);
    void PutCap(GrLineCap cap);
    void PutAngle(int s, int e);
    void PutDia(Point e);
};

#endif Picture_First
