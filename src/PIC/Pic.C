//$PicPort,PicPrinter$

#include <stdio.h>

#include "Port.h"
#include "Pic.h"
#include "Error.h"
#include "String.h"
#include "PrintPort.h"

//---- PicPort -----------------------------------------------------------------

class PicPort: public PrintPort {    
    FILE *pic;
    float xscale;
    int bcnt, lastfn, lastps;
    FontPtr lbfont, lfont;
    byte bbuf[MaxTextBatchCnt];
    
    void PrintPenSize(int psz);
    void PrintTextPs(int ps);
    void PrintFont(FontPtr fp);
public:

    PicPort(char *name= 0);
    ~PicPort();
    
    void DevClip(Rectangle, Point);
    void DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point);
    void DevShowBitmap(Rectangle*, struct Bitmap*);
    bool DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, Point);
    void DevShowTextBatch(Rectangle*, Point);
    void DevStrokeOval(int, Rectangle*);
    void DevStrokeRRect(int, Rectangle*, Point);
    void DevStrokeRect(int, Rectangle*);
    void DevStrokeWedge(int, GrLineCap, Rectangle*, int, int);
    void DevStrokePolygon(Rectangle*, Point*, int, GrPolyType, int, GrLineCap);
    void DevGiveHint(int, int, void*);

    void DevOpenPage(int pn);
    void DevClosePage();
};

PicPort::PicPort(char *name) : PrintPort(name)
{
    lastfn= lastps= -99;
    
    xscale= 1.0;
    
    if (name == 0)
	name= "pic";
    
    if ((pic= fopen(name, "w")) == NULL) {
	Warning("PicPort", "can't open file %s; trying ./pic", name);
	if ((pic= fopen("pic", "w")) == NULL)
	    Fatal("PicPort", "can't open file \"./pic\"");
    }
}

PicPort::~PicPort()
{
    fclose(pic);
}

//---- Open, Close -------------------------------------------------------------

void PicPort::DevOpenPage(int)
{
    fprintf(pic, ".PS\n");
    fprintf(pic, "scale = 72.0 * %.3f\n", xscale);
    fprintf(pic, "dashwid = 4\n");
    fprintf(pic, "define Bitmap X box dashed wid $3 ht $4 with .nw at $1,$2 X\n");
    fprintf(pic, "define Text X $3 with .nw at $1,$2 ljust X\n");
    fprintf(pic, "define Line X line $1 from $2,$3 to $4,$5 X\n");
    fprintf(pic, "define Box X box wid $3 ht $4 with .nw at $1,$2 X\n");
    fprintf(pic, "define Arc X arc $1 wid $4 ht $5 with .nw at $2,$3 X\n");
    fprintf(pic, "define Ellipse X ellipse wid $3 ht $4 with .w at $1,$2-$4/2 X\n");
    fprintf(pic, "define AL X line $2 $3-$1; $2; arc rad $1/2; X\n");
    fprintf(pic, "define RBox X move to $1+$3,$2-$4+($5/2); AL($5,up,$4);AL($5,left,$3);AL($5,down,$4);AL($5,right,$3); X\n");
}

void PicPort::DevClosePage()
{
    fprintf(pic, ".PE\n");
}

//---- clipping ----------------------------------------------------------------

void PicPort::DevClip(Rectangle, Point)
{
}

//---- state -------------------------------------------------------------------

void PicPort::PrintPenSize(int psz)
{
    int ps= int(float(psz*24)/xscale+0.5);
    if (ps != lastps)
	fprintf(pic, ".ps %d\n", lastps= ps);
}

void PicPort::PrintTextPs(int pps)
{
    int ps= int(float(pps)/xscale+0.5);
    if (ps != lastps)
	fprintf(pic, ".ps %d\n", lastps= ps);
}

static char *Cap(GrLineCap cap)
{
    switch (cap) {
    case 1:
	return "->";
    case 2:
	return "<-";
    case 3:
	return "<->";
    }
    return "";
}

//---- graphic -----------------------------------------------------------------

void PicPort::DevStrokeLine(int psz, Rectangle *r, GrLineCap cap, Point p1, Point p2)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "Line(%s,%d,%d,%d,%d)\n", Cap(cap), p1.x, 700-p1.y, p2.x, 700-p2.y);
}

