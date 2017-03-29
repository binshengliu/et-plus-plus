.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	$(CC) $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   AssocArray.o assoc.o

all:    assoc

assoc:  $(ET_DIR)/src/col.o $(OFILES)
	$(CC) -col -o $@ $(OFILES)

depend:
	etmakedep $(CCFLAGS) *.C

clean:
	rm -f core *.o

clobber:    clean
	etdeldep
	rm -f assoc makefile.bak

