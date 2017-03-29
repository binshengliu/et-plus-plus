#ifndef Port_First
#ifdef __GNUG__
#pragma once
#endif
#define Port_First

#include "System.h"
#include "Types.h"
#include "Point.h"
#include "Metric.h"
#include "Rectangle.h"
#include "Font.h"
#include "Ink.h"
#include "Bitmap.h"

const int MaxTextBatchCnt= 400;

enum GrHints {
    eHintLock,
    eHintUnlock,
    eHintBatch,
    eHintUnbatch,
    eHintTextBatch,
    eHintTextUnbatch,
    eHintPostScript,
    eHintPic,
    eHintFlush
};

enum GrLineCap {
    eDefaultCap= 0,
    eStartArrow= 1,
    eEndArrow  = 2
};

enum GrPolyType {
    ePolyDefault = 0,
    ePolyBezier  = 1
};

enum GrCursor {
    eCrsNone       =  0, 
    eCrsArrow      =  1,
    eCrsBoldArrow  =  2,
    eCrsCross      =  3,
    eCrsBoldCross  =  4,
    eCrsUpArrow    =  5,
    eCrsDownArrow  =  6,
    eCrsRightArrow =  7,
    eCrsLeftArrow  =  8,
    eCrsHourglass  =  9,
    eCrsPen        = 10,
    eCrsMoveHand   = 11,
    eCrsHand       = 12,
    eCrsCrossHair  = 13,
    eCrsMoveStretch= 14,
    eCrsIBeam      = 15,
    eCrsLeftRightArrow  = 18,
    eCrsUpDownArrow= 19
};

extern char gRubout;
extern char gBackspace;
extern bool fullscreen;
extern class Port *tbport;
extern class Port *port;

#include "Token.h"

class ImageCache {
public:
    class Bitmap *b;
    Rectangle vr, r;
    bool invalid;
public:
    ImageCache();
    ~ImageCache();
    bool Open(Rectangle r);
    void Close();
    void Invalidate()
	{ invalid= TRUE; }
};

class Port : public SysEvtHandler {    
public:
    /* graphic state */
    Point origin;
    Rectangle cliprect;
    Ink *fillink;
    
    /* pen state */
    Point penpos;
    short pensize;
    Ink *penink;
    GrLineCap pencap;
    
    /* text state */
    Point textpos;
    Ink *textink;
    Font *textfont;
    
    /* other */
    Ink *lastink;
    bool hascolor;
	  
    void flushtext();

protected:
    Port();
    
    bool Visible(Ink *ink, Rectangle &r)
	{ return (bool) (ink != gInkNone && (fullscreen || cliprect.Intersects(r))); }

    void InstallInk(Ink *ink)
	 { if (ink != lastink) { ink->SetInk(this); lastink= ink; } }

    void FlushMyText()
	{ if (tbport == this) flushtext(); }
		
    void FlushAnyText()
	{ if (tbport) tbport->flushtext(); }

    virtual void DrawObject(char, Rectangle*, Point, int, GrLineCap);
    virtual void DrawPolygon(char, Rectangle*, Point*, int, GrPolyType, int, GrLineCap);

public:
    MetaDef(Port);
    virtual ~Port();
    void PortInit();

