.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	etCC $(CCFLAGS) -c $<

#--------------------------------------------------------------------------------

all:    miniedit

OFILES  =   MiniEditDoc.o miniedit.o

miniedit:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	rm -f .MAP/*.map
	etdeldep
	rm -f miniedit makefile.bak .MAP/*.map


