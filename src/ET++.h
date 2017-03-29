#ifndef ET_First
#ifdef __GNUG__
#pragma once
#endif
#define ET_First

#include "Port.h"
#include "Application.h"
#include "Document.h"
#include "Window.h"
#include "View.h"
#include "Scroller.h"
#include "Splitter.h"
#include "Clipper.h"
#include "Expander.h"
#include "Menu.h"
#include "Command.h"
#include "CmdNo.h"
#include "String.h"
#include "Error.h"

//---- Alert Utilities
#include "Alert_e.h"

extern void ETInit();
extern void InitPrinters();
extern void InitProgEnv();
extern void InitWindowSystem();
extern void InitSystem();

#endif ET_First

