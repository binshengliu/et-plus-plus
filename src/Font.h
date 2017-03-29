#ifndef Font_First
#ifdef __GNUG__
#pragma once
#endif
#define Font_First

#include "Root.h"
#include "Rectangle.h"

#define MINSIZE     6
#define MAXSIZE     24
#define SIZES       (MAXSIZE-MINSIZE+1)

enum GrFont {
    eFontTimes,
    eFontHelvetica,
    eFontCourier,
    eFontSymbol,
    eFontChicago,
    eFontAvantgarde,
    eFontBookman,
    eFontSchoolbook,
    eFontGacha,
    eFontGeneva,
    eFontMonaco,
    eFontNHelvetica,
    eFontPalatino,
    eFontChancery,
    eFontDingbats,
    eFontAthens,
    eFontCairo,
    eFontLondon,
    eFontLosAngeles,
    eFontMobile,
    eFontNewYork,
    eFontSanFrancisco,
    eFontVenice,
    
    eFontDefault=eFontTimes
};

enum GrFace {
    eFacePlain      = 0,
    eFaceBold       = BIT(0),
    eFaceItalic     = BIT(1),
    eFaceUnderline  = BIT(2),
    eFaceOutline    = BIT(3),
    eFaceShadow     = BIT(4),
};

enum GrHAdjust {
    eAdjHLeft,
    eAdjHCenter,
    eAdjHRight
};

enum GrVAdjust {
    eAdjVBottom,
    eAdjVTop,
    eAdjVBase,
    eAdjVCenter
};

class Font;

typedef Font *FontPtr;

class Font : public Root {
protected:
    u_short cw[256];        // character widths
    GrFont family;          // family id
    GrFace face;            // style
    bool fixed;             // is a fixed character font
    short size;             // point size
    short ils;              // interline spacing
    short ht;               // maximum ascender
    short bs;               // maximum descender
    short width;            // max width
    
public:
    
    friend Font *new_Font(GrFont fid, int size, GrFace face= eFacePlain);

    int Width(byte c)
	{ return cw[c]; }
    int Width(byte*, int l= -1);
    int MaxWidth(int l= 1);
    Metric GetMetric(byte *s, int l= -1);
    Metric GetMetric(byte c);
    Rectangle BBox(byte *s, int l= -1);
    Rectangle BBox(byte c, Point pos);
    Point AdjustString(byte *s, Point p, GrVAdjust va, GrHAdjust ha);
    int Ascender()
	{ return ht; }
    int Descender()
	{ return bs; }
    GrFont Fid()
	{ return family; }
    int Size()
	{ return size; }
    GrFace Face()
	{ return face; }
    int Spacing()
	{ return ils; }
    bool Fixed()
	{ return fixed; }

    Font *WithFace(GrFace face);
    Font *WithSize(int size);

    virtual ostream& StoreOn(ostream&);
    virtual bool Loaded();
    virtual bool LoadFont();
    virtual Font *MakeFont(Font**, GrFace);
    virtual void MakeChar(Font*, GrFace, byte, Font*);
    virtual class DevBitmap *CharAsBitmap(byte, Point *offset);

    void CheckChar(byte c);
    Font *Load();
    char *AsString();

    friend ostream& operator<<(ostream&, FontPtr&);
    friend istream& operator>>(istream&, FontPtr&);
};

SimpleMetaDef(Font);

extern Font *gSysFont,
	    *gApplFont,
	    *gFixedFont;

//---- FontFamily --------------------------------------------------------------

class FontFamily {
public:
    GrFont familyid;
    char *name;
    char *name1;
    bool ispsfont;
    Font **smap[SIZES];
    
public:
    FontFamily(GrFont fid, char *n, char *family, bool isps);
    virtual ~FontFamily();
    void AddFont(char *fontname, GrFace fc, int ps);
    bool IsPSFont()
	{ return ispsfont; }
	
    char *Name()
	{ return name; }
	
    char *Name1()
	{ return name1; }
	
    Font **MapSize(int);

    Font *MapSizeFace(int, GrFace);
};

typedef FontFamily *FontFamilyPtr;

//---- FontManager -------------------------------------------------------------

class FontManager : public Root {
    char *fontDir;
    class FontFamily **fmap;
    
public:
    FontManager(char *lib);
    virtual ~FontManager();
    
    bool Init();        // returns TRUE on error
    
    virtual Font *MapFont(GrFont font, int size, GrFace face);
    virtual Font *MakeFont(char*, GrFont, int, GrFace);
    
    FontFamily *MapFamily(GrFont fid)
	{ return fmap[fid]; }
	
    Font **MapFamilySize(GrFont fid, int size)
	{ return MapFamily(fid)->MapSize(size); }

    Font *MapFamilySizeFace(GrFont fid, int size, GrFace face)
	{ return MapFamily(fid)->MapSizeFace(size, face); }
	
    char *Name(GrFont fid)
	{ return MapFamily(fid)->Name(); }
	
    bool IsPSFont(GrFont fid)
	{ return MapFamily(fid)->IsPSFont(); }
	  
    virtual Font *ScaleFont(Font*, GrFont, int, GrFace);
    
    char *StyleString(GrFace face);

    int NameToId(char *fontname);
    char *IdToName(GrFont fid);
};

extern FontManager *gFontManager;

#endif Font_First

