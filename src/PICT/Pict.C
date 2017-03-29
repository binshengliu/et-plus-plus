//$PictPrinter,PictPort2$
#include "Port.h"
#include "Pict.h"
#include "PictPort.h"
#include "Picture.h"
#include "FileType.h"
#include "ObjList.h"

class PictPort2: public PictPort {
    Collection *list;
    ostream *os;
public:
    MetaDef(PictPort2);
    PictPort2(char *name);
    ~PictPort2();
    void OpenPage(int);
    void ClosePage();
};

MetaImpl0(PictPort2);

PictPort2::PictPort2(char *name) : PictPort((Picture*)0)
{
    list= new ObjList;
    os= new ostream(name);
}

PictPort2::~PictPort2()
{
    FlushMyText();
    if (list) {
	(*os) << cMagic << cDocTypePict NL << list NL;
	list->FreeAll();
	SafeDelete(list);
	SafeDelete(os);
    }
}

void PictPort2::OpenPage(int)
{
    pict= new Picture;
}

void PictPort2::ClosePage()
{
    pict->Close();
    if (pict->Length() > 0)
	list->Add(pict);
}

//---- PictPrinter -------------------------------------------------------------

PictPrinter::PictPrinter() : Printer("PICT", FALSE)
{
}

class PrintPort *PictPrinter::MakePrintPort(char *name)
{
    return new PictPort2(name);
}

//---- entry point--------------------------------------------------------------

Printer *NewPictPrinter()
{
    return new PictPrinter;
}
