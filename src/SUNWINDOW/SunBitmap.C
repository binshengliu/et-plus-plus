#include "SunBitmap.h"
#include "Error.h"
#include "Ink.h"
#include "String.h"
#include "SunWindowSystem.h"
#include "SunWindowPort.h"

#include "pixrect.h"

extern bool NewPixrect(pixrect **prp, Point e, bool exact, bool clear, int depth);

SunBitmap::SunBitmap(Point sz, u_short *im, u_short dep) : DevBitmap(sz, im, dep)
{
    pr= 0;
    cmap= 0;
    cmseq= 0;
    freepr= TRUE;
    if (im)
	pr= mem_point(sz.x, sz.y, dep, (short*) im);
    spr= dpr= pr;
}

SunBitmap::SunBitmap(pixrect *npr, colormap_t *c, bool f) : DevBitmap(gPoint0, 0, 0)
{
    dpr= spr= pr= npr;
    cmap= 0;
    cmseq= 0;
    freepr= f;
    if (pr) {
	cmap= c;
	size.x= pr->pr_size.x;
	size.y= pr->pr_size.y;
	depth= pr->pr_depth;
    }
}

SunBitmap::~SunBitmap()
{
    if (freepr && pr)
	pr_destroy(pr);
    if (spr != pr)
	pr_destroy(spr);
    if (dpr != spr && dpr != pr)
	pr_destroy(dpr);
    SafeDelete(cmap);
    pr= spr= dpr= 0;
}

pixrect *SunBitmap::Expand(Point e)
{
    e.x= ((e.x >> 5)+1) << 5;
    e.y= ((e.y >> 5)+1) << 5;
    
    if (spr == pr)
	spr= 0;
    if (NewPixrect(&spr, e, FALSE, FALSE, pr->pr_depth))
	pr_replrop(spr, 0, 0, e.x, e.y, PIX_SRC | PIX_DONTCLIP, pr, 0, 0);
    return dpr= spr;
}

int SunBitmap::GetColormapSize()
{
    if (cmap)
	return cmap->length;
    return DevBitmap::GetColormapSize();
}

void SunBitmap::SetColormapSize(int sz)
{
    if (cmap == 0)
	cmap= new colormap_t;
    if (cmap->length < sz)
	for (int i= 0; i < 3; i++)
	    cmap->map[i]= (u_char*) Realloc(cmap->map[i], sz);
    cmap->length= sz;
}

void SunBitmap::SetColormapEntry(int ix, int r, int g, int b)
{
    SetColormapSize(ix+1);
    cmap->map[0][ix]= r;
    cmap->map[1][ix]= g;
    cmap->map[2][ix]= b;
}

void SunBitmap::GetColormapEntry(int ix, int *r, int *g, int *b)
{
    if (cmap) {
	*r= cmap->map[0][ix];
	*g= cmap->map[1][ix];
	*b= cmap->map[2][ix];
    } else
	DevBitmap::GetColormapEntry(ix, r, g, b);
}

void SunBitmap::createpixrect()
{
    spr= dpr= pr= mem_create(size.x, size.y, depth);
    pr_rop(pr, 0, 0, size.x, size.y, PIX_CLR, 0, 0, 0);
}

void SunBitmap::SetPixel(u_int x, u_int y, int value)
{
    CreatePixrect();
    pr_put(pr, x, y, value);
}

int SunBitmap::GetPixel(u_int x, u_int y)
{
    CreatePixrect();
    return pr_get(pr, x, y);
}

void SunBitmap::SetByte(u_int x, u_int y, u_int value)
{
    CreatePixrect();
    ((char*)pr->pr_data->md_image)[y*pr->pr_data->md_linebytes+x]= value;
}

u_int SunBitmap::GetByte(u_int x, u_int y)
{
    CreatePixrect();
    return ((char*)pr->pr_data->md_image)[y*pr->pr_data->md_linebytes+x];
}

void SunBitmap::MapColors(SunWindowPort *p, suncolormap *cm, int cmseq2)
{
    if (pr && pr->pr_depth > 1 && cmap) {
	if (p->HasColor()) {
	    register u_char *from, *to;
	    register int i;
	    int l= cmap->length;
	    u_int tmap[256];
	    colormap_t *ct= cmap;
	    
	    for (i= 0; i < l; i++)
		tmap[i]= cm->mapcolor(ct->map[0][i], ct->map[1][i], ct->map[2][i]);
    
	    if (dpr == 0)
		dpr= mem_create(size.x, size.y, pr->pr_depth);
	    
	    from= (u_char*) pr->pr_data->md_image;
	    to= (u_char*) dpr->pr_data->md_image;
	    l= size.x * size.y;
	    for (i= 0; i < l; i++)
		*to++= tmap[*from++];
	} else
	    dither();
	cmseq= cmseq2;
    }
}


