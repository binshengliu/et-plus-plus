//$Ink,InkManager,RGBColor$

#include "Ink.h"
#include "String.h"
#include "Port.h"
#include "WindowSystem.h"

const int MaxWord1= MaxWord+1;

InkManager *gInkManager;

Ink *gInkXor, *gInkNone, *gInkWhite, *gInkBlack, *gHighlightColor;   
// obsolete
Ink *ePatXor, *ePatNone, *ePatWhite, *ePatBlack;

const int cMaxPatterns= 22;

static u_short PatBits[cMaxPatterns][8]= {
    { 0x8000,0x0000,0x0800,0x0000,0x8000,0x0000,0x0800,0x0000 },
    { 0x8800,0x0000,0x2200,0x0000,0x8800,0x0000,0x2200,0x0000 },
    { 0xaa00,0x0000,0xaa00,0x0000,0xaa00,0x0000,0xaa00,0x0000 },
    { 0xaa00,0x5500,0xaa00,0x5500,0xaa00,0x5500,0xaa00,0x5500 },
    { 0x5500,0xff00,0x5500,0xff00,0x5500,0xff00,0x5500,0xff00 },
    { 0x7700,0xff00,0xdd00,0xff00,0x7700,0xff00,0xdd00,0xff00 },
    { 0x7f00,0xff00,0xf700,0xff00,0x7f00,0xff00,0xf700,0xff00 },
    { 0x9900,0xCC00,0x6600,0x3300,0x9900,0xCC00,0x6600,0x3300 },
    { 0xff00,0x0000,0xff00,0x0000,0xff00,0x0000,0xff00,0x0000 },
    { 0x5500,0x5500,0x5500,0x5500,0x5500,0x5500,0x5500,0x5500 },
    { 0x4400,0x8800,0x1100,0x2200,0x4400,0x8800,0x1100,0x2200 },
    { 0xbb00,0x7700,0xee00,0xdd00,0xbb00,0x7700,0xee00,0xdd00 },
    { 0xff00,0x0000,0x0000,0x0000,0xff00,0x0000,0x0000,0x0000 },
    { 0x0000,0xff00,0xff00,0xff00,0x0000,0xff00,0xff00,0xff00 },
    { 0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000 },
    { 0xfe00,0xfd00,0xfb00,0xf700,0xef00,0xdf00,0xbf00,0x7f00 },
    { 0x5500,0x8800,0x5500,0x2200,0x5500,0x8800,0x5500,0x2200 },
    { 0xaa00,0x7700,0xaa00,0xdd00,0xaa00,0x7700,0xaa00,0xdd00 },
    { 0x8800,0x8800,0x8800,0x8800,0x8800,0x8800,0x8800,0x8800 },
    { 0xee00,0xee00,0xee00,0xee00,0xee00,0xee00,0xee00,0xee00 },
    { 0xaa00,0x5500,0xaa00,0x5500,0xaa00,0x5500,0xaa00,0x5500 },
    { 0x8000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000 }
};

static Bitmap *patterns[cMaxPatterns+1];

Ink *ePatGrey12, *ePatGrey25, *ePatGrey40, *ePatGrey50;
Ink *ePatGrey60, *ePatGrey75, *ePatGrey87;
Ink *ePat00, *ePat01, *ePat02, *ePat03, *ePat04, *ePat05;
Ink *ePat06, *ePat07, *ePat08, *ePat09, *ePat10, *ePat11;
Ink *ePat12, *ePat13, *ePat14, *ePat15;

//---- Ink ------------------------------------------------------------------------------

MetaImpl(Ink, (T(id), 0));

Ink::Ink()
{
    id= -1;
}
	
Ink::Ink(long i)
{
    id= i;
}

void Ink::SetId(long i)
{
    id= i;
}

void Ink::SetInk(Port *p)
{
    p->DevSetOther((int)id);
}

bool Ink::IsEqual(Object *op)
{
    return Object::IsEqual(op);
}

ostream& Ink::PrintOn(ostream &s)
{
    Object::PrintOn(s);
    return s << id SP;
}

istream& Ink::ReadFrom(istream &s)
{
    Object::ReadFrom(s);
    return s >> id;
}

//---- InkManager -----------------------------------------------------------------------

InkManager::InkManager()
{
    ePatXor= gInkXor= new Ink(-1);
    ePatNone= gInkNone= new Ink((int)0);
    ePatWhite= gInkWhite= new Ink(1);
    ePatBlack= gInkBlack= new Ink(2);

    for (int i= 0; i < cMaxPatterns; i++)
	patterns[i]= new Bitmap(gPoint8, PatBits[i], 1);
	
    ePatGrey12= patterns[0];
    ePatGrey25= patterns[1];
    ePatGrey40= patterns[2];
    ePatGrey50= patterns[3];
    ePatGrey60= patterns[4];
    ePatGrey75= patterns[5];
    ePatGrey87= patterns[6];    
    ePat00= patterns[7];
    ePat01= patterns[8];
    ePat02= patterns[9];
    ePat03= patterns[10];
    ePat04= patterns[11];
    ePat05= patterns[12];
    ePat06= patterns[13];
    ePat07= patterns[14];
    ePat08= patterns[15];
    ePat09= patterns[16];
    ePat10= patterns[17];
    ePat11= patterns[18];
    ePat12= patterns[19];
    ePat13= patterns[20];
    ePat14= patterns[21];
    ePat15= patterns[21];
}

InkManager::~InkManager()
{
}

bool InkManager::Init()
{
    if (gColor)
	gHighlightColor= new RGBColor(0, 255, 0); // light green
    else
	gHighlightColor= gInkXor;
    return FALSE;
}

