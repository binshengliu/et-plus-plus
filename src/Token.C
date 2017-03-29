#include "Token.h"

const int cDoubleClick= 500;  // 0.5 sec

Token::Token()
{
    Code= eEvtNone;
    Flags= eFlgNone;
    Pos= ext= gPoint0;
}

Token::Token(EventFlags f, Rectangle &r)    // constructor for damage events
{
    Code= eEvtDamage;
    Flags= f;
    Pos= r.origin;
    ext= r.extent;
}

Token::Token(EventCodes ec, EventFlags f, Point p)
{
    Code= ec;
    Flags= f;
    Pos= p;
}

bool Token::DoubleClick(Token &t)
{
    if (t.IsMouseButton())
	if (t.Code == Code && abs((int)(t.Pos.x-Pos.x)) < 2
		    && abs((int)(t.Pos.y-Pos.y)) < 2
			&& abs((long)(t.At-At)) < cDoubleClick)
	    return TRUE;
    return FALSE;
}

char Token::MapToAscii()
{
    return (Code & 0x7f) + 0x60;
}

Point Token::CursorPoint()
{
    switch (CursorDir()) {
    case eCrsUp:
	return Point(0,-1);
    case eCrsDown:
	return Point(0,1);
    case eCrsLeft:
	return Point(-1,0);
    case eCrsRight:
	return Point(1,0);
    default:
	return gPoint0;
    }
}

void Token::Dump()
{
    extern int gDebug;
    char *c= "???";
    
    if (gDebug == 0)
	return;
    
    int code= Code;
    switch(Code) {
    case eEvtNone:
	c= "None";
	break;
    case eEvtLocMove:
	c= "LocMove";
	break;
    case eEvtLocStill:
	c= "LocStill";
	break;
    case eEvtEnter:
	c= "Enter";
	break;
    case eEvtExit:
	c= "Exit";
	break;
    case eEvtLocMoveBut:
	c= "LocMoveBut";
	break;
    case eEvtIdle:
	c= "Idle";
	break;
    case eEvtLeftButton:
	c= "LeftButton";
	break;
    case eEvtMiddleButton:
	c= "MiddleButton";
	break;
    case eEvtRightButton:
	c= "RightButton";
	break;
    case eEvtDamage:
	c= "Damage";
	break;
    default:
	if (Code >= eEvtPfkFirst && Code <= eEvtPfkLast) {
	    c= "PfkFirst";
	    code-= eEvtPfkFirst;
	} if (Code >= eEvtAscFirst && Code <= eEvtAscLast)
	    c= "AscFirst";
	if (Code >= eEvtMetaFirst && Code <= eEvtMetaLast)
	    c= "MetaFirst";
	break;
    }
    if (Code == eEvtDamage)
	cerr.form("c:%s(%d) f:%04x x:%d y:%d w:%d h:%d\n", c, code, Flags, Pos.x,
							Pos.y, ext.x, ext.y);
    else
	cerr.form("c:%s(%d) f:%04x x:%d y:%d\n", c, code, Flags, Pos.x, Pos.y);
}
