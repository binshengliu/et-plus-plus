#ifndef Ink_First
#ifdef __GNUG__
#pragma once
#endif
#define Ink_First

#include "Types.h"
#include "Object.h"

//---- Ink ---------------------------------------------------------------------

class Ink : public Object {
    long int id;
public:
    MetaDef(Ink);
    Ink();
    Ink(long i);
    
    long GetId()
	{ return id; }
	
    void SetId(long id);
    virtual void SetInk(class Port *p);
    virtual bool IsEqual(Object*);
    virtual ostream& PrintOn(ostream&);
    virtual istream& ReadFrom(istream&);
};

extern Ink *gInkXor,
	   *gInkNone,
	   *gInkWhite,
	   *gInkBlack,
	   *gHighlightColor;
	   
// obsolete !!!
extern Ink *ePatXor, *ePatNone, *ePatWhite, *ePatBlack;
extern Ink *ePatGrey12, *ePatGrey25, *ePatGrey40, *ePatGrey50, *ePatGrey60;
extern Ink *ePatGrey75;
extern Ink *ePat00, *ePat01, *ePat02, *ePat03, *ePat04, *ePat05;
extern Ink *ePat06, *ePat07, *ePat08, *ePat09, *ePat10, *ePat11;
extern Ink *ePat12, *ePat13, *ePat14, *ePat15;

typedef Ink *GrPattern;

//---- RGBColor ----------------------------------------------------------------

typedef short word;

const int MaxWord= 255;

class RGBColor: public Ink {
friend class HSVColor;
    short prec;
    short red, green, blue;
public:
    class Port *port;
    
public:
    MetaDef(RGBColor);
    RGBColor();
    RGBColor(RGBColor*);
    RGBColor(short r, short g, short b, short prec= 0);
    RGBColor(class HSVColor &h, short prec= 0);
    RGBColor(short graylevel, short prec= 0);
    RGBColor(float graylevel, short prec= 0);
    RGBColor(const RGBColor&);
    
    short GetRed()
	{ return red; }
    short GetGreen()
	{ return green; }
    short GetBlue()
	{ return blue; }
    short GetPrec()
	{ return prec; }
    bool SetHSV(short hue, short sat, short value, short p= 0);
    bool SetRGB(short r, short g, short b, short p= 0);
    bool SetRed(short r)
	{ return SetRGB(r, green, blue, prec); }
    bool SetGreen(short g)
	{ return SetRGB(red, g, blue, prec); }
    bool SetBlue(short b)
	{ return SetRGB(red, green, b, prec); }
    bool SetPrec(short p)
	{ return SetRGB(red, green, blue, p); }
    int AsGreyLevel();
    
    void SetInk(Port *p);
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

//---- HSVColor ----------------------------------------------------------------

class HSVColor {
public:
    short hue, saturation, value;

    HSVColor()
	{ hue= saturation= value= 0; }
    HSVColor(int h, int s, int v) : hue(h), saturation(s), value(v)
	{ }
    HSVColor(RGBColor &c);
    HSVColor(int graylevel)
	{ hue= saturation= 0; value= graylevel; }
    
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

//---- InkManager --------------------------------------------------------------

class InkManager : public Root {
public:
    InkManager();
    virtual ~InkManager();
    
    bool Init();        // returns TRUE on error
};

extern InkManager *gInkManager;

#endif Ink_First
