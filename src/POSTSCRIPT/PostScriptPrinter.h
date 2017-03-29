#ifndef PostScriptPrinter_First
#define PostScriptPrinter_First

#include "Printer.h"
#include "PostScriptPort.h"

//---- PostScriptPrinter -------------------------------------------------------

class PostScriptPrinter : public Printer {
    class OneOfCluster *orientationCluster, *resolutionCluster;
    class ManyOfCluster *optionsCluster;
    class EditTextItem *eti;
    PostScriptState curr, old;
public:
    MetaDef(PostScriptPrinter);
    PostScriptPrinter();
    class PrintPort *MakePrintPort(char *name);
    void DoSetDefaults();
    void DoSave();
    void DoRestore();
    VObject *DoCreatePrintDialog();
    void DoSetup();
    void Control(int id, int, void *v);
    Point GetPageSize();
};

#endif PostScriptPrinter_First
