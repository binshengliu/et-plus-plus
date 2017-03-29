//$PostScriptPort$

#include <stdio.h>
#include <osfcn.h>

#include "PostScriptPort.h"
#include "Error.h"
#include "System.h"
#include "String.h"
#include "DevBitmap.h"

const int MaxFonts      =   50,
	  CharsPerFont  =   256,
	  BitsPerWord   =   sizeof(int)*8;

const char *PsTmp   =   "etprt",
	   *LpPath  =   "/usr/ucb/lpr -P%s",
	   *TmpPath =   "/usr/tmp",
	   *PsPath  =   "%s/postscript";

struct fts {
    short fid;
    short face;
    int cbits[CharsPerFont/BitsPerWord];
};

inline void SetBit(int *a, int i)
{
    a[i/BitsPerWord] |= 1 << (i % BitsPerWord);
}

inline bool TestBit(int *a, int i)
{
    return (a[i/BitsPerWord] & (1 << (i % BitsPerWord))) != 0;
}

/*
extern "C" {
    int unlink(char*);
    char *tempnam(char*, char*);
}
*/

//---- PostScriptPort ----------------------------------------------------------

MetaImpl0(PostScriptPort);

PostScriptPort::PostScriptPort(char *name, PostScriptState *st) : PrintPort(name)
{
    pst= st;

    xorigin = 23.0;
    yorigin = (pst->portrait) ? 790.0 : 50.0;      

    xorigin= 50.0;
    yorigin= 820.0;
	
    fonts= new fts[MaxFonts];
    tofile= (name != 0);
    fid= 4711;
    pagecnt= 0;

    if (tofile)
	prfp= fopen(name, "w");
    else
	prfp= popen(form(LpPath, pst->prnname), "w");
     
    PrintTmpFile= tempnam((char*) TmpPath, (char*) PsTmp);
    if (PrintTmpFile == NULL)
	Error("PostScriptPort", "can't create tmpfile");
    if ((pfp= fopen(PrintTmpFile, "w")) == NULL)
	Error("PostScriptPort", "can't open tmpfile");
    
    fprintf(prfp, "%%!PS-Adobe-1.0\n");
    fprintf(prfp, "%%%%Creator: ET++\n");
}

PostScriptPort::~PostScriptPort()
{
    register FILE *rpfp;
    char buf[200];
    float xs, ys;
    
    FlushMyText();
    
    fclose(pfp);
    
    pfp= prfp;
    
    if ((rpfp= fopen(PrintTmpFile, "r")) == NULL) {
	Error("PostScriptPort::~PostScriptPort", "can't open tmpfile %s", PrintTmpFile);
	return;
    }
    
    Printf("%%%%Pages: %d\n", pagecnt);
    xs= (float) pst->resolution / 300.0 * 72.0;
    ys= (float) pst->resolution / 300.0 * 72.0;
    Printf("%%%%BoundingBox: %d %d %d %d\n",
	    (int)(xorigin + xs*bbox.origin.x - 0.5),
	    (int)(yorigin - ys*(bbox.origin.y + bbox.extent.y) - 0.5),
	    (int)(xorigin + xs*(bbox.origin.x + bbox.extent.x) + 0.5),
	    (int)(yorigin - ys*bbox.origin.y + 0.5));
    Printf("%%%%EndComments\n");
    if (pst->smooth)
	loadlib(prfp, "smooth");
    loadlib(prfp, "et");
    Printf("ET++Dict begin\n");
    flushfonts();
    
    Printf("/init {\n");
    Printf("%f %f translate\n", xorigin, yorigin);
    if (! pst->portrait)
	Printf("90 rotate\n");
    Printf("%f 300 div 72 mul %f 300 div 72 mul neg scale\n",
			     (float) pst->resolution, (float) pst->resolution);
    Printf("} def\n");

    Printf("%%%%EndProlog\n");

    while (fgets(buf, 200, rpfp) != NULL)
	fputs(buf, prfp);
    fclose(rpfp);
	
    unlink(PrintTmpFile);

    Printf("%%%%Trailer\n");
    Printf("end\n");
    
    if (tofile)
	fclose(prfp);
    else
	pclose(prfp);
	
    SafeDelete(fonts);
}

