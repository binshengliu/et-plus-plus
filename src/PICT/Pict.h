#ifndef PictPrinter_First
#define PictPrinter_First

#include "Printer.h"

//---- PictPrinter -------------------------------------------------------------

class PictPrinter : public Printer {
public:
    PictPrinter();
    class PrintPort *MakePrintPort(char *name);
};

#endif PictPrinter_First
