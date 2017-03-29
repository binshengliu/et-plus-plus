#ifndef NeWSServerFont_First
#define NeWSServerFont_First

#include "Font.h"

class NeWSServerFont: public Font {
    int id;
    
public:
    NeWSServerFont(char*, GrFont, int, GrFace);
    ~NeWSServerFont();
    
    bool LoadFont();
    bool Loaded();
    int GetId()
	{ return id; }
    Font *MakeFont(Font**, GrFace fc);
};

class NeWSFontManager: public FontManager {
public:
    NeWSFontManager(char *lib) : (lib)
	{ }
    Font* MakeFont(char*, GrFont, int, GrFace);
};

#endif NeWSServerFont_First

