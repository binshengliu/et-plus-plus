.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	$(CC) $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   guard.o

all:    guard

guard:  $(ET_DIR)/src/col.o $(OFILES)
	$(CC) -col -o $@ $(OFILES)

depend:
	etmakedep $(CCFLAGS) *.C

clean:
	rm -f core *.o errs

clobber:    clean
	etdeldep
	rm -f guard makefile.bak


