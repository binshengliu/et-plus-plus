//$Preferences$

#include "Form.h"
#include "BorderItems.h"
#include "NumItem.h"
#include "EditTextItem.h"
#include "ManyOfCluster.h"
#include "Buttons.h"
#include "Window.h"
#include "Expander.h"
#include "PopupItem.h"
#include "Menu.h"
#include "System.h"

#include "Preferences.h"
 
static Prefstate current;
static PrefDialog *prefDialog= 0;
static char *version= "0";

MetaImpl0(PrefDialog);

PrefDialog::PrefDialog() : Dialog("Preferences", eBWinBlock)
{
}

VObject *PrefDialog::DoCreateDialog()
{
    VObject *textopt= 
	    new Form(cIdNone, eVObjHCenter, 2,
		"Tabs (in Characters):", ni= new NumItem(cIdNone, 4, 0, 20, 2),
		"Make Command:", make= new EditTextItem(cIdNone, "make", 120), 
	    0);
	    
    VObject *opt=
	new BorderItem ("Options",
	    gopt= new ManyOfCluster(cIdPrefOpt, eVObjHLeft, 0,
					"Make Backup", "Fast Icons", 0),
	    Point(6, 4)
	);
    
    VObject *opt2=
	new BorderItem ("Text Options",
	    txtopt= new ManyOfCluster(cIdTxtOpt, eVObjHLeft, 0,
					"Styled Text", "Auto indent", 0),
	    Point(6, 4)
	);
    
    VObject *actions=
	new Expander(cIdNone, eHor, 20, 
	    new ActionButton(cIdOk, "Ok", TRUE),
	    new ActionButton (cIdCancel, "Cancel"),
	    0
	);

    Menu *m= new Menu("", FALSE, 0, 1, FALSE);
    m->AppendItems(
	    "9",    9,
	    "10",   10,
	    "12",   12,
	    "14",   14,
	    "18",   18,
	    0); 
	    
    VObject *size= 
	new BorderItem("Default Font",
	    fontsize= new PopupItem(cIdNone, 9, "Font Size:", m),
	    Point(6, 4)
	);
    fontsize->SetFlag(eVObjHFixed|eVObjVFixed);
	
    Menu *m2= new Menu("", FALSE, 0, 1, FALSE);
    m2->AppendItems(
	    "1",    1,
	    "2",    2,
	    "3",    3,
	    "4",    4,
	    "5",    5,
	    "6",    6,
	    0); 
	    
    VObject *flists= 
	new BorderItem("Filelists",
	    npanes= new PopupItem(cIdNone, 9, "Number of Filelists:", m2),
	    Point(6, 4)
	);
    npanes->SetFlag(eVObjHFixed|eVObjVFixed);

    return new BorderItem(
	new Expander(1001, eVert, 4,
	    textopt,
	    new Expander(cIdNone, eHor, 10,
		opt,
		opt2,
		0 
	    ),
	    new Expander(cIdNone, eHor, 10,
		size,
		flists,
		0 
	    ),
	    actions,
	    0
	)
    );            
}

void PrefDialog::DoSetup()
{
    DoRestore();
}

void PrefDialog::DoRestore() 
{
    ni->SetValue(current.tabs);
    make->SetString((byte*)current.make);
    gopt->SetItemOn(cIdPrfBak, current.bak);
    gopt->SetItemOn(cIdPrfFast, current.fasticons);
    txtopt->SetItemOn(cIdPrfStyled, current.styled);
    txtopt->SetItemOn(cIdPrfAuto, current.autoind);
    fontsize->SetSelectedItem(current.size);
    npanes->SetSelectedItem(current.npanes);
}

void PrefDialog::DoStore() 
{
    current.tabs= ni->GetValue();
    strreplace(&current.make, make->GetText()->AsString());
    current.bak= gopt->IsItemOn(cIdPrfBak);
    current.fasticons= gopt->IsItemOn(cIdPrfFast);
    current.styled= txtopt->IsItemOn(cIdPrfStyled);
    current.autoind= txtopt->IsItemOn(cIdPrfAuto);
    current.size= fontsize->GetSelectedItem();
    current.npanes= npanes->GetSelectedItem();
    current.modified= TRUE;
}
    
//---- entry points ----------------------------------------------------------

int Preferences::ShowDialog(VObject *vop)
{
    if (prefDialog == 0)
	prefDialog= new PrefDialog;
    return prefDialog->ShowOnWindow(vop->GetWindow());
}

int Preferences::TabWidth()
{
    return current.tabs;
}

bool Preferences::UseFastIcons()
{
    return current.fasticons;
}

char *Preferences::MakeCmd()
{
    return current.make;
}

bool Preferences::AutoIndent()
{
    return current.autoind;
}

bool Preferences::UseStyledText()
{
    return current.styled;
}

bool Preferences::MakeBak()
{
    return current.bak;
}

int Preferences::FontSize()
{
    return current.size;
}

int Preferences::FileLists()
{
    return current.npanes;
}

void Preferences::Load()
{
    current.tabs= 8;
    current.bak= TRUE;
    current.fasticons= TRUE;
    current.styled= TRUE;
    current.autoind= TRUE;
    current.make= 0;
    current.size= gFixedFont->Size();
    current.npanes= 3;
    strreplace(&current.make, "make");
    char *profile= form("%s/.filebrowserrc", gSystem->HomeDirectory());
    if (!gSystem->AccessPathName(profile, 4)) {
	istream from(profile);
	current.ReadFrom(from);
    }
    current.modified= FALSE;
}

void Preferences::Save()
{
    if (current.modified) {
	char *profile= form("%s/.filebrowserrc", gSystem->HomeDirectory());
	ostream to(profile);
	current.PrintOn(to);
    }
}

//---- Prefstate ----------------------------------------------------------

ostream& Prefstate::PrintOn (ostream&s)
{
    PrintString(s, (byte*)version);
    s SP << current.tabs SP << (current.bak == TRUE) SP
      << (current.fasticons == TRUE) SP
      << (current.styled == TRUE) SP
      << (current.autoind == TRUE) NL;
    PrintString(s, (byte*)current.make);
    return s SP << current.size SP << current.npanes NL;
}

istream& Prefstate::ReadFrom(istream &s)
{
    byte *v;
    int b, f, st, a;
    ReadString(s, &v);
    SafeDelete(v);
    s >> current.tabs >> b >> f >> st >> a;
    ReadString(s, (byte**)&current.make);
    s >> current.size >> current.npanes;
    current.bak= b;
    current.fasticons= f;
    current.styled= st;
    current.autoind= a;
    return s;
}
