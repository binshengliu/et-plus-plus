#ifndef Printer_First
#ifdef __GNUG__
#pragma once
#endif
#define Printer_First

#include "Dialog.h"

//---- Printer -----------------------------------------------------------------

class Printer : public Dialog {
    char *name;
    VObject *options;
    bool canprint;
public:
    MetaDef(Printer);
    Printer(char *n= "GENERIC", bool cp= FALSE);
    ~Printer();
    virtual class PrintPort *MakePrintPort(char *name);
    char *GetName();
    VObject *DoCreateDialog();
    virtual VObject *DoCreatePrintDialog();
    void Control(int id, int, void *v);
    VObject *GetOptions();
    CanPrint()
	{ return canprint; }
    virtual Point GetPageSize();
};

#endif Printer_First


