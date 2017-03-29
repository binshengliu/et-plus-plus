#include "Port.h"
#include "NeWSBitmap.h"
#include "NeWSWindowSystem.h"

#include "../SUNWINDOW/SunBitmap.h"

#include "psio.h"
#include "news.h"

NeWSBitmap::NeWSBitmap(Point sz, u_short *data, u_short dep)
{ 
    size= sz;
    image= im;
    depth= dep;
    id= -1;
}

NeWSBitmap::NeWSBitmap(const char *name)
{
    
    struct pixrect *pr= sunbitmap_load((char*) name);
    if (pr) {
	size= sunbitmap_size(pr);
	image= sunbitmap_image(pr);
    } else
	cerr << "can't load bitmap\n";
    id= -1;
}

NeWSBitmap::~NeWSBitmap()
{
}

int NeWSBitmap::Download()
{
    char buf[10000], *dd= (char*)GetImage();;
    int i, j, k=0, bpl= BytesPerLine(), bb= ShortsPerLine()*2;
    
    for(i=0; i<size.y; i++)
	for(j=0; j<bpl; j++)
	    buf[k++]= dd[i*bb+j];
    ps_mkbitmap(size.x, size.y, buf, bpl*size.y, &id);
    return id;
}

