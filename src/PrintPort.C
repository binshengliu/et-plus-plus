//$PrintPort$
#include "PrintPort.h"
#include "String.h"
#include "Error.h"

MetaImpl0(PrintPort);

PrintPort::PrintPort(char*)
{
    bbox= gRect0;
}

void PrintPort::Merge(Rectangle *r)
{
    pagebbox.Merge(Inter(cliprect, *r));
}

void PrintPort::OpenPage(int pn)
{
    pagebbox= gRect0;
    DevOpenPage(pn);
}

void PrintPort::ClosePage()
{
    bbox.Merge(pagebbox);
    DevClosePage();
    pagebbox= gRect0;
}

void PrintPort::DevOpenPage(int)
{
}

void PrintPort::DevClosePage()
{
}

