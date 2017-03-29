#ifndef SunFont_First
#define SunFont_First

#include "Font.h"

class SunFont: public Font {
friend class SunWindowPort;
    struct pixrect *notdef;     // private data
    struct pixfont *pf;         // private data
    char *vfontname;            // name of v-font file
    bool loaded;                // all pixrects are in memory
public:
    
    SunFont(char*, GrFont, int, GrFace);
    ~SunFont();
    
    bool Loaded();
    bool LoadFont();
    Font *MakeFont(Font**, GrFace);
    void ScaleFont(Font*);
    void MakeChar(Font*, GrFace, byte, Font*);
    DevBitmap *CharAsBitmap(byte c, Point *offset);
};

class SunFontManager: public FontManager {
public:
    SunFontManager(char *lib);
    Font *MakeFont(char*, GrFont, int, GrFace);
    Font *ScaleFont(Font *ofp, GrFont fid, int ps, GrFace face);
};

#endif SunFont_First

