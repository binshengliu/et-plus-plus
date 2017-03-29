#ifndef Alert_e_First
#define Alert_e_First

enum AlertType {
    eAlertNote,         // OK-Button
    eAlertCaution,      // Yes, Cancel and No-Button
    eAlertStop,         // Yes, Cancel and No-Button
    eAlertMessage,      // OK-Button, no image
    eAlertSun,          // OK-Button, Sun-Logo
    eAlertError         // image only
};

extern int ShowAlert(AlertType at, char* fmt, ...);
extern bool TestInterrupt(char *what);

#endif Alert_e_First
