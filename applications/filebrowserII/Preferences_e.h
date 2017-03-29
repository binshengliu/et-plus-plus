#ifndef Prefs_e_First
#define Prefs_e_First

//---- entry points

static class Preferences { 
public:
    static int ShowDialog(VObject *v);
    static void Load();
    static void Save();
    
    //---- options
    static int TabWidth();
    static bool UseFastIcons();
    static char *MakeCmd();
    static bool AutoIndent();
    static bool UseStyledText();
    static bool MakeBak();
    static int FontSize();
    static int FileLists();
};

#endif Prefs_e_First
