.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   threeshapes.o

all:    threeshapes    

threeshapes:  $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f threeshapes makefile.bak .MAP/*.map