    //---- device dependent primitives ------------------------------------------
    virtual void DevClip(Rectangle, Point);
    virtual void DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point);
    virtual void DevStrokeRect(int, Rectangle*);
    virtual void DevFillRect(Rectangle*);
    virtual void DevStrokeOval(int, Rectangle*);
    virtual void DevFillOval(Rectangle*);
    virtual void DevStrokeRRect(int, Rectangle*, Point);
    virtual void DevFillRRect(Rectangle*, Point);
    virtual void DevStrokeWedge(int, GrLineCap, Rectangle*, int, int);
    virtual void DevFillWedge(Rectangle*, int, int);
    virtual void DevStrokePolygon(Rectangle*, Point*, int, GrPolyType t, int, GrLineCap);
    virtual void DevFillPolygon(Rectangle*, Point*, int, GrPolyType t);
    virtual void DevShowBitmap(Rectangle*, struct Bitmap*);
    virtual bool DevShowChar(FontPtr, Point, byte, bool, Point);
    virtual void DevShowTextBatch(Rectangle*, Point);
    virtual void DevShowPicture(Rectangle *r, class Picture *pic);
    virtual void DevGiveHint(int code, int len, void *vp);
    virtual void DevSetPattern(struct DevBitmap*);
    virtual bool DevSetColor(RGBColor*);
    virtual void DevSetOther(int);
    virtual void DevSetGrey(float);
    virtual bool DevImageCacheBegin(ImageCache *bb, Rectangle r);
    virtual void DevImageCacheEnd(ImageCache *bb);
    //---- end of device dependent primitives ----------------------------------


    //---- device independent primitives ---------------------------------------
    virtual GrCursor SetCursor(GrCursor c);
    virtual GrCursor SetWaitCursor(unsigned int d= 400, GrCursor c= eCrsHourglass);
    virtual GrCursor GetCursor();

    void Clip(Rectangle, Point);
    void StrokeLine(Ink*, int, GrLineCap, Point, Point);
    void StrokeRect(Ink*, int, Rectangle*);
    void FillRect(Ink*, Rectangle*);
    void StrokeOval(Ink*, int, Rectangle*);
    void FillOval(Ink*, Rectangle*);
    void StrokeRRect(Ink*, int, Rectangle*, Point);
    void FillRRect(Ink*, Rectangle*, Point);
    void StrokeWedge(Ink*, int, GrLineCap, Rectangle*, int, int);
    void FillWedge(Ink*, Rectangle*, int, int);
    void StrokePolygon(Point, Ink*, Point*, int, GrPolyType, int, GrLineCap);
    void FillPolygon(Point, Ink*, Point*, int, GrPolyType);
    void ShowTextBatch(Ink*, FontPtr, Rectangle*, Point, int, byte*);
    void ShowString(FontPtr, Ink*, Point, byte*, int);
    void ShowBitmap(Ink*, Rectangle*, struct Bitmap*);
    void GiveHint(int code, int len, void *vp);
    void ShowPicture(Rectangle*, Picture*);
    void SetColor(RGBColor*);
    //---- end of device independent primitives --------------------------------

    //---- clipping ------------------------------------------------------------
    Point GetOrigin()
	{ return origin; }
    Rectangle GetCliprect()
	{ return cliprect; }

    //---- Fill -----------------------------------------------------------------
    void SetNormal()
	{ fillink= gInkBlack; }
    void SetPattern(Ink *p)     // obsolete
	{ fillink= p; }
    void SetInk(Ink *p)
	{ fillink= p; }

    //---- Pen and Lines --------------------------------------------------------
    void SetPenNormal();
    void SetPenInk(Ink *p)  // obsolete
	{ penink= p; }
    void SetPenPattern(Ink *p)
	{ penink= p; }
    void SetLineCap(GrLineCap lc)
	{ pencap= lc; }
    void SetPenSize(int w)
	{ pensize= w; }
    void Moveto(Point p)
	{ penpos= p; }
    void Moveby(Point p)
	{ penpos+= p; }
    void Line(Point p1, Point p2)
	{ StrokeLine(penink, pensize, pencap, p1, p2); }
    void PaintLine(Ink *ink, GrLineCap cap, Point p1, Point p2)
	{ StrokeLine(ink, pensize, cap, p1, p2); }
    void Lineto(Point);

    //---- Text -----------------------------------------------------------------
    void SetTextNormal();
    int ShowChar(Font *fdp, Ink *ink, Point pos, byte c);
    void DrawChar(byte);
    void DrawString(byte*, int);
    void SetFamily(GrFont);
    void SetFont(Font *fd)
	{ textfont= fd; }
    Font *GetFont()
	{ return textfont; }
    void SetSize(int);
    void SetFace(GrFace);
    void SetTextPattern(Ink *p)
	{ textink= p; }
    void SetTextInk(Ink *p)
	{ textink= p; }
    Point GetTextPos()   
	{ return textpos; }
    void TextMoveto(Point p)
	{ textpos= p; }
    void TextMoveby(Point p)
	{ textpos+= p; }
    void TextHMoveby(int h)
	{ textpos.x+= h; }
    void TextVMoveby(int v)
	{ textpos.y+= v; }
	
    bool HasColor()
	{ return hascolor; }
};

