//$ColorCell,PickerCommand,ColorWheel,ColorPicker,RGBPicker,HSVPicker$

#include "ColorPicker.h"

#include "ET++.h"
#include "ScrollBar.h"
#include "Slider2.h"
#include "WindowPort.h"
#include "BlankWin.h"
#include "Buttons.h"

const int cIdValSlider  =   cIdFirstUser + 1,
	  cIdRGBSlider  =   cIdFirstUser + 2,
	  cIdHSVPicker  =   cIdFirstUser + 5,
	  cIdRGBPicker  =   cIdFirstUser + 6,
	  cIdWheel      =   cIdFirstUser + 7,
	  cIdReset      =   cIdFirstUser + 8;
	  
static ColorPicker *picker;

//---- ColorCell ---------------------------------------------------------------

class ColorCell: public VObject {
    RGBColor rc;
public:
    ColorCell(Point e, RGBColor c) : rc(&c), VObject(e)
	{ }
    void Draw(Rectangle)
	{ GrPaintRect(contentRect, &rc); }
    void SetColor(RGBColor c)
	{ if (rc.SetRGB(c.GetRed(), c.GetGreen(), c.GetBlue(), rc.GetPrec()))
		ForceRedraw(); }
    RGBColor GetColor()
	{ return rc; }
};

//---- ColorWheel -------------------------------------------------------------

static short StdSectors[] = { 36, 36, 18, 18, 1, 0 };

class ColorWheel: public VObject {
protected:
    short *sectors;
    int Circles, maxix;
    HSVColor hsv;
    RGBColor *palette;
    Point border;
    ImageCache cache;
public:
    ColorWheel(int id, Point Size, short *sectors= StdSectors);
    ~ColorWheel();
    void Draw(Rectangle cr);
    Command *DoLeftButtonDownCommand(Point, Token, int);
    void SetColor(HSVColor c, bool redraw);
    void SetColor2(HSVColor c, bool redraw);
    Rectangle DotRect();
    HSVColor MapPointToColor(Point p);
};

//---- PickerCommand --------------------------------------------------------------

class PickerCommand: public Command {
    ColorWheel *wheel;
public:
    PickerCommand(class ColorWheel *w) : Command("pick")
	{ wheel= w; }
    Command *TrackMouse(TrackPhase, Point ap, Point, Point np);
    void TrackFeedback(Point ap, Point, bool)
	{ }
};

Command *PickerCommand::TrackMouse(TrackPhase atp, Point, Point, Point np)
{
    wheel->SetColor2(wheel->MapPointToColor(np), TRUE);
    if (atp == eTrackRelease)
	return gNoChanges;
    return this;
}

//---- ColorWheel -------------------------------------------------------------

ColorWheel::ColorWheel(int id, Point Size, short *sects) : VObject(Size, id)
{
    sectors= sects;
    for (int sz= Circles= 0; sectors[Circles]; Circles++)
	sz+= sectors[Circles];
	
    border= 6;
    
    palette= new RGBColor[sz];
    
    maxix= 0;
    SetColor(HSVColor(0, MaxWord, MaxWord), FALSE);
}

ColorWheel::~ColorWheel()
{
    SafeDelete(palette);
}

HSVColor ColorWheel::MapPointToColor(Point p)
{
    HSVColor hc= hsv;
    double rad= contentRect.Inset(border).extent.x / 2.0;

    hc.hue= contentRect.PointToAngle(p);
    hc.saturation= (int) (Length(p - contentRect.Center()) / rad
						    * (double)MaxWord + 0.5);
    return hc;
}

Rectangle ColorWheel::DotRect()
{
    Rectangle mark(13), inner= contentRect.Inset(border);
    double w= (double) inner.extent.x / (double) MaxWord * hsv.saturation / 2.0,
	   h= (double) inner.extent.y / (double) MaxWord * hsv.saturation / 2.0;
    mark.origin= contentRect.Center() 
		    + PolarToPoint((double)(hsv.hue), w, h) - mark.extent/2;
    return mark;
}

void ColorWheel::SetColor(HSVColor c, bool redraw)
{
    if (hsv.hue != c.hue || hsv.saturation != c.saturation) {
	if (redraw)
	    InvalidateRect(DotRect());
	hsv.hue= c.hue;
	hsv.saturation= min(c.saturation, MaxWord);
	if (redraw)
	    InvalidateRect(DotRect());
    }
    if (c.value != hsv.value) {
	int i, cir;        
	bool invalidate= FALSE;

	hsv.value= c.value;

	for (i= cir= 0; cir < Circles; cir++) {
	    int sat= (MaxWord * (Circles-1-cir)) / Circles;
	    for (int h= 0; h < sectors[cir]; h++) {
		int hue= (360 * h + (360/sectors[cir]/2)) / sectors[cir];
		if (palette[i].SetHSV(hue, sat, hsv.value, MaxWord))
		    invalidate= TRUE;
		i++;
	    }
	}

	if (invalidate) {
	    ForceRedraw();
	    cache.Invalidate();
	}
    }
}

void ColorWheel::SetColor2(HSVColor c, bool redraw)
{
    if (hsv.hue != c.hue || hsv.saturation != c.saturation) {
	SetColor(c, redraw);
	Control(GetId(), cPartSetState, (void*) &hsv);
    }
}

