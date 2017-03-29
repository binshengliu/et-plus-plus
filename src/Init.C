#include <stdio.h>

#include "System.h"
#include "WindowSystem.h"
#include "ProgEnv.h"
#include "Error.h"

bool gInMain;

void InitPrinters();
void InitProgEnv();
void InitWindowSystem();
void InitSystem();

extern class Printer *gPrinters[];
extern Printer *NewPrinter();
extern Printer *NewPicPrinter();
extern Printer *NewPostScriptPrinter();
extern Printer *NewPictPrinter();

void InitPrinters()
{
    int i= 0;
    
    if (gPrinters[0])
	return;
    InitWindowSystem();
    if (! gInMain) {
	fprintf(stderr, "--- InitPrinters called before main\n"); 
	abort();
    }
    gPrinters[i++]= NewPrinter();

#ifdef PR_POSTSCRIPT
    gPrinters[i++]= NewPostScriptPrinter();
#endif PR_POSTSCRIPT

#ifdef PR_PIC
    gPrinters[i++]= NewPicPrinter();
#endif PR_PIC

#ifdef PR_PICT
    gPrinters[i++]= NewPictPrinter();
#endif PR_PICT

    gPrinters[i]= 0;
}

extern ProgEnv *NewEtProgEnv();

void InitProgenv()
{
    InitWindowSystem();

    /*
    if (gProgEnv == 0)
	gProgEnv= (ProgEnv*) gSystem->Load(gProgname, "EtProgEnv");
    */
    if (! gInMain) {
	fprintf(stderr, "--- InitProgenv called before main\n"); 
	abort();
    }

#ifdef ET_PROGENV
    if (gProgEnv == 0)
	gProgEnv= NewEtProgEnv();
#endif ET_PROGENV

    if (gProgEnv == 0) 
	gProgEnv= new ProgEnv();

    if (gProgEnv == 0) {
	fprintf(stderr, "can't find programming environment\n");
	_exit(0);    // give up
    }
}

extern System *NewSunSystem();

void InitSystem()
{
    if (gSystem)
	return;
	
    if (! gInMain) {
	fprintf(stderr, "--- InitSystem called before main\n"); 
	abort();
    }
    
#ifdef OS_SUNOS
    gSystem= NewSunSystem();
    if (gSystem->Init()) {
	fprintf(stderr, "can't gInMain operating system\n");
	_exit(1);
    }
#endif OS_SUNOS
    
    if (gSystem == 0) {
	fprintf(stderr, "can't find operating system\n");
	_exit(0);    // give up
    }

}

extern WindowSystem *NewXWindowSystem();
extern WindowSystem *NewServerWindowSystem();
extern WindowSystem *NewSunWindowSystem();
extern WindowSystem *NewNeWSWindowSystem();

void InitWindowSystem()
{
    if (gWindowSystem)
	return;
    InitSystem();
	
    if (! gInMain) {
	fprintf(stderr, "--- InitWindowSystem called before main\n");
	abort();
    }

#ifdef WS_SUNSERVER
    if (gWindowSystem == 0)
	gWindowSystem= NewServerWindowSystem(); // try to connect to SunWindow server
#endif WS_SUNSERVER

#ifdef WS_X
    if (gWindowSystem == 0)
	gWindowSystem= NewXWindowSystem();      // try to connect to X server
#endif WS_X

#ifdef WS_NEWS
    if (gWindowSystem == 0)
	gWindowSystem= NewNeWSWindowSystem();   // try to connect to NeWS server
#endif WS_NEWS
    
#ifdef WS_SUNWINDOW
    if (gWindowSystem == 0)
	gWindowSystem= NewSunWindowSystem();    // try SunWindow System
#endif WS_SUNWINDOW
    
    if (gWindowSystem == 0) {
	fprintf(stderr, "can't find window system\n");
	_exit(0);    // give up
    }
    
    gWindowSystem->Init();
}

void ETInit()
{
    gInMain= TRUE;
    InitWindowSystem();
}

