
#include "Math.h"

#ifdef sun
extern "C" void sincos(double, double*, double*);
#endif sun

int Hypot(int x, int y)     // return sqrt(px*px + py*py);
{
    return (int) (hypot(x, y) + 0.5);
}

double intatan2(int y, int x)
{
    double dd= (0.75 - (atan2(-y, x) / MathPi + 1.0)/2.0 ) * 360.0;
    while (dd < 0.0)
	dd+= 360.0;
    return dd;
}

double Atan2(double fy, double fx)
{
    return intatan2((int)(fy+0.5), (int)(fx+0.5));
}

int intsqrt(int x)
{
    return (int) (sqrt(x) + 0.5);
}

void Sincos(double a, double *sinp, double *cosp)
{
#ifdef sun
    sincos(2 * MathPi * (90.0 - a) / 360.0, sinp, cosp);
    *sinp= -*sinp;
#else sun
    a= 2 * MathPi * (90.0 - a) / 360.0;
    *cosp= cos(a);
    *sinp= -sin(a);
#endif sun
}
