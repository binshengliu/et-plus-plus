#ifndef ColorPicker_First
#define ColorPicker_First

#include "Dialog.h"

//---- ColorPicker ------------------------------------------------------------------

class ColorPicker: public Dialog {
    class HSVPicker *hsvpicker;
    class RGBPicker *rgbpicker;
    class ColorCell *oldcell, *newcell;
    HSVColor color, oldcolor;
public:
    MetaDef(ColorPicker);
    ColorPicker(char *name= "ColorPicker");
    VObject *DoCreateDialog();
    void Control(int id, int part, void *val);
    HSVColor PickColor(HSVColor c, VObject *vop);
    void DoSetup();
};

extern HSVColor PickColor(HSVColor c, VObject *vop);

#endif ColorPicker_First