void ColorWheel::Draw(Rectangle cr)
{
    if (cache.Open(cr)) {
	Rectangle r(contentRect.Inset(border));
	Point Step= r.extent/(2*Circles-1);
	int sat, i;

	for (i= sat= 0; sat < Circles; sat++) {
	    for (int hue= 0; hue < sectors[sat]; hue++)
		GrPaintWedge(r, 360 * hue / sectors[sat], 360/sectors[sat],
								 &palette[i++]);
	    r= r.Inset(Step);
	}
	GrSetPenNormal();
	GrStrokeOval(contentRect.Inset(border));
	cache.Close();
    }
    
    GrSetPenSize(3);
    GrSetPenInk(gInkWhite);
    GrStrokeOval(DotRect());
    GrSetPenNormal();
    GrSetPenInk(gInkBlack);
    GrStrokeOval(DotRect().Inset(1));
}

Command *ColorWheel::DoLeftButtonDownCommand(Point, Token, int)
{
    return new PickerCommand(this);
}

//---- HSVPicker ---------------------------------------------------------------

class HSVPicker: public Expander {
    class Slider2 *valslider;
    class ColorWheel *wheel;
    HSVColor color;
public:
    HSVPicker(int id);
    void SetColor(HSVColor, bool redraw);
    void Control(int id, int part, void *val);
};

HSVPicker::HSVPicker(int id) : Expander(id, eHor, Point(20))
{
    Add(wheel= new ColorWheel(cIdWheel, 245));
    Add(valslider= new Slider2(cIdValSlider, MaxWord));
}

void HSVPicker::SetColor(HSVColor hsv, bool redraw)
{
    color= hsv;
    wheel->SetColor(color, redraw);
    valslider->SetVal(color.value, redraw);
}

void HSVPicker::Control(int id, int part, void *val)
{
    switch (id) {
    case cIdValSlider:
	if (part == eSliderThumb) {
	    color.value= ((Point*)val)->y;
	    wheel->SetColor(color, TRUE);
	    Control(GetId(), 1, (void*) &color);
	}
	break;
    case cIdWheel:
	if (part == cPartSetState) {
	    HSVColor hsv= *((HSVColor*)val);
	    color.hue= hsv.hue;
	    color.saturation= hsv.saturation;
	    Control(GetId(), 1, (void*) &color);
	}
	break;
    default:
	Expander::Control(id, part, val);
	break;
    }
}

//---- RGBPicker ---------------------------------------------------------------

class RGBPicker: public Expander {
    class Slider2 *redslider, *greenslider, *blueslider;
public:
    RGBPicker(int id);
    void SetColor(RGBColor, bool redraw);
    void Control(int id, int part, void *val);
};

RGBPicker::RGBPicker(int id) : Expander(id, eHor, Point(20))
{
    Add(redslider= new Slider2(cIdRGBSlider, MaxWord));
    Add(greenslider= new Slider2(cIdRGBSlider, MaxWord));
    Add(blueslider= new Slider2(cIdRGBSlider, MaxWord));
}

void RGBPicker::SetColor(RGBColor color, bool redraw)
{
    redslider->SetVal(color.GetRed(), redraw);
    greenslider->SetVal(color.GetGreen(), redraw);
    blueslider->SetVal(color.GetBlue(), redraw);
}

void RGBPicker::Control(int id, int part, void *val)
{
    if (id == cIdRGBSlider && part == eSliderThumb) {
	RGBColor color(redslider->GetVal().y, greenslider->GetVal().y,
						blueslider->GetVal().y);
	Control(GetId(), 1, (void*) &color);
    } else
	Expander::Control(id, part, val);
}

//---- ColorPicker -------------------------------------------------------------

MetaImpl0(ColorPicker);
	    
ColorPicker::ColorPicker(char *name) : Dialog(name, eBWinBlock)
{
}

void ColorPicker::DoSetup()
{
    hsvpicker->SetColor(color, FALSE);
    rgbpicker->SetColor(color, FALSE);
    newcell->SetColor(color);
    oldcell->SetColor(oldcolor);
}

VObject *ColorPicker::DoCreateDialog()
{
    RGBColor rc(color);
    rc.SetPrec(MaxWord);
    
    VObject *v= new BorderItem("Old/New",
		    new Expander(cIdNone, eVert, gPoint0,
			oldcell= new ColorCell(Point(75, 45), rc),
			newcell= new ColorCell(Point(75, 45), rc),
			0
		    )
		);    
    return
	new Expander(cIdNone, eHor, gPoint10,
	    new Expander(cIdNone, eVert, gPoint10,
		v,
		new Filler(gPoint10),
		new ActionButton(cIdReset,  "Reset"),
		new ActionButton(cIdCancel, "Cancel"),
		new ActionButton(cIdYes,    "Ok", TRUE),
		0
	    ),
	    new BorderItem("Hue-Saturation-Value", hsvpicker= new HSVPicker(cIdHSVPicker)),
	    new BorderItem("R-G-B", rgbpicker= new RGBPicker(cIdRGBPicker)),
	    0
	);
}

void ColorPicker::Control(int id, int part, void *val)
{
    RGBColor rgb;
    
    switch (id) {
    case cIdHSVPicker:
	color= *((HSVColor*)val);
	newcell->SetColor(color);
	rgbpicker->SetColor(color, TRUE);
	break;
    case cIdRGBPicker:
	rgb= *((RGBColor*)val);
	color= rgb;
	newcell->SetColor(color);
	hsvpicker->SetColor(color, TRUE);
	break;
    case cIdReset:
	color= oldcolor;
	hsvpicker->SetColor(color, TRUE);
	rgbpicker->SetColor(color, TRUE);
	newcell->SetColor(color);
	break;
    default:
	Dialog::Control(id, part, val);
	break;
    }
}

HSVColor ColorPicker::PickColor(HSVColor c, VObject *vop)
{
    color= c;
    oldcolor= c;
    Dialog::ShowOnWindow(vop);
    return color;
}

HSVColor PickColor(HSVColor c, VObject *vop)
{
    if (picker == 0)
	picker= new ColorPicker;
    return picker->PickColor(c, vop);
}

