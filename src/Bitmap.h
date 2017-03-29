#ifndef Bitmap_First
#ifdef __GNUG__
#pragma once
#endif
#define Bitmap_First

#include "Port.h"
#include "Ink.h"

const int cPartPixelChanged= 12345;

class Bitmap : public Ink {
protected:
    class DevBitmap *dbm;
    
    void seedfill(int x, int y, u_int seed, u_int val);

public:
    MetaDef(Bitmap);

    Bitmap(Point sz, u_short depth= 1);
    Bitmap(Point sz, u_short *data, u_short depth= 1);
    Bitmap(int sz, u_short *data, u_short depth= 1);
    Bitmap(const char *filename);
    ~Bitmap();
    
    class DevBitmap *GetDevBitmap()
	{ return dbm; }
    void SetDevBitmap(DevBitmap*, bool clear_old= TRUE);

    void SetInk(Port *p);
    void SetPixel(u_int x, u_int y, u_int value);
    u_int GetPixel(u_int x, u_int y);
    void Fill(u_int val);
    void SeedFill(Point, u_int val);
    void Clear()
	{ Fill(0); }
    int GetDepth();
    Point Size();
    int ShortsPerLine();
    int BytesPerLine();
    int Shorts()
	{ return ShortsPerLine() * Size().y; }
    int Bytes()
	{ return Shorts() * 2; }
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

#endif Bitmap_First

