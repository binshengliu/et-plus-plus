#ifndef font_First
#define font_First

#include "dev.h"
#include "Font.h"

#define MAXFONTS 20
#define BMASK 0377

class Fontdesc : public Object {
    struct font *fontbase;
    char *fitab;
    char *codetab;
    char *path;
    GrFont fid;
    GrFace face;
    
public:
    MetaDef(Fontdesc);
    Fontdesc(font *f, char *path= 0);
    
    void Init(char *nm);
    struct font *Base()
	{ return fontbase; }
    GrFont Fid()
	{ return fid; }
    GrFace Face()
	{ return face; }
    int Fit(int c)
	{ return fitab[c] & BMASK; }
    int Code(int c)
	{ return codetab[c] & BMASK; }
    char *Name()
	{ return fontbase->namefont; }
    char *Path()
	{ return path; }
    bool Special()
	{ return fontbase->specfont; }
    int Nw()
	{ return (int)((unsigned) fontbase->nwfont & BMASK); }
    bool Isequal(char *name, char *path);
};

class TroffFontManager : public Object {
public:
    struct dev dev;         // typesetter information
    char *chname;           // list of funny char names
    short *chtab;           // indices into funny character array chname
    bool sorted;            // list of funny characters (chtab) is sorted
    char *DevName;
    class SeqCollection *flist;

public:
    MetaDef(TroffFontManager);
    TroffFontManager(char *devname);
    int Lookup(char *s);
    Fontdesc *Load(char *s, char *s1);
    int FontSize(int nw)
	{ return sizeof(struct font) + 3 * nw + dev.nchtab + 128 - 32; }
};

class Fontmap {
    Fontdesc *map[MAXFONTS+1];
    int smnt;               // index of first special font
public:
    Fontmap()
	{ }
    void Load(int, char*, char*); // load font info for font s on position n (0...)
    Fontdesc* Lookup(int &c, char *s, int fo);
};

extern TroffFontManager *tfm;

#endif font_First
