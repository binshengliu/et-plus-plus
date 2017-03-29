//$PttyConnection$
#include "PttyConnection.h"

//---- PttyConnection ----------------------------------------------------------

PttyConnection::PttyConnection()
{
}

PttyConnection::~PttyConnection()
{
}

FILE *PttyConnection::GetFile()
{
    return 0;
}

int PttyConnection::GetPid()
{
    return -1;
}

int PttyConnection::GetFileNo()
{
    return -1;
}

bool PttyConnection::SubmitToSlave(char*, int)
{
    return FALSE;
}

int PttyConnection::Read(char*, int)
{
    return -1;
}

int PttyConnection::GetMode()
{
    return 0;
}

void PttyConnection::KillChild()
{
}

void PttyConnection::CleanupPtty()
{
}

bool PttyConnection::Echo()
{
    return FALSE;
}

bool PttyConnection::RawOrCBreak()
{
    return FALSE;
}

void PttyConnection::BecomeConsole()
{
}

void PttyConnection::SetSize(int,int)
{
}

void PttyConnection::GetPttyChars(PttyChars*)
{
}

