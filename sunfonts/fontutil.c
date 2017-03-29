#include <ctype.h>
#include "../src/CType.h"
#include <stdio.h>
#include "Font.h"

extern char *gets(), *index();
extern char *strcpy();

static char *myname;    /* initialized from argv[0] */
static char *currentFile;
static int linenum;        /* for error messages */

char *adobename(i)
int i;
{
    static char buf[10];
    
    char *s= (_C_Type_+1)[i].name;
    if (s)
	return s;
    sprintf(buf, "C0%02x", i);
    return buf;
}

static char *hexChars= "0123456789abcdef";

void MaxExtent(font, xp, yp)
Font *font;
int *xp, *yp;
{
    register Glyph *gp;
    register int i, width, height;
    
    width= height= 0;
    for (i= 0; i < MAXCHARS; i++) {
	if (gp= font->glyphs[i]) {
	    width= max(width, gp->left + gp->right);
	    height= max(height, gp->up + gp->down);
	}
    }
    *xp= width;
    *yp= height;
}

void DumpAsBdf(ofp, font)
FILE *ofp;
Font *font;
{
    register Glyph *gp;
    register byte *cp;
    int nchars, i, x, y, w, h, width, height, left, down, up;
    
    fputs("STARTFONT 2.1\n", ofp);
    for (i= 0; i < font->ncomments; i++)
	fprintf(ofp, "%s\n", font->comments[i]);
    fprintf(ofp, "FONT %s\n", font->name);
    fprintf(ofp, "SIZE %d %d %d\n", font->size, font->resx, font->resy);
    
    nchars= width= height= left= down= up= 0;
    for (i= 0; i < MAXCHARS; i++) {
	if (gp= font->glyphs[i]) {
	    width= max(width, gp->left + gp->right);
	    height= max(height, gp->up + gp->down);
	    left= max(left, gp->left);
	    down= max(down, gp->down);
	    up= max(up, gp->up);
	    nchars++;
	}
    }
    fprintf(ofp, "FONTBOUNDINGBOX %d %d %d %d\n", width, height, -left, -down);

    if (font->nprops+3) {
	fprintf(ofp, "STARTPROPERTIES %d\n", font->nprops+3);
	if (font->ascent == 9999)
	    font->ascent= up;
	fprintf(ofp, "FONT_ASCENT %d\n", font->ascent);
	if (font->descent == 9999)
	    font->descent= down;
	fprintf(ofp, "FONT_DESCENT %d\n", font->descent);
	if (font->defaultchar == 9999)
	    font->defaultchar= 0;
	fprintf(ofp, "DEFAULT_CHAR %d\n", font->defaultchar);
	for (i= 0; i < font->nprops; i++) 
	    fprintf(ofp, "%s\n",font->props[i]);
	fputs("ENDPROPERTIES\n", ofp);
    }

    fprintf(ofp, "CHARS %d\n", nchars);
	    
    for (i= 0; i < MAXCHARS; i++) {
	if ((gp= font->glyphs[i]) == 0)
	    continue;
	    
	fprintf(ofp, "STARTCHAR %s\n", gp->name);
	fprintf(ofp, "ENCODING %d\n", i);
	fprintf(ofp, "SWIDTH %d 0\n", gp->swidth);
	fprintf(ofp, "DWIDTH %d 0\n", gp->width);
	w= gp->left+gp->right;
	h= gp->up+gp->down;
	fprintf(ofp, "BBX %d %d %d %d\n", w, h, -gp->left, -gp->down);
	if (gp->attr)
	    fprintf(ofp, "ATTRIBUTES %04x\n", gp->attr);
	fprintf(ofp, "BITMAP\n");
	if (gp->nbytes && gp->bits) {
	    for (y= 0; y < h; y++) {
		cp= &gp->bits[y*gp->bytesperline];
		for (x= 0; x < gp->bytesperline; x++, cp++) {
		    fputc(hexChars[*cp >> 4], ofp);
		    fputc(hexChars[*cp & 15], ofp);
		}
		fputc('\n', ofp);
	    }
	}
	fputs("ENDCHAR\n", ofp);
    }
    fputs("ENDFONT\n", ofp);
}

