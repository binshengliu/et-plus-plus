#include <stdio.h>
#include <sgtty.h>
#include <errno.h>
#include <osfcn.h>
#include "SunPtty.h"
#include "sunptty.h"

//---- class SunOsPttyConnection ------------------------------------------

SunOsPttyConnection::SunOsPttyConnection(char *name, char **args)
{
    slave= sunPttySpawnSlave(name, args, &pid, &slaveTtyName, &pttychars, &tslot);
}

SunOsPttyConnection::~SunOsPttyConnection()
{
    KillChild();
    SafeDelete(slaveTtyName);
    if (slave)
	fclose(slave);
}
    
int SunOsPttyConnection::GetFileNo()
{
    if (slave)
	return fileno(slave);
    return -1;
}

FILE *SunOsPttyConnection::GetFile()
{
    return slave;
}

int SunOsPttyConnection::GetPid()
{
    return pid;
}
    
bool SunOsPttyConnection::SubmitToSlave(char *buf, int n)
{
    // SpawnSlave sets file to non blocking
    
    if (write(fileno(slave),buf,n) == -1) {
	if (errno == EWOULDBLOCK) {
#if defined(TIOCFLUSH)
	    ioctl(fileno(slave), TIOCFLUSH, 0);
#endif /* TIOCFLUSH */
	    return TRUE;
	}
    }
    return FALSE;
}
    
int SunOsPttyConnection::Read(char *buf, int size)
{
#if defined(TIOCGETP)
    struct sgttyb tty;
#endif /* TIOCGETP */

    int n= read(fileno(slave), buf, size); 
#if defined(TIOCGETP)
    ioctl(fileno(slave), TIOCGETP, (char*) &tty);
    mode= tty.sg_flags;
#endif /* TIOCGETP */
    return n;
}

int SunOsPttyConnection::GetMode()
{
    return mode;
}
    
void SunOsPttyConnection::KillChild()
{
    ::sunPttyKillChild(pid);
    CleanupPtty();
}

void SunOsPttyConnection::CleanupPtty()
{
    ::sunPttyCleanupPtty(slaveTtyName, tslot);
}

bool SunOsPttyConnection::Echo()
{
#if defined(ECHO)
    return (bool) (mode & ECHO);
#else
    return FALSE;
#endif
}

bool SunOsPttyConnection::RawOrCBreak()
{
#if (defined(RAW) && defined(CBREAK))
    return (bool) (mode & (RAW | CBREAK));
#else
    return FALSE;
#endif
}

void SunOsPttyConnection::BecomeConsole()
{
    ::sunPttyBecomeConsole(slaveTtyName);    
}

void SunOsPttyConnection::SetSize(int r,int c)
{
    ::sunPttySetSize(slaveTtyName, r, c);
}

void SunOsPttyConnection::GetPttyChars(PttyChars *pc)
{
    *pc= pttychars;
}
