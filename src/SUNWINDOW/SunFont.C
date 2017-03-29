#include "SunFont.h"
#include "SunBitmap.h"
#include "String.h"
#include "Error.h"

#include "pixrect.h"
#include "pixfont.hh"

#define VFontExt "vf"
#define MAXCHAR 256

extern char *FontLib;


struct pixrect *ScaleGlyph(struct pixrect *spr, double sc)
{
    register int src, dst, x, y, sx, sy, scale;
    struct pixrect *pr, *pr1;
    
    x= spr->pr_size.x;
    y= spr->pr_size.y;

    sx= (int) ((double)x*sc);
    sy= (int) ((double)y*sc);
    
    pr= mem_create(sx, y, 1);
    pr1= mem_create(sx, sy, 1);
    
    if (sx == 0 || sy == 0)
	return spr;
	
    if (sx < x) { 
	scale= x/sx;
	for (dst= 0; dst < sx; dst++)       /* x-shrink */
	    pr_rop(pr, dst, 0, 1, y, PIX_SRC|PIX_DONTCLIP, spr, dst*scale, 0);
	for (dst= 0; dst < sy; dst++)       /* y-shrink */
	    pr_rop(pr1, 0, dst, sx, 1, PIX_SRC|PIX_DONTCLIP, pr, 0, dst*scale);
    } else {            
	scale= sx/x;
	/* x-magnify */
	for (src= 0; src < x; src++)        /* slice */
	    pr_rop(pr, src*scale, 0, 1, y, PIX_SRC|PIX_DONTCLIP, spr, src, 0);
	for (src= 0; src < scale-1; src++)  /* smear */
	    pr_rop(pr, 1, 0, sx-1, y, PIX_OR|PIX_DONTCLIP, pr, 0, 0);
	/* y-magnify */
	for (src= 0; src < y; src++)        /* slice */
	    pr_rop(pr1, 0, src*scale, sx, 1, PIX_SRC|PIX_DONTCLIP, pr, 0, src);
	for (src= 0; src < scale-1; src++)  /* smear */
	    pr_rop(pr1, 0, 1, sx, sy-1, PIX_OR|PIX_DONTCLIP, pr1, 0, 0);
    }
    pr_destroy(pr);
    return pr1;
}

static int linenotempty(struct pixrect *pr, int y)
{
    for (int x= 0; x < pr->pr_size.x; x++)
	if (pr_get(pr, x, y))
	    return 1;
    return 0;
}

//---- SunFontManager ----------------------------------------------------------

SunFont::SunFont(char *vname, GrFont fid, int ps, GrFace fc)
{
    family= fid;
    size= (short) ps;
    face= fc;
    if (vname)
	vfontname= strsave(vname);
    else
	vfontname= 0;
    loaded= FALSE;
    notdef= 0;
}

SunFont::~SunFont()
{
    if (notdef) {
	pr_destroy(notdef);
	notdef= 0;
    }
    if (pf) {
	if (vfontname)
	    pf_close(pf);
	else
	    delete pf;
	pf= 0;
    }
    SafeDelete(vfontname);
}

bool SunFont::Loaded()
{
    return loaded;
}

bool SunFont::LoadFont()
{
    static char pathname[100];
    register int maxh= 0, maxb= 0, i, w, h, htt;
    struct pixchar pc, *pcp;
    
    if (vfontname == 0)
	return FALSE;
	
    sprintf(pathname, "./%s.%s", vfontname, VFontExt);
    if ((pf= pf_open_private(pathname)) == NULL) {
	sprintf(pathname, "%s/sunfonts/%s.%s", FontLib, vfontname, VFontExt);
	if ((pf= pf_open(pathname)) == NULL) {
	    Error("SunFont::LoadFont", "can't open font %s", pathname);
	    return TRUE;
	}
    }
    
    pc= pf->pf_char['0'];
    w= pc.pc_pr->pr_size.x;
    h= pc.pc_pr->pr_size.y;
    notdef= pc.pc_pr= mem_create(w, h, 1);
    pr_vector(pc.pc_pr, 1,   1,   w-2, 1,   PIX_OR, 1);
    pr_vector(pc.pc_pr, w-2, 1,   w-2, h-2, PIX_OR, 1);
    pr_vector(pc.pc_pr, w-2, h-2, 1,   h-2, PIX_OR, 1);
    pr_vector(pc.pc_pr, 1,   h-2, 1,   1,   PIX_OR, 1);
	
    for (i= 0; i < MAXCHAR; i++) {
	pcp= &pf->pf_char[i];
	if (pcp->pc_pr) {
	    htt= - pcp->pc_home.y;
	    maxh= max(maxh, htt);
	    maxb= max(maxb, pcp->pc_pr->pr_size.y - htt);
	} else 
	    *pcp= pc;
	cw[i]= pcp->pc_adv.x;
    }
    ht= maxh;
    bs= maxb;
    /* ils= pf->pf_defaultsize.y; */
    ils= maxh + maxb + 3;
    loaded= TRUE;
    return FALSE;
}

