#ifndef String_First
#ifdef __GNUG__
#pragma once
#endif
#define String_First

#include "Types.h"
#include "Storage.h"
#include "CType.h"

extern "C" {
    // standard string operations
    extern char* strcat(char*, const char*);
    extern char* strncat(char*, const char*, int);
    extern int strcmp(const char*, const char*);
    extern int strncmp(const char*, const char*, int);
    extern char* strcpy(char*, const char*);
    extern char* strncpy(char*, const char*, int);
    extern int strlen(const char*);
    
#ifndef __GNUG__
    // bsd byte string operations
    extern void bcopy(const char*, char*, int);
    extern void bzero(char*, int);
    extern int bcmp(const char*, const char*, int);

    extern char* index(const char*, int);
    extern char* rindex(const char*, int);
#endif

#ifndef HAVE_VPRINTF
    extern char *vsprintf(char*, const char*, va_list);
    extern char* sprintf(char*, const char*, ...);
    extern int sscanf(char*, const char*, ...);
#endif /* HAVE_VPRINTF */   
    
    extern int atoi(const char*);
    extern double atof(const char*);
    extern long atol(const char*);
    
    char* strsave(char *s, int l= -1); 
	// allocate a buffer and copy 's'
    
    int StuffChar(char *src, char *dst, int dstlen, char* specchars, char stuffchar);

}

#define memcpy(s1, s2, n) bcopy(s2, s1, n)
#define BCOPY(a,b,c) bcopy((const char*)a,(char*)b,c)
#define BCMP(a,b,c) bcmp((const char*)a,(char*)b,c)

// string utilites
extern char* strprintf(char *fmt, ...);   
    // allocate a buffer and copy the string specified in fmt a la printf
								    
extern char* strvprintf(char*, va_list);
extern char* strreplace(char**dst, char*src, int l= -1);
    // replace dst with src, expanding dst if necessary  
extern char* strfreplace(char**, char *fmt, va_list);
extern bool  strismember(const char *s, ...);
    // return TRUE when s equals one of the following arguments
extern char *strquotechar(byte ch, char *bufp);
extern char* strn0cpy(char*, const char*, int);
    // strncpy which establises always a terminating 0 byte

extern StrCmp(byte*, byte*, int l= -1, byte *map= 0);
extern byte sortmap[], identitymap[];

// utilites to store strings in ET++ format
extern istream &ReadString(istream &i, byte **s, int *lp= 0);
extern ostream &PrintString(ostream &o, byte *s, int l= -1);
extern void PutHex(ostream &ofp, byte b, int *col= 0);
extern byte GetHex(istream &ofp);
extern char *BaseName(char *pathname);  
extern char *form(const char *fmt, ...);     // printf format

#endif String_First