static char *TroffFontTab[]= {
    "R",    "B",    "I",    "D",
    "TT",   "TB",   "TI",   "TD",
    "H",    "HB",   "HI",   "HD",
    0,
};

void PicPort::PrintFont(FontPtr fp)
{
    int fn;
    
    switch (fp->Fid()) {
    case eFontAvantgarde:
    case eFontChicago:
    case eFontHelvetica:
	fn= 8;
	break;
    case eFontGeneva:
    case eFontCourier:
	fn= 4;
	break;
    case eFontTimes:
    case eFontBookman:
    case eFontPalatino:
    default:
	fn= 0;
	break;
    }
    fn+= fp->Face() & (eFaceBold | eFaceItalic);
    if (fn != lastfn)
	fprintf(pic, ".ft %s\n", TroffFontTab[lastfn= fn]);
}

static char *TroffChar(int c)
{
    static char st[2];
    
    switch (c) {
    case '\"':
	return "\\\"";
    case '\\':
	return "\\e";
    }
    st[0]= c;
    st[1]= '\0';
    return st;
}

bool PicPort::DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, Point)
{
    if (isnew) {                // first
	bcnt= 0;
	lfont= fdp;
    } else if (fdp != lfont || delta != gPoint0 || bcnt >= 200)
	return TRUE;
    bbuf[bcnt++]= c;
    return FALSE;
}

void PicPort::DevShowTextBatch(Rectangle *r, Point pos)
{
    Merge(r);
    PrintTextPs(lfont->Size());
    PrintFont(lfont);
    
    fprintf(pic, "Text(%d,%d,\"", pos.x, 700-pos.y);
    for (int i= 0; i < bcnt; i++)
	fprintf(pic, "%s", TroffChar(bbuf[i]));
    fprintf(pic, "\")\n");
} 

void PicPort::DevShowBitmap(Rectangle *r, Bitmap*)
{
    Merge(r);
    PrintTextPs(4);
    fprintf(pic, "Bitmap(%d,%d,%d,%d)\n", r->origin.x, 700 - r->origin.y,
						    r->extent.x, r->extent.y);
}

void PicPort::DevStrokeOval(int psz, Rectangle *r)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "Ellipse(%d,%d,%d,%d)\n", r->origin.x, 700 - r->origin.y,
						r->extent.x, r->extent.y);
}

void PicPort::DevStrokeRRect(int psz, Rectangle *r, Point dia)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "RBox(%d,%d,%d,%d,%d)\n", r->origin.x, 700 - r->origin.y,
					    r->extent.x, r->extent.y, dia.x);
}

void PicPort::DevStrokeRect(int psz, Rectangle *r)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "Box(%d,%d,%d,%d)\n", r->origin.x, 700 - r->origin.y,
					    r->extent.x, r->extent.y);
}

void PicPort::DevStrokeWedge(int psz, GrLineCap cap,
						Rectangle *r, int, int)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "Arc(%s,%d,%d,%d,%d)\n", Cap(cap), r->origin.x, 700 - r->origin.y,
					r->extent.x, r->extent.y);
}

void PicPort::DevStrokePolygon(Rectangle *r, Point *pts,
			    int npts, GrPolyType, int psz, GrLineCap)
{
    Merge(r);
    PrintPenSize(psz);
    fprintf(pic, "line from %d,%d ", r->origin.x+pts[0].x, r->origin.y+pts[0].y);
    for (int i= 1; i < npts; i++)
	fprintf(pic, "to %d,%d ", r->origin.x+pts[i].x, r->origin.y+pts[i].y);    
    fprintf(pic, "\n");
}

void PicPort::DevGiveHint(int code, int l, void *vp)
{
    if ((code == eHintPic) && (l > 0))
	fprintf(pic, "%s", vp);
}

//---- PicPrinter --------------------------------------------------------------

PicPrinter::PicPrinter() : Printer("Pic", FALSE)
{
}

class PrintPort *PicPrinter::MakePrintPort(char *name)
{
    return new PicPort(name);
}

Printer *NewPicPrinter()
{
    return new PicPrinter;
}