char *strsave(s)
char *s;
{
    return strcpy(malloc(strlen(s)+1), s);
}

void AddProp(font, s)
Font *font;
char *s;
{
    font->props[font->nprops++]= strsave(s);
}

void AddComment(font, s)
Font *font;
char *s;
{
    font->comments[font->ncomments++]= strsave(s);
}

Font *NewFont(name, size, resx, resy, asc, desc, dflt)
char *name;
int size, resx, resy, asc, desc, dflt;
{
    Font *fp= (Font*) malloc(sizeof (Font));
    bzero(fp, sizeof (Font));
    fp->props= (char**) malloc(sizeof (char*) * 10);
    fp->comments= (char**) malloc(sizeof (char*) * 10);
    if (name)
	fp->name= strsave(name);
    fp->size= size;
    fp->resx= resx;
    fp->resy= resy;
    fp->ascent= asc;
    fp->descent= desc;
    fp->defaultchar= dflt;
    return fp;
}

AddGlyph(font, i, wd, up, down, left, right, n, bp, name, attr)
Font *font;
char *name;
unsigned int attr;
int i, wd, up, down, left, right, n;
byte *bp;
{
    Glyph *gp;
    
    if (font->glyphs[i])
	FreeGlyph(font->glyphs[i]);
    gp= font->glyphs[i]= (Glyph*) malloc(sizeof (Glyph));
    gp->attr= attr;
    gp->width= wd;
    gp->up= up;
    gp->down= down;
    gp->left= left;
    gp->right= right;
    gp->swidth= 666;
    gp->name= name;
    
    gp->bytesperline= Bytes(left+right);
    gp->nbytes= n;
    if (n > 0 && bp != 0) {
	gp->bits= (byte*) malloc(n);
	bcopy(bp, gp->bits, n);
    } else
	gp->bits= 0;
}

/*
 * read the next line and keep a count for error messages
 */
char *getline(s)
char *s;
{
    s= gets(s);
    linenum++;
    while (s) {
	int len= strlen(s);
	if (len && s[len-1] == '\015')
	    s[--len] = '\0';
	if (len == 0) {
	    s= gets(s);
	    linenum++;
	} else
	    break;
    }
    return s;
}

/*
 * return 1 if str is a prefix of buf
 */
int prefix(buf, str)
char *buf, *str;
{
    return strncmp(buf, str, strlen(str)) ? 0 : 1;
}

/*
 * make a byte from the first two hex characters in s
 */
byte hexbyte(s)
char *s;
{
    byte b = 0;
    register char c;
    int i;

    for (i=2; i; i--) {
	c = *s++;
	if (isascii(c) && isxdigit(c)) {
	    if (isdigit(c))
		b = (b<<4) + (c - '0');
	    else if (isupper(c))
		b = (b<<4) + 10 + (c - 'A');
	    else
		b = (b<<4) + 10 + (c - 'a');
	} else
	    fatal("bad hex char '%c'", c);
    } 
    return b;
}

/*VARARGS*/
warning(msg, p1, p2, p3, p4)
    char *msg, *p1;
{
    fprintf(stderr, "%s: %s: ", myname, currentFile);
    fprintf(stderr, msg, p1, p2, p3, p4);
    if (linenum)
	fprintf(stderr, " at line %d\n", linenum);
    else
	fprintf(stderr, "\n");
}

/*
 * fatal error. never returns.
 */
/*VARARGS*/
fatal(msg, p1, p2, p3, p4)
    char *msg, *p1;
{
    warning(msg, p1, p2, p3, p4);
    exit(1);
}