//---- Open, Close -------------------------------------------------------------

void PostScriptPort::DevOpenPage(int pn)
{
    lastpsz= -1;
    lbfont= 0;
    currpage= pn;
    Printf("%%%%Page: %d %d\n", pagecnt+1, currpage);
    Printf("save\n");
    Printf("init\n");
}

void PostScriptPort::DevClosePage()
{
    Printf("Restore\n");
    if (pagebbox.IsNotEmpty()) {
	Printf("showpage\n");
	pagecnt++;
	if (gDebug)
	    DevStrokeRect(0, &pagebbox);
    }
}

//---- clipping ----------------------------------------------------------------

void PostScriptPort::DevClip(Rectangle r, Point)
{
    Printf("%r SetClip\n", &r);
    lastpsz= -1;
    lbfont= 0;
}

//---- drawing -----------------------------------------------------------------

void PostScriptPort::DevStrokeLine(int pensize, Rectangle *r,
		GrLineCap cap, Point p1, Point p2)
{
    SetPenSize(pensize);
    Merge(r);
    Printf("%p %p %d StrokeArrow\n", &p1, &p2, cap);
}

//---- text batching -----------------------------------------------------------

bool PostScriptPort::DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, 
									    Point)
{
    if (isnew) {                // first
	bcnt= 0;
	lfont= fdp;
    } else if (fdp != lfont || delta != gPoint0)
	return TRUE;
    bbuf[bcnt++]= c;
    return FALSE;
}

void PostScriptPort::DevShowTextBatch(Rectangle *r, Point pos)
{
    int i, blanks= 0;
    register byte c;
    
    if (lbfont != lfont) {
	fix= EnrollFont(lfont);
	Printf("%s-%s %d scalesetfont\n", gFontManager->Name(lfont->Fid()), 
			 gFontManager->StyleString(lfont->Face()), lfont->Size());
	lbfont= lfont;
    }
    Merge(r);
    Printf("%p m(", &pos);
    for (i= 0; i < bcnt; i++) {
	c= bbuf[i];
	SetBit(fonts[fix].cbits, c);
	Printf("%c", c);
	if (c == ' ')
	    blanks++;
    }
    //if (blanks > 0)
    //    Printf(") %d %d bsshow\n", r->extent.x, blanks);
    //else
    //    Printf(") %d sshow\n", r->extent.x);        
    Printf(") show\n");        
} 

void PostScriptPort::DevShowBitmap(Rectangle *r, Bitmap *bm)
{
    int bytes, x, y, by, lines, yy;
    byte bbp[300];
    Point e= bm->Size();
    DevBitmap *bmp= bm->GetDevBitmap();
    
    Merge(r);
    if (!gDebug && pst->smooth) {
	bytes= bm->ShortsPerLine() * 2;
	lines= 1250 / bytes;
	
	Printf("%d %d div %d %d div %p %d %d SmoothBitmapStart\n",
		   r->extent.x, e.x, r->extent.y, e.y, &r->origin, bytes, e.x);
	for (by= 0, y= 0; y < e.y; y+= by) {
	    by= min(lines, e.y-y);
	    Printf("%d Band\n", by);
	    for(yy= y-2; yy < y+by+2; yy++) {
		for (x= 0; x < bytes; x++)
		    Printf("%x", bmp->GetByte(x, yy));
		Printf("\n");
	    }
	}
	Printf("SmoothBitmapEnd\n");
    } else {
	Printf("%r %p %d Bitmap\n", r, &e, bmp->Depth());
	if (bmp->Depth() == 1) {
	    bytes= bmp->BytesPerLine(); // postscript images are Byte aligned
	    for (y= 0; y < e.y; y++) {
		for (x= 0; x < bytes; x++)
		    Printf("%x", bmp->GetByte(x, y));
		Printf("\n");
	    }
	} else {
	    bytes= bmp->BytesPerLine(); /* postscript images are Byte aligned */
	    int cmssize= bmp->GetColormapSize();
	    int *grey= new int[cmssize];
	    for (int i= 0; i < cmssize; i++) {
		int r, g, b;
		bmp->GetColormapEntry(i, &r, &g, &b);
		RGBColor c(r, g, b);
		grey[i]= c.AsGreyLevel();
	    }
	    for (y= 0; y < e.y; y++) {
		for (x= 0; x < bytes; x++)
		    Printf("%x", grey[bmp->GetPixel(x, y)]);
		Printf("\n");
	    }
	    delete grey;
	}
    }
}

