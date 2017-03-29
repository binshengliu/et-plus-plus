.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	etCC $(CCFLAGS) -c $<

#--------------------------------------------------------------------------------

all:    twoviews

OFILES  =   twoviews.o TwoViewDoc.o

twoviews:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f twoviews makefile.bak .MAP/*.map


