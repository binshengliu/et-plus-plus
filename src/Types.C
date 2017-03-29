//$intClass,charClass,shortClass,longClass,floatClass,doubleClass,bitsClass$
//$boolClass$
#include "Types.h"
#include "String.h"
#include "Class.h"
    
SimpleMetaImpl(char)
{
    if (*((char*) addr))
	sprintf(buf, "\'%c\'", *((char*) addr));
    else
	sprintf(buf, "\'\0\'");
}

SimpleMetaImpl(short)
{
    sprintf(buf, "%d", *((short*) addr));
}

SimpleMetaImpl(int) 
{
    sprintf(buf, "%d", *((int*) addr));
}

SimpleMetaImpl(long)
{
    sprintf(buf, "%d", *((long*) addr));
}

SimpleMetaImpl(float)
{
    sprintf(buf, "%f", (double)(*((float*) addr)));
}
 
SimpleMetaImpl(double)
{
    sprintf(buf, "%f", (double)(*((float*) addr)));
}

SimpleMetaImpl(bool)
{
    sprintf(buf, "%s", *((bool*) addr) ? "true" : "false");
}

SimpleMetaImpl(byte)
{
    if (*((char*) addr))
	sprintf(buf, "\'%c\'", *((char*) addr));
    else
	sprintf(buf, "\'\0\'");
}

SimpleMetaImpl(u_short)
{
    sprintf(buf, "%ud", *((u_short*) addr));
}

SimpleMetaImpl(u_int) 
{
    sprintf(buf, "%ud", *((u_short*) addr));
}

SimpleMetaImpl(u_long)
{
    sprintf(buf, "%ud", *((u_short*) addr));
}

_SimpleMetaImpl(_bool,bool)
{
    sprintf(buf, "%s", *((bool*) addr) ? "true" : "false");
}

_SimpleMetaImpl(_flags,u_int)
{
    sprintf(buf, "0x%x", *((u_int*) addr));
}

