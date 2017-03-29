#include <stdio.h>

#include "Font.h"
#include "Error.h"
#include "String.h"
#include "WindowSystem.h"

#define FACES       5    /* Bold, Italic, Underline, Outline, Shadow */
#define MAXFACES    32   /* 2**FACES */
#define MAXFAMILIES 30
#define FONTTABLE  "FontTab"
#define FontNameTemplate "%s%02d%s"

Font *gSysFont,
     *gApplFont,
     *gFixedFont;
	   
class FontManager *gFontManager;
char *FontLib;

SimpleMetaImpl(Font)
{
    sprintf(buf, "%s", ((Font*) addr)->AsString());
}

//---- Font --------------------------------------------------------------

Font *new_Font(GrFont font, int size, GrFace face)
{ 
    return gFontManager->MapFont(font, size, face);
}

Font *Font::WithFace(GrFace fc)
{
    return new_Font(family, size, fc);
}

Font *Font::WithSize(int sz)
{
    return new_Font(family, sz, face);
}

bool Font::Loaded()
{
    return TRUE;
}

Font *Font::Load()
{
    if (! Loaded() && LoadFont())
	return 0;
    return this;
}
	
char *Font::AsString()
{
    return form("%s.%d.%s", gFontManager->IdToName(Fid()), Size(),
					    gFontManager->StyleString(Face()));
}

int Font::Width(register byte *s, register int l)
{
    register int w= 0;
    
    if (s) {
	if (l < 0) {
	    while(*s)
		w+= cw[*s++];
	} else {
	    while(--l >= 0)
		w+= cw[*s++];
	}
    }
    return w;
}

int Font::MaxWidth(int l)
{
    if (width < 0) {
	register int i;
	
	width= 0;
	for (i= 0; i< 255; i++)
	    width= max(width, cw[i]);
    }
    return l*width;
}

Rectangle Font::BBox(register byte *s, register int l)
{
    register int w= 0;
    
    if (s == 0)
	return gRect0;
    if (l < 0) {
	while(*s)
	    w+= cw[*s++];
    } else {
	while(--l >= 0)
	    w+= cw[*s++];
    }
    return Rectangle(0, ht, w, ht+bs);
}

Metric Font::GetMetric(register byte *s, register int l)
{
    register int w= 0;
    
    if (s == 0)
	return Metric(0);
    if (l < 0) {
	while(*s)
	    w+= cw[*s++];
    } else {
	while(--l >= 0)
	    w+= cw[*s++];
    }
    return Metric(w, Ascender()+Descender(), Ascender());
}

Metric Font::GetMetric(byte c)
{
    return Metric(Width(c), Ascender()+Descender(), Ascender());
}

Rectangle Font::BBox(register byte c, Point pos)
{
    return Rectangle(pos.x, pos.y-Ascender(), Width(c), Spacing());
}

bool Font::LoadFont()
{
    return FALSE;
}

Font *Font::MakeFont(Font**, GrFace)
{
    return this;
}

Point Font::AdjustString(register byte *s, Point p, GrVAdjust va, GrHAdjust ha)
{
    register int w= 0;
    
    if (s)
	while(*s)
	   w+= cw[*s++];
    
    switch (ha) {
    case eAdjHRight:
	p.x-= w;
	break;
    case eAdjHCenter:
	p.x-= w/2;
	break;
    default:
	break;
    }
    switch (va) {
    case eAdjVBottom:
	p.y-= bs;
	break;
    case eAdjVCenter:
	p.y+= (ht-bs)/2;
	break;
    case eAdjVTop:
	p.y+= ht;
	break;
    case eAdjVBase:
	break;
    }
    return p;
}

DevBitmap *Font::CharAsBitmap(byte, Point*)
{
    return 0;
}

void Font::CheckChar(byte c)
{
    register FontPtr *fdptr;
    int facebit, ff;
    
    fdptr= gFontManager->MapFamilySize((GrFont)family, size);
    ff= eFacePlain;
    facebit= eFaceBold;
    while (face != ff) {
	if (face & facebit) {
	    fdptr[ff|facebit]->MakeChar(fdptr[ff], (GrFace)facebit, c, fdptr[ff & 7]);
	    ff|= facebit;
	}
	facebit<<= 1;
    }
}

