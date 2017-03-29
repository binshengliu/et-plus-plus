.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   filebrowser.o BrowserView.o BrowserDoc.o BrowserItems.o

all:    filebrowser 

filebrowser:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

print:
	lwp -l70 -Bsn "-HxFile Browserx%fx%Page: %px" *.c

depend: 
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f filebrowser makefile.bak .MAP/*.map



