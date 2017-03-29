#ifndef Form_First
#ifdef __GNUG__
#pragma once
#endif
#define Form_First

#include "Cluster.h"

//---- Form -----------------------------------------------------------

class Form: public Cluster {
    bool transposed;
    void Transpose();
protected:
    short CalcCols();
    void CacheMinSize();
public:
    MetaDef(Form);

    Form(int id, VObjAlign a= eVObjHCenter, Point g= gPoint0, Collection *cp= 0);
    Form(int id, VObjAlign a, Point g, VObject*, ...);
    Form(int id, VObjAlign a, Point g, char*, ...);

    void SetOrigin(Point at);
};

#endif Form_First

