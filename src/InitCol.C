#include "System.h"
#include "ProgEnv.h"
#include "Error.h"

char *gProgname= "*** progname not set ***";
bool gWinInit;

extern System *NewSunSystem();

void InitProgenv()
{
}

void ETInit()
{
#   ifdef OS_SUNOS
    gSystem= NewSunSystem();
    if (gSystem->Init()) {
	Warning("ETInit", "can't init operating system");
	_exit(1);
    }
#   endif OS_SUNOS
    
    if (gSystem == 0) {
	Warning("ETInit", "no operating system found");
	_exit(0);    // give up
    }
}

