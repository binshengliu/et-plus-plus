//$DevBitmap$
#include "DevBitmap.h"
#include "Ink.h"

DevBitmap::DevBitmap(Point sz, u_short *data, u_short dep)
{
    size= sz;
    image= data;
    depth= dep;
}

int DevBitmap::BytesPerLine()
{
    return ((size.x-1)/8+1)*depth;
}

int DevBitmap::GetColormapSize()
{
    return 0;
}

void DevBitmap::SetColormapSize(int)
{
}

void DevBitmap::GetColormapEntry(int ix, int *r, int *g, int *b)
{
}

void DevBitmap::SetColormapEntry(int ix, int r, int g, int b)
{
}

void DevBitmap::SetPixel(u_int x, u_int y, int value)
{
}

int DevBitmap::GetPixel(u_int, u_int)
{
    return 0;
}

void DevBitmap::SetByte(u_int x, u_int y, u_int value)
{
    if (depth == 8)
	SetPixel(x, y, value);
    else {
	register int b= 0x80, i;
	x*= 8;
	for (i= 0; i < 8; i++) {
	    SetPixel(x+i, y, (value & b) ? 1 : 0);
	    b >>= 1;
	}
    }
}

u_int DevBitmap::GetByte(u_int x, u_int y)
{
    if (x < 0 || y < 0 || y >= size.y)
	return 0;

    if (depth == 8)
	return GetPixel(x, y);
	
    register int b= 0, i;
    x*= 8;
    for (i= 0; i < 8; i++) {
	b <<= 1;
	if (GetPixel(x+i, y))
	    b|= 1;
    }
    return b;
}

