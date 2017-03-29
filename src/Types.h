#ifndef Types_First
#ifdef __GNUG__
#pragma once
#endif
#define Types_First

//---- configuration -----------------------------------------------------------

#if defined(__GNUG__) || defined(__STDC__)
#define ANSICPP
#endif

//--- cpp ----------------------------------------------------------------------

#ifdef ANSICPP
    // symbol concatenation operator
#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) name1##name2
#   define _NAME3_(name1,name2,name3) name1##name2##name3

    // stringizing
#   define _QUOTE_(name) #name

#else

#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) _NAME1_(name1)name2
#   define _NAME3_(name1,name2,name3) _NAME2_(name1,name2)name3

#   define _QUOTE_(name) "name"

#endif

//---- constants ---------------------------------------------------------------

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

const int cMaxInt   = 2147483647;
const int cMaxShort = 32767;

//---- types -------------------------------------------------------------------

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned char byte;
typedef u_int bool;

typedef void (*VoidFunc)(...);
typedef int (*IntFunc)(...);
typedef void* (*VoidPtrFunc)(...);

enum HighlightState { Off, On };
enum Direction { eHor, eVert };

//--- stdarg.h -----------------------------------------------------------------

#ifdef __GNUG__
#   include <stdarg.h>
#   define va_(arg) arg    
#   include <builtin.h>
#else

    typedef char *va_list;

#   if defined(sparc)
#       define va_alist __builtin_va_alist
#       define va_(arg) __builtin_va_alist
#   else
#       define va_(arg) arg    
#   endif sparc

#   define va_end(ap)
#   define va_start(ap,arg) ap= ((char*) (&arg+1))
#   define va_arg(ap, mode) ((mode *)(ap += sizeof(mode)))[-1]
#endif

//---- misc --------------------------------------------------------------------

#ifdef __GNUG__
#   define SafeDelete(p) { if (p) { delete p; p= 0; } }
#else
#   define SafeDelete(p) { delete p; p= 0; }
#endif

#define Enum(x) (*((int*)&x))
#define Bool(x) Enum(x)

//--- bit manipulation ---------------------------------------------------------

#define BIT(n) (1 << (n))

inline int SETBIT(int &n, int i)
{
    return n|= BIT(i);
}

inline int CLRBIT(int &n, int i)
{
    return n&= ~BIT(i);
}

inline bool TESTBIT(int n, int i)
{
    return (bool) ((n & BIT(i)) != 0);
}

#ifndef __GNUG__

//---- abs ---------------------------------------------------------------------

inline short abs(short a)
{
    return (a < 0) ? -a : a;
}

inline int abs(int a)
{
    return (a < 0) ? -a : a;
}

inline long abs(long a)
{
    return (a < 0) ? -a : a;
}

inline float abs(float a)
{
    return (a < 0) ? -a : a;
}

inline double abs(double a)
{
    return (a < 0) ? -a : a;
}

//---- sign --------------------------------------------------------------------

inline int sign(int a)
{
    return (a == 0) ? 0 : ( (a > 0) ? 1 : -1 );
}

inline long sign(long a)
{
    return (a == 0) ? 0 : ( (a > 0) ? 1 : -1 );
}

inline double sign(double a)
{
    return (a == 0.0) ? 0 : ( (a > 0.0) ? 1 : -1 );
}

//---- even/odd ----------------------------------------------------------------

inline int even(long a)
{
    return ! (a & 1);
}

inline int odd(long a)
{
    return (a & 1);
}

#endif /* __GNUG__ */

#define EVEN(a) even(a)
#define ODD(a)  odd(a)

inline int min(int a, int b)
{
    return a <= b ? a : b;
}

inline int max(int a, int b)
{
    return a >= b ? a : b;
}

inline void swap(int &a, int &b)
{
    int tmp;
    tmp= a; a= b; b= tmp;
}

inline int range(int lb, int ub, int x)
{
    return x < lb ? lb : (x > ub ? ub : x);
}

inline float range(float lb, float ub, float x)
{
    return x < lb ? lb : (x > ub ? ub : x);
}

//---- -------------------------------------------------------------------------

#define T_PTR       16
#define T_VEC       32
#define T_ARR       64
#define T_PTR2      128

#define SimpleMetaDef(name)                                     \
    extern class Class *_NAME2_(__isa__,name);                  \
    inline class Class *_Type(const name*)                      \
	{ return _NAME2_(__isa__,name); }

SimpleMetaDef(char);
SimpleMetaDef(int);
SimpleMetaDef(short);
SimpleMetaDef(long);
SimpleMetaDef(float);
SimpleMetaDef(double);
SimpleMetaDef(byte);
SimpleMetaDef(u_short);
SimpleMetaDef(u_int);
SimpleMetaDef(u_long);

//---- dummy classes for formatting booleans and flags

class _flags { char dummy; };
class _bool { char dummy; };

SimpleMetaDef(_flags);
SimpleMetaDef(_bool);

#include "IO/stream.h"
#include <stddef.h>

#endif Types_First
