//$Bitmap$
#include <stdio.h>

#include "Bitmap.h"
#include "DevBitmap.h"
#include "Types.h"
#include "Error.h"
#include "Storage.h"
#include "WindowSystem.h"
#include "String.h"

MetaImpl0(Bitmap);

static void encode(DevBitmap*, ostream&, int, int, int*);
static void decode(DevBitmap*, istream&, int, int);

extern void InitWindowSystem();

Bitmap::Bitmap(Point sz, u_short dep)
{
    InitWindowSystem();
    dbm= gWindowSystem->MakeDevBitmap(sz, 0, dep);
}

Bitmap::Bitmap(Point sz, u_short *im, u_short dep)
{    
    InitWindowSystem();
    dbm= gWindowSystem->MakeDevBitmap(sz, im, dep);
}

Bitmap::Bitmap(int sz, u_short *im, u_short dep)
{    
    InitWindowSystem();
    dbm= gWindowSystem->MakeDevBitmap(sz, im, dep);
}

Bitmap::Bitmap(const char *name)
{
    dbm= gWindowSystem->LoadDevBitmap(name);
    if (dbm == 0)
	Error("Bitmap", "can't load %s", name);
}

Bitmap::~Bitmap()
{
    SafeDelete(dbm);
}

Point Bitmap::Size()
{
    return dbm->Size();
}

int Bitmap::ShortsPerLine()
{
    return ((dbm->Size().x-1)/16+1)*dbm->Depth();
}

int Bitmap::BytesPerLine()
{
    return dbm->BytesPerLine();
}

int Bitmap::GetDepth()
{
    return dbm->Depth();
}

void Bitmap::SetDevBitmap(DevBitmap *d, bool clear_old)
{
    if (clear_old && dbm)
	delete dbm;
    dbm= d;
}

void Bitmap::Fill(u_int val)
{
    Point sz= Size();
    for (int x= 0; x < sz.x; x++)
	for (int y= 0; y < sz.y; y++)
	    dbm->SetPixel(x, y, val);
    Changed();
}
	   
void Bitmap::seedfill(int x, int y, u_int seed, u_int val)
{
    if (GetPixel(x, y) == seed) {
	dbm->SetPixel(x, y, val);
	seedfill(x-1, y, seed, val);
	seedfill(x+1, y, seed, val);
	seedfill(x, y+1, seed, val);
	seedfill(x, y-1, seed, val);
    }
}

void Bitmap::SeedFill(Point where, u_int val)
{
    u_int oldval= GetPixel(where.x, where.y);
    if (val == oldval)    // prevent endless recursion
	seedfill(where.x, where.y, oldval, oldval= (val+1) % 256);
    seedfill(where.x, where.y, oldval, val);
    Changed();
}

void Bitmap::SetInk(Port *port)
{
    port->DevSetPattern(dbm);
}

void Bitmap::SetPixel(u_int x, u_int y, u_int val)
{
    if (x >= 0 && x < dbm->Size().x && y >= 0 && y < dbm->Size().y) {
	dbm->SetPixel(x, y, val);
	Rectangle r(x, y, 1, 1);
	Send(cIdNone, cPartPixelChanged, &r);
    }
}

u_int Bitmap::GetPixel(u_int x, u_int y)
{
    if (x >= 0 && x < dbm->Size().x && y >= 0 && y < dbm->Size().y)
	return dbm->GetPixel(x, y);
    return 0;
}

ostream &Bitmap::PrintOn(ostream &s)
{
    register int i;
    int col= 0, enc= Bytes() > 100 ? 1 : 0;
    Point size= Size();
    int cmsize= dbm->GetColormapSize();
    int bpl= BytesPerLine();
    
    Object::PrintOn(s);
    
    s << size SP << GetDepth() SP << enc SP << cmsize NL;
    
    if (cmsize > 0) {
	for (i= 0; i < cmsize; i++) {
	    int r, g, b;
	    dbm->GetColormapEntry(i, &r, &g, &b);
	    s << r SP << g SP << b NL;
	}
    }
    
    if (enc)
	encode(dbm, s, bpl, size.y, &col);
    else {
	register int x, y;
	
	for (y= 0; y < size.y; y++)
	    for (x= 0; x < bpl; x++)
		PutHex(s, dbm->GetByte(x, y), &col);
    }
    if (col > 0)
	s NL;
    return s;
}

istream &Bitmap::ReadFrom(istream &s)
{
    int decd, depth, cmsize, bpl;
    Point size;
    
    SafeDelete(dbm);
	
    Object::ReadFrom(s);
    s >> size >> depth >> decd >> cmsize;
    
    dbm= gWindowSystem->MakeDevBitmap(size, 0, depth);
    
    if (cmsize > 0) {
	dbm->SetColormapSize(cmsize);
	for (int i= 0; i < cmsize; i++) {
	    int r, g, b;
	    s >> r >> g >> b;
	    dbm->SetColormapEntry(i, r, g, b);
	}
    }

    bpl= BytesPerLine();
    if (decd)
	decode(dbm, s, bpl, size.y);
    else {
	register int x, y;
	
	for (y= 0; y < size.y; y++)
	    for (x= 0; x < bpl; x++)
		dbm->SetByte(x, y, GetHex(s));
    }

    return s;
}

//---- private stuff -----------------------------------------------------------

static void encode(DevBitmap *dbm, ostream &ofp, int bpl, int h, int *col)
{
    register int i, j, k, b;
    int start, l= bpl*h;

    for (i= 0; i < l;) {
	b= dbm->GetByte(i%bpl, i/bpl);
	for (i++, j= 1; i < l; i++, j++)
	    if (b != dbm->GetByte(i%bpl, i/bpl))
		break;
	if (j>1) {
	    while (j>128) {
		PutHex(ofp, 128-1, col);
		PutHex(ofp, b, col);
		j-= 128;
	    }
	    PutHex(ofp, j-1, col);
	    PutHex(ofp, b, col);
	} else 
	    i--;

	if (i>= l)
	    break;

	start= i;
	for (i++, j= 1; i < l; i++, j++)
	    if (dbm->GetByte((i-1)%bpl, (i-1)/bpl) == dbm->GetByte(i%bpl, i/bpl))
		break;
	if (i<l) {
	    i--;
	    j--;
	}
	if (j>0) {
	    while (j>128) {
		PutHex(ofp, 128+127, col);
		for (k= 0; k<128; k++, start++)
		    PutHex(ofp, dbm->GetByte(start%bpl, start/bpl), col);
		j-= 128;
	    }
	    PutHex(ofp, j+127, col);
	    for (; j > 0; j--, start++)
		PutHex(ofp, dbm->GetByte(start%bpl, start/bpl), col);
	}
    }
}

static void decode(DevBitmap *dbm, istream &in, int bpl, int h)
{
    register int count, value, i, j, l= bpl*h;
    
    for (i= 0; i < l;) {
	count= GetHex(in);
	if (count >= 128) {
	    count-= 127;
	    for (j= 0; j < count; j++) {
		dbm->SetByte(i%bpl, i/bpl, GetHex(in));
		i++;
	    }
	} else {
	    count++;
	    value= GetHex(in);
	    for (j= 0; j < count; j++) {
		dbm->SetByte(i%bpl, i/bpl, value);
		i++;
	    }
	}
    }
}

