#ifndef PictPort_First
#ifdef __GNUG__
#pragma once
#endif
#define PictPort_First

#include "PrintPort.h"

class PictPort: public PrintPort {    
protected:
    class Picture *pict;
    
public:
    MetaDef(PictPort);
    PictPort(Picture*);
    ~PictPort();
    
    Picture *GetPicture()
	{ return pict; }
    void DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point);
    bool DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, Point pos);
    void DrawObject(char, Rectangle*, Point, int, GrLineCap);
    void DevShowTextBatch(Rectangle *r, Point);
    void DevShowBitmap(Rectangle*, class Bitmap*);
    void DevSetPattern(class DevBitmap*);
    bool DevSetColor(class RGBColor*);
    void DevSetOther(int);
};

#endif PictPort_First
