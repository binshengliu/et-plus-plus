#ifndef PrintPort_First
#ifdef __GNUG__
#pragma once
#endif
#define PrintPort_First

#include "Port.h"

class PrintPort: public Port {    
protected:
    Rectangle bbox, pagebbox;
    
    void Merge(Rectangle *r);

public:
    MetaDef(PrintPort);
    PrintPort(char *name= 0);
    void OpenPage(int pn);
    void ClosePage();
    virtual void DevOpenPage(int pn);
    virtual void DevClosePage();
};

#endif PrintPort_First