void PostScriptPort::DevGiveHint(int code, int, void *vp)
{
    if (code == eHintPostScript)
	Printf("%s\n", (char*) vp);
}

void PostScriptPort::DrawObject(char type, Rectangle *r, Point e, int psz, GrLineCap)
{
    char *st= "?";
    
    Merge(r);
    
    SetPenSize(psz);
    if (r)
	Printf("%r ", r);
    if (e != gPoint0)
	Printf("%p ", &e);
    switch (type) {
    case 'b':
    case 'B':
	st= "Rect";
	break;
    case 'o':
    case 'O':
	st= "Oval";
	break;
    case 'r':
    case 'R':
	st= "RoundRect";
	break;
    case 'w':
    case 'W':
	st= "Wedge";
	break;
    }
    Printf("%s%s\n", psz >= 0 ? "Stroke" : "Fill", st);
}

void PostScriptPort::DrawPolygon(char, Rectangle *r,
		Point *pts, int npts, GrPolyType t, int psz, GrLineCap)
{
    Merge(r);
    
    SetPenSize(psz);
    Printf("%r %d %d %p [\n", r, (pts[0] == pts[npts-1]) ? 1 : 0,
				 (t & ePolyBezier) ? 1 : 0, &r->extent);
    for (int i= 0; i < npts; i++)
	Printf("%p ", &pts[i]);
	
    Printf("\n] %sPoly\n", (psz >= 0) ? "Stroke" : "Fill");
}

//---- output functions ---------------------------------------------------------

/*
 %c  PostScript character       byte
 %f  floating point             float
 %d  integer                    integer
 %x  2-digit hex num            byte
 %r  Rectangle                  Rectangle*
 %p  Point                      Point
 %s  0-terminated string        char*
*/

void PostScriptPort::Printf(char *va_(format), ...)
{
    static char *t= "0123456789abcdef";
    register char c;
    unsigned int b;
    char *s;
    Point *p;
    int i;
    double d;
    Rectangle *r;
    va_list ap;
    
    va_start(ap,va_(format));
    char *fmt= va_(format);
    
    while (c= *fmt++) {
	switch (c) {
	case '%':
	    switch (c= *fmt++) {
	    case 'c':
		b= va_arg(ap, unsigned int);
		if (b >= ' ' && b <= '~') {
		    if (b == '\\' || b == '(' || b == ')')
			fputc('\\', pfp); 
		    fputc(b, pfp);
		} else
		    fprintf(pfp, "\\%03o", b);
		break;
	    case 'f':
		d= va_arg(ap, double);
		fprintf(pfp, "%.3f", d);
		break;
	    case 'd':
		i= va_arg(ap, int);
		fprintf(pfp, "%d", i);
		break;
	    case 'x':
		b= va_arg(ap, unsigned int);
		fputc(t[(b&0xff)>>4], pfp);
		fputc(t[b&0xf], pfp);
		break;
	    case 's':
		s= va_arg(ap, char*);
		while (c= *s++)
		    fputc(c, pfp);
		break;
	    case 'r':
		r= va_arg(ap, Rectangle*);
		fprintf(pfp, "%d %d %d %d", r->origin.x, r->origin.y, r->extent.x,
								  r->extent.y);
		break;
	    case 'p':
		p= va_arg(ap, Point*);
		fprintf(pfp, "%d %d", (int) p->x, (int) p->y);
		break;
	    default:
		fputc(c, pfp);
		break;
	    }
	    break;
	default:
	    fputc(c, pfp);
	    break;
	}
    }
    va_end(ap);
}

//---- misc functions ---------------------------------------------------------

void PostScriptPort::DevSetGrey(float f)
{
    Printf("%f SetGray\n", f);
}

