CC      =   cc
CFLAGS  =   -O

OFILES  =   storage.o

all:    storage.o
	
#--------------------------------------------------------------------------------

depend:
	etmakedep $(CFLAGS) *.[cC]

clean:
	rm -f core $(OFILES) *..c

clobber:    clean
	rm -f storage.o makefile.bak .MAP/*.map

touch:
	$(MAKE) -t

# DO NOT DELETE THIS LINE -- make depend depends on it.
