#ifndef Math_First
#ifdef __GNUG__
#pragma once
#endif
#define Math_First

#include "Types.h"

const double MathPi =   3.14159265358979323846,
	     MathE  =   2.7182818284590452354;

extern "C" {
    extern double hypot(double, double);
    extern double atan2(double, double);
    extern double sqrt(double);
    extern double sin(double), cos(double);
}

extern int Hypot(int x, int y);     // return sqrt(px*px + py*py)
extern double Atan2(double fx, double fy);
extern double intatan2(int x, int y);
extern int intsqrt(int x);
extern void Sincos(double a, double *sin, double *cos);

#endif Math_First

