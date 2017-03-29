.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	etCC $(CCFLAGS) -c $<

#--------------------------------------------------------------------------------

all:    polydocapp

OFILES  =   PolyDocApp.o EditDoc.o TwoShapesDoc.o

polydocapp:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f polydocapp makefile.bak .MAP/*.map



