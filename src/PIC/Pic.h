#ifndef PicPrinter_First
#define PicPrinter_First

#include "Printer.h"

//---- PicPrinter --------------------------------------------------------------

class PicPrinter : public Printer {
public:
    PicPrinter();
    class PrintPort *MakePrintPort(char *name);
};

#endif PicPrinter_First
