#ifndef SunBitmap_First
#define SunBitmap_First

#include "DevBitmap.h"

class SunBitmap: public DevBitmap {
    struct pixrect *pr, *dpr;
    int cmseq;
    bool freepr;
public:
    struct colormap_t *cmap;
    struct pixrect *spr;
    
    void createpixrect();
    void CreatePixrect()
	{ if (pr == 0) createpixrect(); }
public:
    SunBitmap(Point sz, u_short *data, u_short depth);
    SunBitmap(pixrect *npr, colormap_t *c, bool freepr= TRUE);
    ~SunBitmap();

    void MapColors(class SunWindowPort*, struct suncolormap *cm, int cmseq);
    int GetColormapSize();
    void SetColormapSize(int sz);
    void GetColormapEntry(int ix, int *r, int *g, int *b);
    void SetColormapEntry(int ix, int r, int g, int b);
    void SetPixel(u_int x, u_int y, int value);
    int GetPixel(u_int x, u_int y);
    void SetByte(u_int x, u_int y, u_int value);
    u_int GetByte(u_int x, u_int y);
    void DevScaleBitmap(int sx, int sy);
    struct pixrect *Pr()
	{ return dpr; }
    struct pixrect *Expand(Point e);
    int GetCmapSeq()
	{ return cmseq; }
    void dither();
};

#endif SunBitmap_First


