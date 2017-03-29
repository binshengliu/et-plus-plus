#ifndef PECmdNo_First
#define PECmdNo_First

//---- id's -----------------------------------------------------------

const int //---- EtPeDoc
	  cIdNewInsp    =   cIdFirstUser + 0,
	  cIdInspector  =   cIdFirstUser + 1, 
	  cIdSpawnInsp  =   cIdFirstUser + 2,
	  cIdNewBrowser =   cIdFirstUser + 3,
	  cIdBrowser    =   cIdFirstUser + 4,
	  cIdNewOBrowser=   cIdFirstUser + 5,
	  cIdOBrowser   =   cIdFirstUser + 6,  
	  cIdSpawnOBrowser= cIdFirstUser + 7,
	  cIdNewHier    =   cIdFirstUser + 8,  
	  cIdHierarchy  =   cIdFirstUser + 9,
	  cIdSpawnHier  =   cIdFirstUser + 10,
	  cIdSpawnFIH   =   cIdFirstUser + 11,
	  cIdShowFIH    =   cIdFirstUser + 12;
    
const int //---- ClassListView
	  cIdCLChangedClass= cIdFirstUser + 15,
	  cIdCLSelectClass = cIdFirstUser + 16;
	  
const int //---- InspItem
	  cIdLoadRef    =   cIdFirstUser + 20,
	  cIdLoadRefNew =   cIdFirstUser + 21;
	  
const int //---- methodbrowser
	  cIdChangedMethod=  cIdFirstUser + 30,  // changed selected method
	  cIdImplementors =  cIdFirstUser + 31,  // show implementors
	  cIdInherited    =  cIdFirstUser + 32,  // show inherited
	  cIdOverrides    =  cIdFirstUser + 33;  // show overrides
    
const int //---- inspector
	  cObjects      =   cIdFirstUser + 41,    
	  cClasses      =   cIdFirstUser + 42,
	  cReferences   =   cIdFirstUser + 43,
	  cIdShiftLeft  =   cIdFirstUser + 44,
	  cIdShiftRight =   cIdFirstUser + 45,
	  cIdShiftAppl  =   cIdFirstUser + 46,
	  cIdShiftStart =   cIdFirstUser + 47,
	  cIdShiftEnd   =   cIdFirstUser + 48;
	  
const int //---- class tree
	  cIdClassItem = cIdFirstUser+50;


//---- commands ---------------------------------------------------------------

const int //---- structure browser
	  cUPDATE       =   cUSERCMD + cIdFirstUser + 1,
	  cCLEARPATH    =   cUPDATE  + 2,
	  cEVTHANDLER   =   cUPDATE  + 3,
	  cCONTAINER    =   cUPDATE  + 4,
	  cREFERENCES   =   cUPDATE  + 5,
	  cDEPENDENCIES =   cUPDATE  + 6,
	  cDOINSPECT    =   cUPDATE  + 7,
	  cSHOWLINES    =   cUPDATE  + 8,
	  cSHOWLABELS   =   cUPDATE  + 9,
	  cPOINTERS     =   cUPDATE  +10;
	  
	  //---- object view
const int cEDITDECL     =   cPOINTERS   +1,
	  cEDITIMPL     =   cPOINTERS   +2,
	  cABSTRVIEW    =   cPOINTERS   +3,
	  cOBJBROWSER   =   cPOINTERS   +4,
	  cOBJREFERENCES=   cPOINTERS   +5;

const int //---- class listview in inspector
	  cSHOWALL      =   cOBJREFERENCES+1,
	  cUPDATELIST   =   cOBJREFERENCES+2,
	  cEMPTYCLASSES =   cOBJREFERENCES+3;
	  
const int //---- method browser
	  cIMPLEMENTORS =   cEMPTYCLASSES + 2,
	  cOVERRIDES    =   cEMPTYCLASSES + 3,
	  cINHERITED    =   cEMPTYCLASSES + 4,
	  cMETHREFERENCES=  cEMPTYCLASSES + 5,
	  cFILTER       =   cEMPTYCLASSES + 6,
	  cREMFILTER    =   cEMPTYCLASSES + 7;
	  
const int //---- hierarchy browser
	  cONLYABSTRACT=    cREMFILTER   +   1,
	  cALLCLASSES  =    cREMFILTER   +   2,
	  cONLYAPPL    =    cREMFILTER   +   3,
	  cCLEARREFS   =    cREMFILTER   +   4,
	  cDOCOLLAPSE  =    cREMFILTER   +   5,
	  cMEMBERS     =    cREMFILTER   +   7,
	  cCLIENTS     =    cREMFILTER   +   8,
	  cSOURCE      =    cREMFILTER   +   9,
	  cINSPSOME    =    cREMFILTER   +  10,
	  cPROMOTE     =    cREMFILTER   +  11;

const int cIdClFilter  =    cIdFirstUser + 1;

#endif PECmdNo_First
