#ifndef DevBitmap_First
#ifdef __GNUG__
#pragma once
#endif
#define DevBitmap_First

#include "Root.h"
#include "Types.h"
#include "Point.h"

class DevBitmap : public Root {
public:
    Point size;
    u_short *image;
    u_short depth;
protected:
    DevBitmap(Point sz, u_short *data, u_short depth= 1);
public:
    virtual int BytesPerLine();
    virtual void SetPixel(u_int x, u_int y, int value);
    virtual int GetPixel(u_int x, u_int y);
    virtual void SetByte(u_int x, u_int y, u_int value);
    virtual u_int GetByte(u_int x, u_int y);
    virtual void GetColormapEntry(int x, int *r, int *g, int *b);
    virtual void SetColormapEntry(int x, int r, int g, int b);
    virtual int GetColormapSize();
    virtual void SetColormapSize(int);
    Point Size()
	{ return size; }
    int Depth()
	{ return depth; }
    int ShortsPerLine()
	{ return ((size.x-1)/16+1)*depth; }
};

#endif DevBitmap_First