Font *SunFont::MakeFont(Font**, GrFace fc)
{
    struct pixchar *pc, *npc;
    register int i, adv, homex, homey;
    
    SunFont *nfd= new SunFont(0, family, size, (GrFace)(face | fc));
    bool fixed= (family == eFontCourier);
    
    nfd->pf= new pixfont;
    *nfd->pf= *pf;   // copy all
    nfd->ht= ht;
    nfd->bs= bs;
    nfd->ils= ils;
    nfd->vfontname= 0;
    nfd->notdef= notdef;
    nfd->loaded= FALSE;
    
    pc= pf->pf_char;
    npc= nfd->pf->pf_char;
    for (i= 0; i < MAXCHAR; i++, pc++, npc++) {
	if (pc->pc_pr != notdef) {
	    adv= homex= homey= 0;
	    
	    switch (fc) {
	    case eFaceBold:
		if (fixed)
		    adv= 0;
		else
		    adv= 1;
		break;
	    case eFaceItalic:
		if (fixed)
		    adv= 0;
		else
		    adv= 1;
		homex= -(bs/2);
		break;
	    case eFaceOutline:
		adv= 2;
		homex= homey= -1;
		break;
	    case eFaceShadow:
		adv= 3;
		homex= homey= -1;
		break;
	    default:
		break;
	    }
	    npc->pc_pr= 0;
	    npc->pc_adv.x+= adv;
	    npc->pc_home.x+= homex;
	    npc->pc_home.y+= homey;
	}
	nfd->cw[i]= npc->pc_adv.x;
    }
    return nfd;
}

#define scale(x) ((int) ((double)(x)*s))

void SunFont::ScaleFont(Font *fp)
{
    SunFont *fd= (SunFont*) fp;
    struct pixfont *opf;
    struct pixchar *pc, *npc;
    register int i;
    double s= (double) size / (double) fd->Size();
    
    opf= fd->pf;
    pf= new pixfont;
    *pf= *opf; // copy all
    
    ht= scale(fd->ht);
    bs= scale(fd->bs);
    ils= scale(fd->ils);
    vfontname= 0;
    notdef= ScaleGlyph(fd->notdef, s);
    loaded= TRUE;
    
    pc= opf->pf_char;
    npc= pf->pf_char;
    for (i= 0; i < MAXCHAR; i++, pc++, npc++) {
	if (pc->pc_pr != fd->notdef) {
	    npc->pc_pr= ScaleGlyph(pc->pc_pr, s);
	    npc->pc_adv.x= scale(pc->pc_adv.x);
	    npc->pc_home.x= scale(pc->pc_home.x);
	    npc->pc_home.y= scale(pc->pc_home.y);
	} else
	    pc->pc_pr= notdef;
	cw[i]= npc->pc_adv.x;
    }
}

void SunFont::MakeChar(Font *fp, GrFace face, byte i, Font *innerfd)
{
    register int w, h, x, y, a;
    struct pixrect *pr, *npr= 0, *pr1;
    SunFont *fd= (SunFont*) fp;

    if (pf->pf_char[i].pc_pr)
	return;

    pr= fd->pf->pf_char[i].pc_pr;

    w= pr->pr_size.x;
    h= pr->pr_size.y;

    switch (face) {
    case eFacePlain:
	break;
	
    case eFaceBold:
	npr= mem_create(w+1, h, 1);
	pr_rop(npr, 0, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 1, 0, w, h, PIX_OR, pr, 0, 0);
	break;

    case eFaceItalic:
	a= fd->Ascender()/2;
	npr= mem_create(w + a, h, 1);
	for (y= 0, x= a; y <= h; y+= 2, x--)
	    pr_rop(npr, x, y, w, 2, PIX_OR, pr, 0, y);
	break;

    case eFaceUnderline:
	w= max(w, fd->Width(i));
	y= - fd->pf->pf_char[i].pc_home.y+1;
	npr= mem_create(w, h, 1);
	pr_rop(npr, 0, y, w, 1, PIX_SET, 0, 0, 0);
	pr_rop(npr, 0, y, w-1, 1, PIX_ERASE, pr, 1, y);
	pr_rop(npr, 1, y, w-1, 1, PIX_ERASE, pr, 0, y);
	pr_rop(npr, 0, 0, w, h, PIX_OR, pr, 0, 0);
	break;

    case eFaceOutline:
	npr= mem_create(w+2, h+2, 1);
	pr_rop(npr, 0, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 1, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 2, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 0, 1, w+2, h+1, PIX_OR, npr, 0, 0);
	pr_rop(npr, 0, 1, w+2, h+1, PIX_OR, npr, 0, 0);
	pr_rop(npr, 1, 1, w, h, PIX_ERASE, pr, 0, 0);
	break;

    case eFaceShadow:
	npr= mem_create(w+3, h+3, 1);
	pr_rop(npr, 0, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 1, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 2, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 3, 0, w, h, PIX_OR, pr, 0, 0);
	pr_rop(npr, 0, 1, w+3, h+2, PIX_OR, npr, 0, 0);
	pr_rop(npr, 0, 1, w+3, h+2, PIX_OR, npr, 0, 0);
	pr_rop(npr, 0, 1, w+3, h+2, PIX_OR, npr, 0, 0);
	pr1= ((SunFont*)innerfd)->pf->pf_char[i].pc_pr;
	pr_rop(npr, 1, 1, w, h, PIX_ERASE, pr1, 0, 0);
	break;
    }
    pf->pf_char[i].pc_pr= npr;
}

DevBitmap *SunFont::CharAsBitmap(byte c, Point *offset)
{ 
    struct pixchar *pc= &pf->pf_char[c];

    if (pc->pc_pr == 0)
	CheckChar(c);

    if (pc->pc_pr && pc->pc_pr != notdef) {
	offset->x= pc->pc_home.x;
	offset->y= -pc->pc_home.y;
	return new SunBitmap(pc->pc_pr, 0, FALSE);
    }
    return 0;
}

//---- SunFontManager ----------------------------------------------------------

SunFontManager::SunFontManager(char *lib) : FontManager(lib)
{
}

Font* SunFontManager::MakeFont(char *vname, GrFont fid, int ps, GrFace fc)
{
    return new SunFont(vname, fid, ps, fc);
}

Font *SunFontManager::ScaleFont(Font *ofp, GrFont fid, int ps, GrFace face)
{
    SunFont *sfp= new SunFont(0, fid, ps, face);
    sfp->ScaleFont(ofp);
    return sfp;
}