//---- dither ------------------------------------------------------------------

static int tab[] = {
    255,
    0
};
static int tabsize= 2;


static int FindColor(int v, int *error) 
{
    int dist, mindist, minx;
	
    minx= 0;
    mindist= v-tab[minx];
    for (int i= 1; i < tabsize; i++) {
	dist= v-tab[i];
	if (abs(dist) < abs(mindist)) {
	    minx= i;
	    mindist= dist;
	}
    }
    *error= mindist;
    return minx;
}

void SunBitmap::dither()
{
    register int *next, i, x, y, incr, lasterror, *grey= 0;
    int error;

    if (depth <= 1)
	return;
	
    dpr= mem_create(size.x, size.y, 1);
	
    if (depth <= 16) {
	grey= new int[cmap->length];
	for (i= 0; i < cmap->length; i++) {
	    int r, g, b;
	    GetColormapEntry(i, &r, &g, &b);
	    RGBColor c(r, g, b);
	    grey[i]= c.AsGreyLevel();
	}
    }

    next= new int[size.x + 2];
    next++;

    for (y= 0; y < size.y; y++) {
	if (ODD(y)) {   // scan left
	    incr= 1;
	    i= size.x-1;
	} else {        // scan right
	    incr= -1;
	    i= 0;
	}
	for (lasterror= x= 0; x < size.x; x++, i-= incr) {
	    int v= grey[pr_get(spr, i, y)];
	    pr_put(dpr, i, y, FindColor(lasterror + v, &error));
	    lasterror= next[i-incr] + error * 7 / 16;
	    next[i-incr] = error * 1 / 16;
	    next[i]     += error * 5 / 16;
	    next[i+incr]+= error * 3 / 16;
	}
    }

    delete --next;
    if (grey)
	delete grey;
}

void SunBitmap::DevScaleBitmap(int sx, int sy)
{
    struct pixrect *pr1, *pr2;
    register float scale;
    int x= size.x, y= size.y;
    int src, dst, d;
    
    if (sx == x && sy == y)
	return;     // use unscaled bitmap

    if (spr->pr_size.x == sx && spr->pr_size.y == sy)
	return;

    if (spr)
	pr_destroy(spr);
    if (dpr != spr && dpr != pr)
	pr_destroy(dpr);
    
    if (sx == x)            // same width
	pr1= pr;
    else if (sx < x) {      // x-shrink
	pr1= mem_create(sx, y, depth);
	scale= (float)x/(float)sx;
	for (dst= 0; dst < sx; dst++)
	    pr_rop(pr1, dst, 0, 1, y, PIX_SRC|PIX_DONTCLIP, pr,
						(int)(dst*scale+0.5), 0);
    } else {                // x-magnify
	pr1= mem_create(sx, y, depth);
	d= sx/x;
	scale= (float)sx/(float)x;
	for (src= 0; src < x; src++)    // slice
	    pr_rop(pr1, (int)(src*scale+0.5), 0, 1, y, PIX_SRC|PIX_DONTCLIP, pr, src, 0);
	for (src= 0; src < d; src++)    // smear
	    pr_rop(pr1, 1, 0, sx-1, y, PIX_OR|PIX_DONTCLIP, pr1, 0, 0);
    }
    
    if (sy < y) {           // y-shrink
	pr2= mem_create(sx, sy, depth);
	scale= (float)y/(float)sy;
	for (dst= 0; dst < sy; dst++)
	    pr_rop(pr2, 0, dst, sx, 1, PIX_SRC|PIX_DONTCLIP, pr1, 0, (int)(dst*scale+0.5));
	if (sx != x)
	    pr_destroy(pr1);
	pr1= pr2;
    } else if (sy > y) {    // y-magnify
	pr2= mem_create(sx, sy, depth);
	d= sy/y;
	scale= (float)sy/(float)y;
	for (src= 0; src < y; src++)    // slice
	    pr_rop(pr2, 0, (int)(src*scale+0.5), sx, 1, PIX_SRC|PIX_DONTCLIP, pr1, 0, src);
	for (src= 0; src < d; src++)    // smear
	    pr_rop(pr2, 0, 1, sx, sy-1, PIX_OR|PIX_DONTCLIP, pr2, 0, 0);
	if (sx != x)
	    pr_destroy(pr1);
	pr1= pr2;
    }
    dpr= spr= pr1;
}