//---- RGBColor ----------------------------------------------------------------

MetaImpl(RGBColor, (T(red), T(green), T(blue), T(prec), 0));

RGBColor::RGBColor() : Ink(-1)
{
    red= green= blue= prec= 0;
    this->port= 0;
}

RGBColor::RGBColor(RGBColor *cp) : Ink(-1)
{
    if (cp) {
	red= cp->red;
	green= cp->green;
	blue= cp->blue;
	prec= cp->prec;
    }
    this->port= 0;
}

RGBColor::RGBColor(short r, short g, short b, short p) : Ink(-1)
{
    red= r;
    green= g;
    blue= b;
    prec= p;
    this->port= 0;
}

RGBColor::RGBColor(short graylevel, short p) : Ink(-1)
{
    prec= p;
    red= green= blue= graylevel;
    this->port= 0;
}
    
RGBColor::RGBColor(float graylevel, short p) : Ink(-1)
{
    prec= p;
    red= green= blue= (short) (graylevel * MaxWord);
    this->port= 0;
}

RGBColor::RGBColor(HSVColor &hsv, short pre) : Ink(-1)
{
    register int i, f, h;
    int p, q, t;
    
    prec= pre;
    this->port= 0;
    
    if (hsv.hue > 359)
	hsv.hue-= 360;
    else if (hsv.hue < 0)
	hsv.hue+= 360;
    
    if (hsv.saturation == 0)
	red= green= blue= hsv.value;    // achromatic color: there is no hue
    else {
	h= hsv.hue * MaxWord1 / 60;
	i= h / MaxWord1 * MaxWord1;
	f= h - i;
	p= hsv.value * (MaxWord1 - hsv.saturation) / MaxWord1;
	q= hsv.value * (MaxWord1 - (hsv.saturation*f)/MaxWord1) / MaxWord1;
	t= hsv.value * (MaxWord1 - (hsv.saturation * (MaxWord1 - f))/MaxWord1) / MaxWord1;
	switch (i / MaxWord1) {
	case 0:
	    red= hsv.value;     green= t;           blue= p;
	    break;
	case 1:
	    red= q;             green= hsv.value;   blue= p;
	    break;
	case 2:
	    red= p;             green= hsv.value;   blue= t;
	    break;
	case 3:
	    red= p;             green= q;           blue= hsv.value;
	    break;
	case 4:
	    red= t;             green= p;           blue= hsv.value;
	    break;
	case 5:
	    red= hsv.value;     green= p;           blue= q;
	    break;
	}
    }
}

RGBColor::RGBColor(const RGBColor &rc) : Ink(-1)
{
    red= rc.red;
    green= rc.green;
    blue= rc.blue;
    prec= rc.prec;
    this->port= 0;
}

bool RGBColor::SetHSV(short hue, short sat, short value, short p)
{
    HSVColor hc(hue, sat, value);
    RGBColor rc(hc);
    return SetRGB(rc.red, rc.green, rc.blue, p);
}

bool RGBColor::SetRGB(short r, short g, short b, short p)
{
    red= range(0, 255, r);
    green= range(0, 255, g);
    blue= range(0, 255, b);
    prec= range(0, 255, p);
    if (prec == MaxWord && this->port && this->port->HasColor())
	return this->port->DevSetColor(this);
    SetId(-1);
    return TRUE;
}

void RGBColor::SetInk(Port *p)
{
    this->port= p;
    p->SetColor(this);
}

ostream &RGBColor::PrintOn(ostream &s)
{
    Object::PrintOn(s);
    return s << red SP << green SP << blue SP << prec SP; 
}
    
istream &RGBColor::ReadFrom(istream &s)
{
    Object::ReadFrom(s);
    SetId(-1);
    this->port= 0;
    return s >> red >> green >> blue >> prec; 
}
    
int RGBColor::AsGreyLevel()
{
    int l= (int) (0.299 * red + 0.587 * green + 0.114 * blue + 0.5);
    if (l > 255)
	return 255;
    return l;
}

//---- HSVColor ----------------------------------------------------------------

HSVColor::HSVColor(RGBColor &rgb)
{
    int cmax= max(max(rgb.red, rgb.green), rgb.blue);
    int cmin= min(min(rgb.red, rgb.green), rgb.blue);
    
    value= cmax;
    if (cmax)
	saturation= (cmax - cmin) * MaxWord1 / cmax;
    else
	saturation= 0;
	
    if (saturation == 0)
	hue= 0;
    else {      // determine hue
	int red_distance=   (cmax - rgb.red) * MaxWord1 / (cmax - cmin);
	int green_distance= (cmax - rgb.green) * MaxWord1 / (cmax - cmin);
	int blue_distance=  (cmax - rgb.blue) * MaxWord1 / (cmax - cmin);
	
	if (rgb.red == cmax)    // resulting color between yellow and magenta
	    hue= blue_distance - green_distance;
	else if (rgb.green == cmax)  // resulting color between cyan and yellow
	    hue= 2*MaxWord1 + red_distance - blue_distance;
	else                    // resulting color between magenta and cyan
	    hue= 4*MaxWord1 + green_distance - red_distance;
	    
	hue= (hue * 60) / MaxWord1;      // convert to degrees
	if (hue < 0)
	    hue+= 360;              // make nonnegative
	else if (hue > 359)
	    hue-= 360;              // make nonnegative
    }
}

ostream &HSVColor::PrintOn(ostream &s)
{
    return s << hue SP << saturation SP << value SP;
}

istream &HSVColor::ReadFrom(istream &s)
{
    return s >> hue >> saturation >> value;
}
