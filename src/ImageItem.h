#ifndef ImageItem_First
#ifdef __GNUG__
#pragma once
#endif
#define ImageItem_First

#include "VObject.h"

//---- ImageItem ---------------------------------------------------------------

class ImageItem : public VObject {
protected:
    bool deleteBitmap;
    Bitmap *bmp;
    short base;
public:
    MetaDef(ImageItem);

    ImageItem(Bitmap *b, int base= 0);
    ImageItem(short*, Point, int base= 0);
    ImageItem(int id, Bitmap *b, int base= 0);
    ~ImageItem();

    void SetBitmap(Bitmap *bm, bool redraw= TRUE);
    Bitmap *GetBitmap()
	{ return bmp; }
    Metric GetMinSize();
    int Base();
    void SetBase(int b)
	{ base= b; }
    void Draw(Rectangle);
    void DoObserve(int, int part, void*, Object *op);
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif ImageItem_First