Font *ReadBdf(file)
char *file;
{
    int nchars, nbytes, i, resx, resy, haveFontAscent, haveFontDescent;
    unsigned int attributes;
    char linebuf[BUFSIZ], namebuf[100], fontName[100];
    byte *bp, bbuf[10000];
    float size;
    Font *font;
    
    haveFontAscent= haveFontDescent= 0;

    font= NewFont(0, 0, 0, 0, 0, 0, 0);
    
    currentFile= file;
    
    if (freopen(currentFile, "r", stdin) == NULL)
	fatal("could not open file\n");

    getline(linebuf);

    if ((sscanf(linebuf, "STARTFONT %s", namebuf) != 1) || strcmp(namebuf, "2.1"))
	fatal("bad 'STARTFONT'");
    getline(linebuf);
    
    while (prefix(linebuf, "COMMENT")) {
	AddComment(font, linebuf);
	getline(linebuf);
    }

    if (sscanf(linebuf, "FONT %[^\n]", fontName) != 1)
	fatal("bad 'FONT'");
    getline(linebuf);

    if (!prefix(linebuf, "SIZE"))
	fatal("missing 'SIZE'");
    if ((sscanf(linebuf, "SIZE %f%d%d", &size, &resx, &resy) != 3))
	fatal("bad 'SIZE'");
    if ((size < 1) || (resx < 1) || (resy < 1))
	fatal("SIZE values must be > 0");
    if (resx != resy)
	fatal("x and y resolution must be equal");
    getline(linebuf);

    if (!prefix(linebuf, "FONTBOUNDINGBOX"))
	fatal("missing 'FONTBOUNDINGBOX'");
    getline(linebuf);

    font->name= strsave(fontName);
    font->size= (int)size;
    font->resx= resx;
    font->resy= resy;

    if (prefix(linebuf, "STARTPROPERTIES")) {
	int nprops;

	if (sscanf(linebuf, "STARTPROPERTIES %d", &nprops) != 1)
	   fatal("bad 'STARTPROPERTIES'");
	   
	getline(linebuf);
	AddProp(font, linebuf);
	while((nprops-- > 0) && !prefix(linebuf, "ENDPROPERTIES")) {
	    getline(linebuf);
	    if (nprops > 0)
		AddProp(font, linebuf);
	}
	if (!prefix(linebuf, "ENDPROPERTIES"))
	    fatal("missing 'ENDPROPERTIES'");
	/*
	if (!haveFontAscent || !haveFontDescent)
	    fatal("must have 'FONT_ASCENT' and 'FONT_DESCENT' properties");
	*/
	if (nprops != -1)
	    fatal("%d too few properties", nprops+1);
	    
    } else /* no properties */
	fatal("missing 'STARTPROPERTIES'");
    getline(linebuf);

    if (sscanf(linebuf, "CHARS %d", &nchars) != 1)
	fatal("bad 'CHARS'");
    if (nchars < 1)
	fatal("invalid number of CHARS");
    getline(linebuf);

    while ((nchars-- > 0) && prefix(linebuf, "STARTCHAR"))  {
	int t, ix, wx, wy, bw, bh, bl, bb, enc, enc2, bytesperrow, row;
	char *p, charName[100];

	if (sscanf(linebuf, "STARTCHAR %s", charName) != 1)
	    fatal("bad character name");

	getline(linebuf);
	if ((t=sscanf(linebuf, "ENCODING %d %d", &enc, &enc2)) < 1)
	    fatal("bad 'ENCODING'");
	if ((enc < -1) || ((t == 2) && (enc2 < -1)))
	    fatal("bad ENCODING value");
	if (t == 2 && enc == -1)
	    enc = enc2;
	if (enc == -1) {
	    warning("character '%s' ignored\n", charName);
	    do {
		char *s = getline(linebuf);
		if (!s)
		    fatal("Unexpected EOF");
	    } while (!prefix(linebuf, "ENDCHAR"));
	    getline(linebuf);
	    continue;
	}
	if (enc >= MAXCHARS)
	    fatal("character '%s' has encoding(=%d) too large", charName, enc);

	getline(linebuf);
	if (sscanf( linebuf, "SWIDTH %d %d", &wx, &wy) != 2)
	    fatal("bad 'SWIDTH'");
	if (wy != 0)
	    fatal("SWIDTH y value must be zero");

	getline(linebuf);
	if (sscanf( linebuf, "DWIDTH %d %d", &wx, &wy) != 2)
	    fatal("bad 'DWIDTH'");
	if (wy != 0)
	    fatal("DWIDTH y value must be zero");

	getline(linebuf);
	if (sscanf( linebuf, "BBX %d %d %d %d", &bw, &bh, &bl, &bb) != 4)
	    fatal("bad 'BBX'");
	if (bh < 0 || bw < 0)
	    fatal("character '%s' has a negative sized bitmap, %dx%d", charName, bw, bh);

	getline(linebuf);
	if (prefix(linebuf, "ATTRIBUTES")) {
	    for (p= linebuf + strlen("ATTRIBUTES "); (*p == ' ') || (*p == '\t');
		p++)
		/* empty for loop */ ;
	    attributes = hexbyte(p)<< 8 + hexbyte(p+2);
	    getline(linebuf);  /* set up for BITMAP which follows */
	} else
	    attributes= 0;
	if (!prefix(linebuf, "BITMAP"))
	    fatal("missing 'BITMAP'");

	bytesperrow = Bytes(bw);
	nbytes= bytesperrow * bh;
	bp= bbuf;
	for (row= 0; row < bh; row++) {
	    getline(linebuf);
	    p = linebuf;
	    if (strlen(p) & 1)
		fatal("odd number of characters in hex encoding");
	    for (ix= 0; ix < bytesperrow; ix++, p+=2)
		*bp++= hexbyte(p);
	}
	getline(linebuf);
	if (!prefix(linebuf, "ENDCHAR"))
	    fatal("missing 'ENDCHAR'");
	AddGlyph(font, enc, wx, bh+bb, -bb, -bl, bw+bl, nbytes, bbuf, charName, attributes);
	getline(linebuf);              /* get STARTCHAR or ENDFONT */
    }

    if (nchars != -1)
	fatal("%d too few characters", nchars+1);
    if (prefix(linebuf, "STARTCHAR"))
	fatal("more characters than specified");
    if (!prefix(linebuf, "ENDFONT"))
	fatal("missing 'ENDFONT'");
    if (nchars != -1)
	fatal("%d too few characters", nchars+1);
/*
    if (nGl == 0)
	fatal("No characters with valid encodings");
*/
    return font;
}

