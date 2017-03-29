#ifndef Token_First
#ifdef __GNUG__
#pragma once
#endif
#define Token_First

#include "Rectangle.h"

enum EvtCursorDir {
    eCrsUp,
    eCrsDown,
    eCrsLeft,
    eCrsRight
}; 

enum EventCodes {
    eEvtNone        = -1,
    eEvtAscFirst    = 0,
    eEvtAscLast     = 127,
    eEvtMetaFirst   = 128,   // returned if key depressed with 'Left' or 'Right' key
    eEvtMetaLast    = 255,
    eEvtPseudo      = 0x7f << 8,
	eEvtLocMove     = eEvtPseudo + 0,
	eEvtLocStill    = eEvtPseudo + 1,
	eEvtEnter       = eEvtPseudo + 2,
	eEvtExit        = eEvtPseudo + 3,
	eEvtLocMoveBut  = eEvtPseudo + 4,
	eEvtIdle        = eEvtPseudo + 5,
	eEvtButtons     = eEvtPseudo + 32,
	    eEvtLeftButton  = eEvtButtons + 0,
	    eEvtMiddleButton= eEvtButtons + 1,
	    eEvtRightButton = eEvtButtons + 2,
	eEvtPfkFirst    = eEvtPseudo + 42,
	    eEvtPfkLast     = eEvtPfkFirst + 64,
	eEvtFirstCursorKey= eEvtPseudo + 128,
	    eEvtCursorUp    = eEvtFirstCursorKey + eCrsUp,
	    eEvtCursorDown  = eEvtFirstCursorKey + eCrsDown,
	    eEvtCursorLeft  = eEvtFirstCursorKey + eCrsLeft,
	    eEvtCursorRight = eEvtFirstCursorKey + eCrsRight,
		eEvtLastCursorKey= eEvtCursorRight,
	eEvtDamage  = eEvtPseudo + 250
};

enum EventFlags {
    eFlgNone        = 0,
    
    eFlgButDown     = BIT(0),
    eFlgShiftKey    = BIT(1),
    eFlgCntlKey     = BIT(2),
    eFlgMetaKey     = BIT(3), // left or right keys on SUN-2 and SUN-3 keyboards 

    eFlgDamage1     = BIT(4),
    eFlgDamage2     = BIT(5),
    eFlgDamage3     = BIT(6),
    eFlgDamage4     = BIT(7),
    eFlgDamage5     = BIT(8)
}; 
  
class Token {
public:
    short Code;   // enum EventCodes
    short Flags;  // enum EventFlags
    unsigned long At;
    Point Pos;
    Point ext;
    
public:
    Token();
    Token(EventFlags f, Rectangle &r);    // constructor for damage events
    Token(EventCodes, EventFlags f= eFlgNone, Point pos= gPoint0);
    
    bool DoubleClick(Token &t);
    bool IsAscii()
	{ return (bool) (Code >= eEvtAscFirst && Code <= eEvtAscLast); }
    bool IsMeta()
	{ return (bool) (Code >= eEvtMetaFirst && Code <= eEvtMetaLast); }
    bool IsControl()
	{ return (bool) (Code >= eEvtAscFirst && Code <= eEvtAscLast); }
    bool IsKey()
	{ return (bool) (Code >= eEvtAscFirst && Code <= eEvtMetaLast); }
    bool IsFunctionKey()
	{ return (bool) (Code >= eEvtPfkFirst && Code <= eEvtPfkLast); }
    bool IsCursorKey()
	{ return (bool) (Code >= eEvtFirstCursorKey
						&& Code <= eEvtLastCursorKey); }
    bool IsMouseButton()
	{ return (bool) (Code >= eEvtButtons && Code <= eEvtRightButton); }
    char MapToAscii();
    Point CursorPoint();
    EvtCursorDir CursorDir()
	{ return (EvtCursorDir)((EvtCursorDir)Code - eEvtFirstCursorKey); }
    int FunctionCode()
	{ return Code - eEvtPfkFirst; }
    Rectangle DamageRect()
	{ return Rectangle(Pos, ext); }
    void Dump();
};

#endif Token_First
