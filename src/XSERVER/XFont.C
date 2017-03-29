#include <stdio.h>

#include "X11.h"
#include "XFont.h"
#include "XWinSystem.h"
#include "String.h"
#include "Error.h"
#include "XBitmap.h"

XServerFont::XServerFont(char *fontname, GrFont fid, int ps, GrFace fc)
{
    family= fid;
    size= ps;
    face= fc;
    
    xfid= 0;
    xFont= 0;
    fname= strsave(fontname);
}

bool XServerFont::Loaded()
{
    return xfid != 0;
}

bool XServerFont::LoadFont()
{
    register int i, j;
    register byte w;
    char *fn= fname;
    
    if ((xFont= XLoadQueryFont(display, fn)) == 0) {
	char *otherfont;
	
	switch (Fid()) {
	case eFontCourier:
	    otherfont= "fixed";
	    break;
	case eFontChicago:
	    otherfont= "variable";
	    break;
	default:
	    otherfont= "8x13";
	    break;
	}
	
	fprintf(stderr, "can't load font %s; trying %s\n", fn, otherfont);
	fn= otherfont;
	if ((xFont= XLoadQueryFont(display, fn)) == 0) {
	    fprintf(stderr, "can't load font %s; giving up\n", fn);
	    return TRUE;
	}
    }
	
    ht= xFont->max_bounds.ascent;
    bs= xFont->max_bounds.descent;
    ils= xFont->ascent + xFont->descent;
    ils= ht+bs+3;
    
    if (xFont->per_char)
	w= (byte) xFont->per_char[xFont->default_char].width;
    else
	w= xFont->max_bounds.width;

    for (i= 0; i < 256; i++)
	cw[i]= w;
    
    if (xFont->per_char) {
	if (xFont->min_byte1 == NULL && xFont->max_byte1 == NULL)
	    for (i= xFont->min_char_or_byte2, j= 0; i <= xFont->max_char_or_byte2; i++, j++)
		cw[i]= (byte) xFont->per_char[j].width;
	else
	    fprintf(stderr, "can't handle two-byte fonts\n", fn);
    }
    xfid= xFont->fid;
    return FALSE;
}

XServerFont::~XServerFont()
{
    SafeDelete(fname);
    if (xfid) {
	XUnloadFont(display, xfid);
	xfid= 0;
    }
    if (xFont) {
	XFreeFontInfo(NULL, xFont, 0);
	xFont= 0;
    }
}

DevBitmap *XServerFont::CharAsBitmap(byte c, Point *offset)
{ 
    XGCValues gcv;
    XCharStruct cs;
    Pixmap pm;
    GC gc;
    Point e;
    
    if (xFont->per_char)
	cs= xFont->max_bounds;
    else
	cs= xFont->per_char[c];
    e.x= cs.rbearing - cs.lbearing;
    e.y= cs.ascent + cs.descent;
    offset->x= -cs.lbearing;
    offset->y= cs.ascent;
    pm= XCreatePixmap(display, rootwin, e.x, e.y, 1);
    gcv.background= 0;
    gcv.foreground= 0;
    gc= XCreateGC(display, pm, GCForeground|GCBackground, &gcv);
    XFillRectangle(display, pm, gc, 0, 0, e.x, e.y);
    XSetForeground(display, gc, 1);
    XDrawString(display, pm, gc, offset->x, offset->y, (const char*) &c, 1);
    XFreeGC(display, gc);
    return new XBitmap(e, pm);
}

Font* XFontManager::MakeFont(char *vname, GrFont fid, int ps, GrFace fc)
{
    return new XServerFont(vname, fid, ps, fc);
}