void Font::MakeChar(Font*, GrFace, byte, Font*)
{
}

ostream& Font::StoreOn(ostream &s)
{
    return s << (int)family SP << (int)size SP << (int)face SP;
}

ostream& operator<< (ostream &s, FontPtr &fp)
{
    return fp->StoreOn(s);
}

istream& operator>> (istream &s, FontPtr &fp)
{
    GrFont fid;
    GrFace fc;
    int ps;
    
    s >> Enum(fid) >> ps >> Enum(fc);
    fp= gFontManager->MapFont(fid, ps, fc);
    return s;
}

//---- misc ---------------------------------------------------------------------

char *FontManager::StyleString(GrFace face)
{
    static char buf[20];
    char *cp= buf;
    if (face == eFacePlain)
	*cp++= 'r';
    else {
	if (face & eFaceBold)
	    *cp++= 'b';
	if (face & eFaceItalic)
	    *cp++= 'i';
	if (face & eFaceUnderline)
	    *cp++= 'u';
	if (face & eFaceOutline)
	    *cp++= 'o';
	if (face & eFaceShadow)
	    *cp++= 's';
    }
    *cp= '\0';
    return buf;
}

//---- FontManager -------------------------------------------------------------

FontManager::FontManager(char *etdir)
{
    FontLib= fontDir= etdir;
    fmap= new FontFamilyPtr[MAXFAMILIES];
}

FontManager::~FontManager()
{
    for (int f= 0; f < MAXFAMILIES; f++)
	SafeDelete(fmap[f]);
    SafeDelete(fmap);
}

bool FontManager::Init()
{
    char *psname, family[40], fontname[80], name[40], buf[80];
    FILE *fonts;
    int fid, psfont, c, size;
    bool firstchar;
    GrFace style;
    
    if ((fonts= fopen(form("./%s", FONTTABLE), "r")) == 0)
	if ((fonts= fopen(form("%s/%s", fontDir, FONTTABLE), "r")) == 0)
	    Fatal("FontManager::FontManager", "can't locate %s", FONTTABLE);
       
    // read available fonts
    while (fscanf(fonts, "%d %s %s %d", &fid, name, family, &psfont) == 4) {
	if (fid < 0 || fid >= MAXFAMILIES) {
	    Error("FontManager::FontManager", "fid out of range", FALSE);
	    continue;
	}
	fmap[fid]= new FontFamily((GrFont)fid, name, family, psfont);
	firstchar= TRUE;
	style= eFacePlain;
	
	while (c= fgetc(fonts)) {
	    switch (c) {
	    case EOF:
		break;
	    case 'r':
		if (firstchar)
		    firstchar= FALSE;
		style= eFacePlain;
		break;
	    case 'b':
		if (firstchar) {
		    style= eFacePlain;
		    firstchar= FALSE;
		}
		style= (GrFace)(style|eFaceBold);
		break;
	    case 'i':
		if (firstchar) {
		    style= eFacePlain;
		    firstchar= FALSE;
		}
		style= (GrFace)(style|eFaceItalic);
		break;
	    case '\n':
		goto out;
	    default:
		if (Isdigit(c)) {
		    size= 0;
		    while (Isdigit(c)) {
			size= size * 10 + (c-'0');
			c= fgetc(fonts);
		    }
		    ungetc(c, fonts);
		    sprintf(fontname, FontNameTemplate, family, size,
					gFontManager->StyleString(style));
		    fmap[fid]->AddFont(fontname, style, size);
		    firstchar= TRUE;
		}
		break;
	    }
	}
out:    ;
    }

    fclose(fonts);
    
    if ((psname= gSystem->getenv("ET_FONT_SIZE")) == 0)
	psname= "12";
    size= atoi(psname);
    
    gSysFont= new_Font(eFontChicago, size);
    gApplFont= new_Font(eFontMonaco, size);
    gFixedFont= new_Font(eFontCourier, size);

    return FALSE;
}

Font *FontManager::MakeFont(char*, GrFont, int, GrFace)
{
    return 0;
}
    
Font *FontManager::MapFont(GrFont font, int size, GrFace face)
{
    if (font < 0 || font >= MAXFAMILIES || fmap[font] == 0)
	return gSysFont;
    return MapFamily(font)->MapSizeFace(size, face);
}

