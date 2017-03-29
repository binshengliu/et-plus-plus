#ifndef Error_First
#ifdef __GNUG__
#pragma once
#endif
#define Error_First

#include "Types.h"

const int cWarning  =   0,
	  cError    =   1000,
	  cSysError =   2000,
	  cFatal    =   3000;
	  
typedef void (*ErrorHandlerFunc)(int level, bool abort, char *location, char *msg);
 
extern void ErrorHandler(int level, char *location, char *fmt, va_list va);
extern void AbstractMethod(char*);

// All these functions are written in C++, but I want them
// to have C linkage so I can call them from my C code.
extern "C" {
    void Error(char *location, char *msgfmt, ...);
    void SysError(char *location, char *msgfmt, ...);
    void Warning(char *location, char *msgfmt, ...);
    void Fatal(char *location, char *msgfmt, ...);
}

extern void DefaultErrorHandler(int level, bool abort, char *location, char *msg);

ErrorHandlerFunc SetErrorHandler(ErrorHandlerFunc newhandler);
ErrorHandlerFunc GetErrorHandler();
int SetAbortLevel(int newlevel);
int GetAbortLevel();
int SetIgnoreLevel(int newlevel);
int GetIgnoreLevel();

#endif Error_First