void FreeGlyph(gp)
Glyph *gp;
{
    if (gp->bits)
	free(gp->bits);
    free(gp);
}

void FreeFont(font)
Font *font;
{
    register int i;
    
    if (font == 0)
	return;
    
    for (i= 0; i < MAXCHARS; i++)
	if (font->glyphs[i])
	     FreeGlyph(font->glyphs[i]);
    if (font->props) {
	for (i= 0; i < font->nprops; i++) 
	    if (font->props[i])
		free(font->props[i]);
	free(font->props);
    }
    if (font->ncomments) {
	for (i= 0; i < font->ncomments; i++) 
	    if (font->comments[i])
		free(font->comments[i]);
	free(font->comments);
    }
    if (font->name)
	free(font->name);
    free(font);
}

unsigned int mask[]= { 128, 64, 32, 16, 8, 4, 2, 1 };

BitBlt(to, tobytesperline, tox, toy, from, frobytesperline, fromx, fromy, w, h)
byte *to, *from;
int tobytesperline, frobytesperline, tox, toy, fromx, fromy, w, h;
{
    register int x, y;
    
    for (y= 0; y < h; y++)
	for (x= 0; x < w; x++)
	    if (getbit(from, frobytesperline, fromx+x, fromy+y))
		putbit(to, tobytesperline, tox+x, toy+y);
}