void PostScriptPort::DevSetPattern(DevBitmap *bmp)
{
    Printf("<");
    for (int y= 0; y < bmp->Size().y; y++)
	Printf("%x", bmp->GetByte(0, y));
    Printf("> SetPattern\n");
}

void PostScriptPort::DevSetOther(int pid)
{
    Printf("%f SetGray\n", pid == 1 ? 1.0 : 0.0);
}

void PostScriptPort::SetPenSize(int pensize)
{
    if (pensize < 0 || (pensize == lastpsz))
	return;
    lastpsz= pensize;
    Printf("%d SetPenSize\n", pensize);
}

int PostScriptPort::EnrollFont(FontPtr fd)
{
    register int i;
    
    for (i= 0; i<maxfont; i++)
	if (fd->Fid() == fonts[i].fid && fd->Face() == fonts[i].face)
	    return i;
    fonts[i].fid= fd->Fid();
    fonts[i].face= fd->Face();
    maxfont++;
    return i;
}

static bool Reencode(int family)
{
    switch (family) {
    case eFontTimes:
    case eFontHelvetica:
    case eFontCourier:
    case eFontAvantgarde:
    case eFontBookman:
    case eFontSchoolbook:
    case eFontNHelvetica:
    case eFontPalatino:
    case eFontChancery:
	return TRUE;
    default:
	return FALSE;
    }
}

void PostScriptPort::flushfonts()
{
    GrFont family;
    GrFace face;
    char fbuf[40];
    
    for (int i= 0; i < maxfont; i++) {
	family= (GrFont)fonts[i].fid;
	face= (GrFace)fonts[i].face;
	
	sprintf(fbuf, "%s-%s", gFontManager->Name(family), 
					    gFontManager->StyleString(face));
	
	if (! gFontManager->IsPSFont(family)) {
	    downloadfont(new_Font(family, 99, face), fbuf, fonts[i].cbits);
	    fprintf(pfp, "/%s /%s findfont def\n", fbuf, fbuf);
	} else if (Reencode(family))
	    fprintf(pfp, "/%s Reencode\n", fbuf);
    }
}

void PostScriptPort::downloadfont(FontPtr fd, char *fname, int *ia)
{
    int z, i, x, y, ps, llx, lly, urx, ury, bytesperline;
    DevBitmap *dbm;
    Point offset, size;
    
    llx= lly= 9999;
    urx= ury= 0;
    ps= fd->Size();

    Printf("/%s %d %d [\n", fname, fid++, ps);
    for (z= i= 0; i < 256; i++) {
	if (! TestBit(ia, i))
	    continue;
	if ((dbm= fd->CharAsBitmap(i, &offset)) == 0)
	    continue;
	size= dbm->Size();
       
	llx= min(llx, offset.x);
	lly= min(lly, offset.y);
	urx= max(urx, offset.x+size.x);
	ury= max(ury, offset.y+size.y);
	
	Printf("<%x%x%x%x%x%x", i, fd->Width(i), size.x, size.y, offset.x,
								offset.y-size.y);
	
	if (pst->smooth) {
	    bytesperline= (((size.x+2)-1)/16+1)*2;
	    for (y= -2; y < size.y+2; y++) {
		for (x= 0; x < bytesperline; x++)
		    Printf("%x", dbm->GetByte(x, y));
	    }
	} else {
	    bytesperline= (size.x-1) / 8 + 1;
	    for (y= 0; y < size.y; y++)
		for (x= 0; x < bytesperline; x++)
		    Printf("%x", dbm->GetByte(x, y));
	}
		
	Printf(">\n");
	z++;
	delete dbm;
    }
    Printf("] %d %d %d %d %d new%sbitmapfont\n", z, llx, lly, urx, ury,
						pst->smooth ? "smoothed" : "");
}

extern FILE *OpenFile(const char *name, const char *rw, const char *va_(path)= 0, ...);

void PostScriptPort::loadlib(FILE *ofp, char *name)
{
    if (pst->prolog) {
	char buf[300];
	FILE *fin= OpenFile(form("%s.ps", name), "r", ".", form(PsPath, gEtDir), 0);
	while (fgets(buf, 200, fin) != NULL)
	    fputs(buf, ofp);
    } else
	fprintf(ofp, "%%%%Include: %s\n", name);
}