typedef Port *PortDesc;

//---- Port syntactic sugar ----------------------------------------------------

inline void GrSetPort(PortDesc newport)
    { port= newport; }
inline PortDesc GrGetPort()
    { return port; }

//---- cursor -------------------------------------------------------------------
inline GrCursor GrSetCursor(GrCursor c)
    { return port->SetCursor(c); }
inline GrCursor GrGetCursor()
    { return port->GetCursor(); }
inline GrCursor GrSetWaitCursor(unsigned int d= 400, GrCursor c= eCrsHourglass)
    { return port->SetWaitCursor(d, c); }

//---- clipping -----------------------------------------------------------------

inline void GrSetClip(Rectangle r, Point o)
    { port->Clip(r, o); }

//---- implementation hints -----------------------------------------------------
inline void GrGiveHint(int n, int l= 0, void *v= 0)
    { port->GiveHint(n, l, v); }

//---- pens ---------------------------------------------------------------------
inline void GrSetPenSize(int w)
    { port->SetPenSize(w); }
inline void GrSetLineCap(GrLineCap lc)
    { port->SetLineCap(lc); }
inline void GrSetPenPattern(Ink *p) // obsolete
    { port->SetPenInk(p); }
inline void GrSetPenInk(Ink *p)
    { port->SetPenInk(p); }
inline void GrSetPenNormal()
    { port->SetPenNormal(); }

//---- lines --------------------------------------------------------------------
inline void GrMoveto(Point p)
    { port->Moveto(p); }
inline void GrLineto(Point p)
    { port->Lineto(p); }
inline void GrLine(Point p1, Point p2)
    { port->StrokeLine(port->penink, port->pensize, port->pencap, p1, p2); }
inline void GrPaintLine(Ink *ink, int psz, GrLineCap cap, Point p1, Point p2)
    { port->StrokeLine(ink, psz, cap, p1, p2); }
inline void GrStrokeLine(Ink *ink, GrLineCap cap, Point p1, Point p2)
    { port->StrokeLine(ink, port->pensize, cap, p1, p2); }
inline void GrInvertLine(Point p1, Point p2)
    { port->StrokeLine(gInkXor, port->pensize, port->pencap, p1, p2); }
inline void GrEraseLine(Point p1, Point p2)
    { port->StrokeLine(gInkWhite, port->pensize, port->pencap, p1, p2); }

//---- fill ---------------------------------------------------------------------
inline void GrSetPattern(Ink *p)    // obsolete
    { port->SetPattern(p); }
inline void GrSetInk(Ink *p)
    { port->SetInk(p); }

//---- rectangle ----------------------------------------------------------------
inline void GrFillRect(Rectangle r)
    { port->FillRect(port->fillink, &r); }

inline void GrStrokeRect(Rectangle r)
    { port->StrokeRect(port->penink, port->pensize, &r); }

inline void GrEraseRect(Rectangle r)
    { port->FillRect(gInkWhite, &r); }

inline void GrInvertRect(Rectangle r)
    { port->FillRect(gInkXor, &r); }

inline void GrPaintRect(Rectangle r, Ink *ink)
    { port->FillRect(ink, &r); }

//---- round rectangles --------------------------------------------------------

inline void GrStrokeRoundRect(Rectangle r, Point dia)
    { port->StrokeRRect(port->penink, port->pensize, &r, dia); }

inline void GrFillRoundRect(Rectangle r, Point dia)
    { port->FillRRect(port->fillink, &r, dia); }

inline void GrEraseRoundRect(Rectangle r, Point dia)
    { port->FillRRect(gInkWhite, &r, dia); }

inline void GrInvertRoundRect(Rectangle r, Point dia)
    { port->FillRRect(gInkXor, &r, dia); }

inline void GrPaintRoundRect(Rectangle r, Point dia, Ink *ink)
    { port->FillRRect(ink, &r, dia); }

//----- ovals ------------------------------------------------------------------
inline void GrStrokeOval(Rectangle r)
    { port->StrokeOval(port->penink, port->pensize, &r); }

inline void GrFillOval(Rectangle r)
    { port->FillOval(port->fillink, &r); }

