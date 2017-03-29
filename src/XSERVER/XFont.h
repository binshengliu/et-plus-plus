#ifndef XServerFont_First
#define XServerFont_First

#include "X11.h"
#include "Font.h"

class XServerFont: public Font {
    XFont xfid;
    char *fname;
    XFontStruct *xFont;
    
public:
    XServerFont(char *name, GrFont, int, GrFace);
    ~XServerFont();
    
    bool LoadFont();
    bool Loaded();
    XFont GetId()
	{ return xfid; }
    DevBitmap *CharAsBitmap(byte c, Point *offset);
};

class XFontManager: public FontManager {
public:
    XFontManager(char *lib) : FontManager(lib)
	{ }
    Font* MakeFont(char*, GrFont, int, GrFace);
};

#endif XServerFont_First
