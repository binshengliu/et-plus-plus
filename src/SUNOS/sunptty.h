#ifndef sunptty_First
#define sunptty_First

//---- C++ interface to sunptty.c ----------------------------------------

extern "C" {
    FILE *sunPttySpawnSlave(char *name, char **args, int *pid, char **name,
						    PttyChars *pc, int *tslot);
	// open a pseudotty, fork a process name with args and connect 0,1,2 to
	// slave side, pid returns the pid of the process on the slave side
	// pid is the process group of the process on the slave side
	// name is the name of ptty
    
    void sunPttyCleanupPtty(char *slaveName, int tslot);
	// reset state of ptty
    
    void sunPttyKillChild(int pgrp);
    void sunPttyBecomeConsole(char*);
    void sunPttySetSize(char*, int, int);
}

#endif sunptty_First

