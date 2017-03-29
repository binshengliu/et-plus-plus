#ifndef FBrowserCmdNo_First
#define FBrowserCmdNo_First

//---- id's -----------------------------------------------------------

const int cIdLoadFile   =    cIdFirstUser,        // load file
	  cIdShowShell  =    cIdFirstUser +  1,   // show shell
	  cIdMake       =    cIdFirstUser +  2,   // show shell & make
	  cIdShellCmd   =    cIdFirstUser +  3,   // execute shell cmd
	  cIdShowErr    =    cIdFirstUser +  4,   // show error in source
	  cIdShiftLeft  =    cIdFirstUser +  5,
	  cIdShiftRight =    cIdFirstUser +  6,
	  cIdChDir      =    cIdFirstUser +  7,   
	  cIdFirstList  =    cIdFirstUser +  10,
	  cIdHistList   =    cIdFirstUser +  11,  
	  cIdFuncButton =    cIdFirstUser +  100;

//---- parts -----------------------------------------------------------
	  
const int cPartSetupFunc  =    cPartFirstUser +  101,  
	  cPartGotoFunc   =    cPartFirstUser +  1000;

//---- commands ---------------------------------------------------------------

const int //---- structure browser
	  cMAKE         =   cUSERCMD + 1,
	  cSHOWERR      =   cUSERCMD + 2,
	  cETBROWSER    =   cUSERCMD + 3,
	  cPREFERENCES  =   cUSERCMD + 4,
	  cTYPESCRIPTMENU=  cUSERCMD + 100;

#endif FBrowserCmdNo_First