void GlyphBBox(font, i)
Font *font;
int i;
{
    register Glyph *glyph;
    register byte *cp;
    register int x, y, ox, oy, w, h, down, right;
    int nbytes, tobytesperline, bytesperline, wd, ht;
    byte buf[1000], bbuf[10000];
    
    if ((glyph= font->glyphs[i]) == 0)
	return;
	
    cp= glyph->bits;
    bytesperline= glyph->bytesperline;
    wd= glyph->left+glyph->right;
    ht= glyph->up+glyph->down;
    
    for (y= 0; y < ht; y++)
	for (x= 0; x < wd; x++)
	    if (getbit(cp, bytesperline, x, y))
		goto out;
out:
    oy= y;
    
    for (y= ht-1; y >= 0; y--)
	for (x= 0; x < wd; x++)
	    if (getbit(cp, bytesperline, x, y))
		goto out2;
out2:
    h= y-oy+1;
    down= ht-1-y;
    
    bzero(buf, bytesperline);
    for (y= oy; y < oy+h+1; y++)
	for (x= 0; x < bytesperline; x++)
	    buf[x] |= cp[y*bytesperline+x];
	    
    for (x= 0; x < wd; x++)
	if (getbit(buf, 0, x, 0))
	    break;
    ox= x;
    
    for (x= wd-1; x >= 0; x--)
	if (getbit(buf, 0, x, 0))
	    break;
    w= x-ox+1;
    right= wd-1-x;
    
    if (w < 0 || h < 0) {
	AddGlyph(font, i, glyph->width, 0, 0, 0, 0, 0, 0, glyph->name, glyph->attr);
    } else {
	tobytesperline= Bytes(w);
	nbytes= tobytesperline * h;
	bzero(bbuf, nbytes);
	BitBlt(bbuf, tobytesperline, 0, 0, glyph->bits, bytesperline, ox, oy, w, h);
	AddGlyph(font, i, glyph->width, glyph->up-oy, glyph->down-down, glyph->left-ox,
			    glyph->right-right, nbytes, bbuf, glyph->name, glyph->attr);
    }
}

void FontBBox(font)
Font *font;
{
    register int i;
    
    if (font == 0)
	return;
	
    for (i= 0; i < MAXCHARS; i++)
	GlyphBBox(font, i);
}

void RemoveGlyph(font, i)
Font *font;
int i;
{
    if (font && font->glyphs[i]) {
	FreeGlyph(font->glyphs[i]);
	font->glyphs[i]= 0;
    }
}

void Encode(font)
Font *font;
{
    RemoveGlyph(font, 176);
    RemoveGlyph(font, 177);
    RemoveGlyph(font, 178);
    RemoveGlyph(font, 179);
    RemoveGlyph(font, 181);
    RemoveGlyph(font, 182);
    RemoveGlyph(font, 183);
    RemoveGlyph(font, 184);
    RemoveGlyph(font, 185);
    RemoveGlyph(font, 186);
    RemoveGlyph(font, 189);
    RemoveGlyph(font, 195);
    RemoveGlyph(font, 196);
    RemoveGlyph(font, 197);
    RemoveGlyph(font, 198);
    RemoveGlyph(font, 215);
    RemoveGlyph(font, 218);
    RemoveGlyph(font, 240);
}

int IsFixed(font)
Font *font;
{
    int i, w;
    Glyph *gp;
    
    gp= font->glyphs['a'];
    if (gp) {
	w= gp->width;
	for (i= 'b'; i < 'z'; i++)
	    if (gp= font->glyphs[i])
		if (gp->width != w)
		    return 0;
	for (i= 'A'; i < 'Z'; i++)
	    if (gp= font->glyphs[i])
		if (gp->width != w)
		    return 0;
    }
    return 1;
}

Clean(font)
Font *font;
{
    int i, w;
    Glyph *gp;
    
    w= font->glyphs['a']->width;
    for (i= 0; i < MAXCHARS; i++) {
	if (gp= font->glyphs[i]) {
	    if (gp->left > 0 || (gp->left+gp->right > w)) {
		fprintf(stderr, "removing: %s\n", adobename(i));
		FreeGlyph(font->glyphs[i]);
		font->glyphs[i]= 0;
	    } else
		gp->width= w;
	}
    }
}


