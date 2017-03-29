#ifndef CmdNo_First
#ifdef __GNUG__
#pragma once
#endif
#define CmdNo_First

//---- standard command codes --------------------------------------------------

const int cUNDO         =   1,
	  cSAVE         =   2,
	  cLOAD         =   3,
	  cPRINT        =   4,
	  cCLOSE        =   5,
	  cQUIT         =   6,
	  cINSPECT      =   7,
	  cMOVE         =   8,
	  cSTRETCH      =   9,
	  cTOP          =  10,
	  cBOTTOM       =  11,
	  cREDISPLAY    =  12,
	  cCOLLAPSE     =  13,
	  cCUT          =  14,
	  cCOPY         =  15,
	  cPASTE        =  16,
	  cLASTEDIT     =  cPASTE,
	  cTYPEING      =  17,
	  cSELECTALL    =  18,
	  cACCEPT       =  19,
	  cNEW          =  20,
	  cNEWLAST      =  29,
	  cDEBUGFIRST   =  30,
	  cDEBUGLAST    =  39,
	  cABOUT        =  40,
	  cOPEN         =  41,
	  cSAVEAS       =  42,
	  cREVERT       =  43,
	  cFIND         =  44,
	  cSHOWAPPLWIN  =  45,
	  cIMPORT       =  46,
	  cSETPAGE      =  55,
	  cCHANGEALL    =  56,
	  cCHOOSEPRINTER=  57,
	  cREDO         =  58,
//        standard menus
	  cFILEMENU     =  88,
//        ....
//        source code browser
	  cEDITOTHER        = 100,
	  cEDITSUPER        = 101,
	  cEDITSELECTED     = 102,
	  cSHOWHIERARCHY    = 103,
	  cSHOWINHPATH      = 104,
	  cGOBACK           = 105,
	  cGOTOLINE         = 106,
	  cSPAWN            = 107,
	  cREFORMAT         = 108,
	  cUTILMENU         = 109,
	  cINSPECTSOME      = 110,
	  cSHOWIMPLEMENTORS = 112,
	  //  ShellTextView
	  cDOIT             = 200,
	  cBECOMECONSOLE    = 201,
	  cCLEAR            = 202,
	  cRECONNECT        = 203,
	  cAUTOREVEAL       = 204,
	  cUSERCMD          = 1000;

//---- standard dialog item id's -----------------------------------------------
// for Control and DownControl

const int cIdNone           = -1,
	  cIdOk             = 1,
	  cIdYes            = 2,
	  cIdNo             = 3,
	  cIdCancel         = 4,
	  cIdUp             = 5,
	  cIdDown           = 6,
	  cIdLeft           = 7,
	  cIdRight          = 8,
	  cIdCloseBox       = 9,
	  cIdWindowTitle    = 10,
	  cIdDefault        = 11,
	  cIdIgnore         = 12,
	  cIdAbort          = 13,
	  cIdInspect        = 14,
	  cIdTrace          = 15,
	  cIdWinDestroyed   = 16,
	  cIdStartKbdFocus  = 17,
	  cIdEndKbdFocus    = 18,
	  cIdIconLabel      = 19,
	  
	  //---- ET++PE ----------------------------------------------------
	  
	  cIdFirstETPE      = 100,
	  cIdNewInsp        = cIdFirstETPE + 0,
	  cIdInspector      = cIdFirstETPE + 1, 
	  cIdSpawnInsp      = cIdFirstETPE + 2,
	  cIdNewBrowser     = cIdFirstETPE + 3,
	  cIdBrowser        = cIdFirstETPE + 4,
	  cIdNewOBrowser    = cIdFirstETPE + 5,
	  cIdOBrowser       = cIdFirstETPE + 6,  
	  cIdSpawnOBrowser  = cIdFirstETPE + 7,
	  cIdNewHier        = cIdFirstETPE + 8,  
	  cIdHierarchy      = cIdFirstETPE + 9,
	  cIdSpawnHier      = cIdFirstETPE + 10,
	  cIdSpawnFIH       = cIdFirstETPE + 11,
	  cIdShowFIH        = cIdFirstETPE + 12,
    
	   //---- ClassListView
	  cIdCLChangedClass = cIdFirstETPE + 15,
	  cIdCLSelectClass  = cIdFirstETPE + 16,
	  
	  //---- InspItem
	  cIdLoadRef        = cIdFirstETPE + 20,
	  cIdLoadRefNew     = cIdFirstETPE + 21,
	  
	  //---- methodbrowser
	  cIdChangedMethod  = cIdFirstETPE + 30,  // changed selected method
	  cIdImplementors   = cIdFirstETPE + 31,  // show implementors
	  cIdInherited      = cIdFirstETPE + 32,  // show inherited
	  cIdOverrides      = cIdFirstETPE + 33,  // show overrides
    
	  //---- inspector
	  cObjects          = cIdFirstETPE + 41,    
	  cClasses          = cIdFirstETPE + 42,
	  cReferences       = cIdFirstETPE + 43,
	  cIdShiftLeft      = cIdFirstETPE + 44,
	  cIdShiftRight     = cIdFirstETPE + 45,
	  cIdShiftAppl      = cIdFirstETPE + 46,
	  cIdShiftStart     = cIdFirstETPE + 47,
	  cIdShiftEnd       = cIdFirstETPE + 48,
	  
	  //---- class tree
	  cIdClassItem      = cIdFirstETPE+50;


const int cIdClFilter  =    cIdFirstETPE + 112;

	  //...
	  cIdFirstUser      = 1000;

//---- standard part codes -----------------------------------------------------
// for Control and DownControl

const int cPartWantKbdFocus    = 1,
	  cPartChangedText      = 2,
	  cPartLayoutChanged    = 3,
	  cPartIncr             = 4,
	  cPartDecr             = 5,
	  cPartSetState         = 6,
	  cPartStateIsOn        = 7,
	  cPartAction           = 8,
	  cPartValidate         = 9,
	  cPartCollSelect       = 10,
	  cPartCollDoubleSelect = 11,
	  cPartScrollStep       = 12,
	  cPartScrollPage       = 13,
	  cPartScrollAbs        = 14,
	  cPartScrollHAbs       = 15,
	  cPartScrollVAbs       = 16,
	  cPartScrollRel        = 17,
	  cPartScrollPos        = 18,
	  cPartViewSize         = 19,
	  cPartToggle           = 20,
	  cPartOriginChanged    = 21,
	  cPartExtentChanged    = 22,
	  cPartEnableLayoutCntl = 23,
	  cPartTreeSelect       = 24,
	  cPartTreeDoubleSelect = 25,
	  cPartSenderDied       = 26,
	  cPartAnyChange        = 27,
	  cPartReplacedText     = 28,
	  cPartFocusChanged     = 29,
	  cPartHasSelection     = 30,
	  cPartFocusRemove      = 31,
	  // reserved
	  cPartLast             = 100,
	  // ...
	  cPartFirstUser        = 1000;

#endif CmdNo_First
