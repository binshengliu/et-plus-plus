#ifndef Alert_First
#ifdef __GNUG__
#pragma once
#endif
#define Alert_First


#include "Dialog.h"
#include "Alert_e.h"

class Alert: public Dialog {
    VObject *text, *image, *buttons;
public:
    MetaDef(Alert);

    Alert(AlertType, byte *text= 0, Bitmap *bm= 0, ...);
    ~Alert();

    VObject *DoCreateDialog();
    int Show(char *fmt, ...);
    int ShowV(char *fmt, va_list ap);
    class Menu *GetMenu();
    void InspectorId(char *buf, int sz);
};

#endif Alert_First
