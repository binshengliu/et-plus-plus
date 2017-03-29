.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   ErShapes.o ErCommands.o ErDocument.o ErView.o Er.o

all:    er    

er:     $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)
	
print:
	lwp -l70 -Bsn "-HxER Editorx%fx%Page: %px" *.[ch]

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f er makefile.bak .MAP/*.map


