.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	etCC $(CCFLAGS) -c $<

#--------------------------------------------------------------------------------

all:    typescript

OFILES  = typescript.o TypeScriptDoc.o

typescript:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f typescript makefile.bak .MAP/*.map