int FontManager::NameToId(char *fontname)
{
    for (int i= 0; i < MAXFAMILIES; i++)
	if (fmap[i] && fmap[i]->Name() && strcmp(fmap[i]->Name(), fontname) == 0)
	    return i;
    return -1;
}

char *FontManager::IdToName(GrFont fid)
{
    if (fid >= 0 && fid < MAXFAMILIES && fmap[fid])
	return fmap[fid]->Name();
    return 0;
}

Font *FontManager::ScaleFont(Font*, GrFont, int, GrFace)
{
    return 0;
}

//---- FontFamily --------------------------------------------------------------

FontFamily::FontFamily(GrFont fid, char *nm, char *family, bool psfont)
{
    familyid= fid;
    name1= strsave(family);
    name= strsave(nm);
    ispsfont= psfont;
}

FontFamily::~FontFamily()
{
    int s, ss, fc;
    
    for (s= 0; s < SIZES; s++) {
	if (smap[s]) {
	    for (ss= s+1; ss < SIZES; ss++)
		if (smap[s] == smap[ss])
		    smap[ss]= 0;    // don't delete smap twice
	    for (fc= 0; fc < MAXFACES; fc++)
		if (smap[s][fc] && smap[s][fc]->Size())
		    SafeDelete(smap[s][fc]);
	    SafeDelete(smap[s]);
	}
    }
    SafeDelete(name1);
    SafeDelete(name);
}

void FontFamily::AddFont(char *fontname, GrFace fc, int ps)
{
    int internalsize;
    FontPtr *fdptr;
    
    if (ps < MINSIZE || ps > MAXSIZE)
	Error("FontFamily::AddFont:", "size out of range", FALSE);
    if (fc < 0 || fc >= MAXFACES)
	Error("FontFamily::AddFont:", "face out of range", FALSE);
	
    internalsize= ps-MINSIZE;
    if (smap[internalsize] == 0)
	smap[internalsize]= new FontPtr[MAXFACES];
    fdptr= smap[internalsize];
    
    fdptr[fc]= gFontManager->MakeFont(fontname, familyid, ps, fc);
}

Font **FontFamily::MapSize(int size)
{
    Font *fp= 0, **ffp= 0;
    register int s, try, i;
    
    size= range(MINSIZE, MAXSIZE, size);
    s= size-MINSIZE;
    
    if (smap[s])
	return smap[s];
	
    // try twice a bigger font
    try= size * 2;
    if (try <= MAXSIZE && smap[try-MINSIZE]
					&& (fp= smap[try-MINSIZE][eFacePlain]))
	fp= gFontManager->ScaleFont(fp->Load(), familyid, size, eFacePlain);
    
    if (fp == 0) {  // try twice a smaller font
	try= size / 2;
	if (try >= MINSIZE && smap[try-MINSIZE]
					&& (fp= smap[try-MINSIZE][eFacePlain]))
	    fp= gFontManager->ScaleFont(fp->Load(), familyid, size, eFacePlain);
    }
    
    if (fp == 0) {  // try nearest font
	for (i= 1; i < MAXSIZE-MINSIZE; i++) {
	    if (s-i >= 0 && (smap[s]= smap[s-i]))
		break;
	    if (s+i <= MAXSIZE-MINSIZE && (smap[s]= smap[s+i]))
		break;
	}
    }
    
    if (fp) {
	if (smap[s] == 0)
	    smap[s]= new FontPtr[MAXFACES];
	smap[s][eFacePlain]= fp;
    }
    
    return smap[s];
}

Font *FontFamily::MapSizeFace(int size, GrFace face)
{
    int ff, facebit;
    Font **fdptr= MapSize(size);
    
    if (face < 0 || face >= MAXFACES)
	face= eFacePlain;
	
    if (fdptr[face])
	return fdptr[face]->Load();
	
    ff= eFacePlain;
    facebit= eFaceBold;
    while (face != ff) {
	if (face & facebit) {
	    if (fdptr[ff | facebit] == 0)
		fdptr[ff | facebit]= fdptr[ff]->Load()->MakeFont(fdptr, (GrFace)facebit);
	    ff|= facebit;
	}
	facebit<<= 1;
    }
    return fdptr[face];
}
