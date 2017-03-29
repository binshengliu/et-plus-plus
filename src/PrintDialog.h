#ifndef PrintDialog_First
#ifdef __GNUG__
#pragma once
#endif
#define PrintDialog_First

#include "Dialog.h"
   
//---- PrintDialog -------------------------------------------------------------

class PrintDialog : public Dialog {
    class FileDialog *saveDialog;
    class CompositeVObject *scroller;
    class CollectionView *collview;
    class OrdCollection *printers;
    class Printer *current, *lastcurrent;
    class BlankWin *window;
    class VObject *vobject, *oldvobject, *optionDialog;
    class NumItem *from, *to;
    char *title;
    bool showgrid, lastshowgrid;
    int lastfrom, lastto, lastprinter;
    
public:
    MetaDef(PrintDialog);
    
    PrintDialog(char *title= "Print");
    ~PrintDialog();    
    VObject *DoCreateDialog();
    void Control(int id, int, void *v);
    void Print(char*);
    int ShowPrintDialog(VObject *v);  
    void UpdateButtons();
    void ShowPageGrid(Rectangle r, VObject *);
    Point GetPageSize();
    
    void DoSetDefaults();
    void DoSave();
    void DoRestore();
    void DoSetup();
};

int ShowPrintDialog(VObject *v);

extern PrintDialog *gPrintManager;
extern bool gPrinting;

#endif PrintDialog_First

