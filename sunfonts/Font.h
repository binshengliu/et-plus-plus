#define MAXCHARS 256

typedef unsigned char byte;

typedef struct {
    char *name;
    int width, swidth;
    int up, down, left, right;
    int nbytes, bytesperline;
    byte *bits;
    unsigned int attr;
} Glyph;

typedef struct {
    char *name, **props, **comments;
    int size, resx, resy;
    int nprops, ncomments;
    int ascent, descent, defaultchar;
    Glyph *glyphs[MAXCHARS];
} Font;

#define max(a,b) ((a) > (b) ? (a) : (b))

extern char *malloc();
extern Font *NewFont();
extern void DumpAsBdf();
extern Font *ReadBdf();
extern char *strsave();
extern void FreeFont();
extern void FreeGlyph();
extern char *adobename();
extern void RemoveGlyph();
extern void MaxExtent();

extern unsigned int mask[];

#define getbit(bp,bpl,x,y) ((bp)[(y)*(bpl) + ((x)>>3)] & mask[(x)&07])

#define putbit(bp,bpl,x,y) ((bp)[(y)*(bpl) + ((x)>>3)] |= mask[(x)&07])

#define Bytes(x) (((x)+7) >> 3)




