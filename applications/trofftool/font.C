//$Fontdesc,TroffFontManager$
#include "String.h"
#include "Storage.h"
#include "Error.h"
#include "OrdColl.h"
#include "font.h"

/* #define FONTDIR "/local/lib/font" */
#define FONTDIR "."

TroffFontManager *tfm;

struct fontmap {
    char *name;
    GrFont fid;
    GrFace face;
} fontmap[]= {
    "R",    eFontTimes,        eFacePlain,    
    "I",    eFontTimes,        eFaceItalic,    
    "B",    eFontTimes,        eFaceBold,    
    "D",    eFontTimes,        GrFace(eFaceBold | eFaceItalic),    
    "TT",   eFontCourier,      eFacePlain,    
    "TB",   eFontCourier,      eFaceBold,    
    "TI",   eFontCourier,      eFaceItalic,    
    "TD",   eFontCourier,      GrFace(eFaceBold | eFaceItalic),
    "L",    eFontCourier,      eFacePlain,    
    "CW",   eFontCourier,      eFacePlain,    
    "C",    eFontCourier,      eFacePlain,    
    "CO",   eFontCourier,      eFaceItalic,    
    "CB",   eFontCourier,      eFaceBold,    
    "BI",   eFontTimes,        GrFace(eFaceBold | eFaceItalic),    
    "H",    eFontHelvetica,    eFacePlain,    
    "HB",   eFontHelvetica,    eFaceBold,    
    "HD",   eFontHelvetica,    GrFace(eFaceBold | eFaceItalic),
    "HI",   eFontHelvetica,    eFaceItalic,    
    "HO",   eFontHelvetica,    eFaceItalic,
    "Ho",   eFontHelvetica,    eFaceOutline,
    "S",    eFontSymbol,       eFacePlain,
    "SY",   eFontSymbol,       eFacePlain,
    "SL",   eFontSymbol,       eFacePlain,
    NULL
};
    
//---- Fontdesc ----------------------------------------------------------------

MetaImpl(Fontdesc, (TE(fid), TE(face), I_CS(path)));

Fontdesc::Fontdesc(struct font *f, char *p)
{
    int nw;
    
    fontbase= f;
    nw= f->nwfont & BMASK;
    codetab= (char*) f + sizeof(font) + 2 * nw;
    fitab= (char*) f + sizeof(font) + 3 * nw;
    
    if (p)
	path= strsave(p);
}

void Fontdesc::Init(char *nm)
{
    struct fontmap *fmp;
    
    for (fmp= fontmap; fmp->name; fmp++)
	if (strcmp(nm, fmp->name) == 0)
	    break;
    if (fmp->name) {
	fid= fmp->fid;
	face= fmp->face;
    }
}

bool Fontdesc::Isequal(char *name, char *path)
{
    return strcmp(Name(), name) == 0 && strcmp(Path(), path) == 0;
}

//---- TroffFontManager --------------------------------------------------------

MetaImpl(TroffFontManager, (I_O(flist)));

TroffFontManager::TroffFontManager(char *devname)
{
    int i;
    char *filebase, *p;
    Fontdesc *fdp;

    sorted= TRUE;
    DevName= devname;
    flist= new OrdCollection;
    
    /* open table for device,
     * read in resolution, size info, font info, etc.
     */
    istream fin(form("%s/dev%s/DESC.out", FONTDIR, devname));
    if (! fin)
	Fatal("TroffFontManager", "can't open tables for device %s", devname);
    fin.read((u_char*) &dev, sizeof(struct dev));
    
    if (dev.nfonts > MAXFONTS)
	Warning("TroffFontManager", "too many fonts in DESC");

    /* enough room for whole file */
    filebase= new char[dev.filesize];
    fin.read((u_char*) filebase, dev.filesize); // all at once
    
    chtab= ((short*) filebase) + dev.nsizes + 1;
    chname= (char*) (chtab + dev.nchtab);
    
    p= (char*) chname + dev.lchname;   /* p points to first default font */
    
    for (i= 0; i < dev.nfonts; i++) {
	fdp= new Fontdesc((struct font*) p, "");
	flist->Add(fdp);
	p+= FontSize(fdp->Nw());
    }
    
    /* is chtab sorted ? */
    for (i= 1; i < dev.nchtab-1; i++) {
	if (strcmp(&chname[chtab[i-1]], &chname[chtab[i]]) > 0) {
	    sorted= FALSE;
	    break;
	}
    }
}

int TroffFontManager::Lookup(char *s)
{
    static int k;
    register int i, j, r;
    
    /* same character as last time ? */
    if (strcmp(s, &chname[chtab[k]]) == 0)
	return k+128;
    
    if (sorted) {   /* binary search */
	i= 0;
	j= dev.nchtab-1;
	do {
	    k= (i+j) / 2;
	    if (r= strcmp(s, &chname[chtab[k]])) {
		if (r > 0)
		    i= k+1;
		else
		    j= k-1;
	    } else
		return k+128;
	} while (i <= j);            
    } else {        /* sequential search */
	for (k= 0; k < dev.nchtab; k++)
	    if (strcmp(&chname[chtab[k]], s) == 0) 
		return k+128;
    }
    k= 0;
    return -1;
}

Fontdesc *TroffFontManager::Load(char *s, char *s1)
{
    Iter next(flist);
    register Fontdesc *fdp;

    while (fdp= (Fontdesc*) next())  // is font already loaded ?
	if (fdp->Isequal(s, s1))
	    break;
    
    if (fdp == 0) {
	char *p, *temp;
	if (s1 == NULL || s1[0] == '\0')
	    temp= form("%s/dev%s/%s.out", FONTDIR, DevName, s);
	else
	    temp= form("%s/%s.out", s1, s);
		
	istream fin(temp);
	if (fin) {
	    p= new char[FontSize(255)];
	    fin.read((u_char*) p, FontSize(255));
	} else {
	    p= (char*) ((Fontdesc*)flist->First())->Base();
	}
	fdp= new Fontdesc((struct font*)p, s1);
	flist->Add(fdp);
    }
    fdp->Init(s);
    return fdp;
}

//---- Fontmap -----------------------------------------------------------------

void Fontmap::Load(int n, char *s, char *s1)
{
    if (n < 0 || n > MAXFONTS) {
	Warning("Load", "font position (%d) for %s out of range (illegal fp command)", n, s);
	n= 0;
    }
    map[n]= tfm->Load(s, s1);
    if (smnt == 0 && map[n]->Special())
	smnt= n;
}

Fontdesc *Fontmap::Lookup(register int &c, char *s, register int fp)
{
    register int i, j;
    
    if (s[1]) {
	if ((c= tfm->Lookup(s)) < 0)
	    return 0;
    } else
	c= s[0];
    
    if (c <= ' ' || map[fp] == 0)
	return 0;
    c-= 32;
    i= map[fp]->Fit(c);

    if (i == 0 && smnt > 0)     // on special (we hope)
	for (fp= smnt, j= 0; j <= MAXFONTS; j++, fp= fp % MAXFONTS + 1)
	    if (map[fp] && (i= map[fp]->Fit(c)))
		break;

    if (i == 0 || fp > MAXFONTS || map[fp] == 0) /* character not found */
	return 0;
    c= map[fp]->Code(i);
    return map[fp];
}
