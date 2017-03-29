#include <stdio.h>

#include "Error.h"
#include "String.h"
#include "System.h"

int gDebug= 0;

void AbstractMethod(char *s)
{
    Warning(s, "abstract method called");
}

static int abortlevel= cSysError;
static int ignorelevel= cError;

void DefaultErrorHandler(int level, bool abort, char *location, char *msg)
{
    char *type= "Warning";
    if (level < ignorelevel)
	return;
    if (level >= cFatal)
	type= "Fatal";
    else if (level >= cSysError)
	type= "SysError";
    else if (level >= cError)
	type= "Error";
	  
    fprintf (stderr, "%s in <%s>: %s\n", type, location, msg);
    fflush (stderr);
    if (abort) {
	fprintf (stderr, "aborting\n");
	fflush (stderr);
	Abort();
    }
}

static ErrorHandlerFunc errorhandler= DefaultErrorHandler;

void ErrorHandler(int level, char *location, char *fmt, va_list ap)
{
    char buf[1024], *bp;
    
    vsprintf(buf, fmt, ap);
    if (level >= cSysError && level < cFatal)
	bp= form("%s (%s)", buf, gSystem->GetError());
    else
	bp= buf;
    errorhandler(level, level >= abortlevel, location, bp);
}

void Error(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    ErrorHandler(cError, location, va_(fmt), ap);
    va_end(ap);
}

void SysError(char *location,  char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap, va_(fmt));
    ErrorHandler(cSysError, location, va_(fmt), ap);
    va_end(ap);
}

void Warning(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    ErrorHandler(cWarning, location, va_(fmt), ap);
    va_end(ap);
}

void Fatal(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    ErrorHandler(cFatal, location, va_(fmt), ap);
    va_end(ap);
}

ErrorHandlerFunc SetErrorHandler(ErrorHandlerFunc newhandler)
{
    ErrorHandlerFunc oldhandler= errorhandler;
    errorhandler= newhandler;
    return oldhandler;
}

ErrorHandlerFunc GetErrorHandler()
{
    return errorhandler;
}

int SetAbortLevel(int newlevel)
{
    int oldlevel= abortlevel;
    abortlevel= newlevel;
    return oldlevel;
}

int GetAbortLevel()
{
    return abortlevel;
}

int SetIgnoreLevel(int newlevel)
{
    int oldlevel= abortlevel;
    ignorelevel= newlevel;
    return oldlevel;
}

int GetIgnoreLevel()
{
    return ignorelevel; 
}