inline void GrEraseOval(Rectangle r)
    { port->FillOval(gInkWhite, &r); }

inline void GrInvertOval(Rectangle r)
    { port->FillOval(gInkXor, &r); }

inline void GrPaintOval(Rectangle r, Ink *ink)
    { port->FillOval(ink, &r); }

//----- wedges -----------------------------------------------------------------

inline void GrStrokeWedge(Rectangle r, int s, int e)
    { port->StrokeWedge(port->penink, port->pensize, port->pencap, &r, s, e); }

inline void GrFillWedge(Rectangle r, int s, int e)
    { port->FillWedge(port->fillink, &r, s, e); }

inline void GrEraseWedge(Rectangle r, int s, int e)
    { port->FillWedge(gInkWhite, &r, s, e); }

inline void GrInvertWedge(Rectangle r, int s, int e)
    { port->FillWedge(gInkXor, &r, s, e); }

inline void GrPaintWedge(Rectangle r, int s, int e, Ink *ink)
    { port->FillWedge(ink, &r, s, e); }

//---- bitmaps -----------------------------------------------------------------

inline void GrShowBitMap(Rectangle r, Bitmap *bmp)
    { port->ShowBitmap(port->fillink, &r, bmp); }

inline void GrPaintBitMap(Rectangle r, Bitmap *bmp, Ink *ink)
    { port->ShowBitmap(ink, &r, bmp); }

inline void GrInvertBitMap(Rectangle r, Bitmap *bmp)
    { port->ShowBitmap(gInkXor, &r, bmp); }

//--- Polygon ------------------------------------------------------------------

inline void GrStrokePolygon(Point at, Point *pts, int npts, GrPolyType t)
    { port->StrokePolygon(at, port->penink, pts, npts, t, port->pensize,
								port->pencap); }

inline void GrFillPolygon(Point at, Point *pts, int npts, GrPolyType t)
    { port->FillPolygon(at, port->fillink, pts, npts, t); }

inline void GrErasePolygon(Point at, Point *pts, int npts, GrPolyType t)
    { port->FillPolygon(at, gInkWhite, pts, npts, t); }

inline void GrInvertPolygon(Point at, Point *pts, int npts, GrPolyType t)
    { port->FillPolygon(at, gInkXor, pts, npts, t); }

inline void GrPaintPolygon(Point at, Ink *ink, Point *pts, int npts, GrPolyType t)
    { port->FillPolygon(at, ink, pts, npts, t); }


//---- text --------------------------------------------------------------------
inline void GrSetFont(Font *fd)
    { port->SetFont(fd); }
inline Font *GrGetFont()
    { return port->GetFont(); }
inline void GrSetFamily(GrFont f)
    { port->SetFamily(f); }
inline void GrSetSize(int s)
    { port->SetSize(s); }
inline void GrSetFace(GrFace f)
    { port->SetFace(f); }
inline void GrDrawChar(byte c)
    { port->DrawChar(c); }
inline void GrDrawString(byte *s, int l= -1)
    { port->DrawString(s, l); }
inline void GrShowString(Font *fd, Ink *ink, Point pos, byte *text, int l= -1)
    { port->ShowString(fd, ink, pos, text, l); }
inline void GrSetTextPattern(Ink *fillink)  // obsolete
    { port->SetTextPattern(fillink); }
inline void GrSetTextInk(Ink *fillink)
    { port->SetTextInk(fillink); }
inline void GrTextMoveto(Point p)
    { port->TextMoveto(p); }
inline void GrTextAdvance(int h)
    { port->TextHMoveby(h); }
inline Point GrGetTextPos()
    { return port->GetTextPos(); }

//---- pictures ----------------------------------------------------------------
inline void GrShowPicture(Rectangle r, Picture *pic)
    { port->ShowPicture(&r, pic); }

//---- other -------------------------------------------------------------------
inline bool GrHasColor()
    { return port->HasColor(); }

//---- Show wait cursor --------------------------------------------------------

class GrShowWaitCursor {
    GrCursor oldCursor;
public:
    GrShowWaitCursor()
	{ oldCursor= GrSetWaitCursor(); }
    ~GrShowWaitCursor()
	{ GrSetCursor(oldCursor); }
};

#endif Port_First
