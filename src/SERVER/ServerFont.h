#ifndef ServerFont_First
#define ServerFont_First

#include "Font.h"

class ServerFont: public Font {
    int id;
    
public:
    ServerFont(char*, GrFont, int, GrFace);
    ~ServerFont();
    
    bool Loaded();
    int GetId()
	{ return id; }
    bool LoadFont();
    Font *MakeFont(Font**, GrFace fc);
};

class ServerFontManager: public FontManager {
public:
    ServerFontManager(char *lib) : (lib)
	{ }
    Font* MakeFont(char*, GrFont, int, GrFace);
};

#endif ServerFont_First

