.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   hello.o

all:    hello 

hello:  $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]
	
print:
	lwp -l70 -Bsn "-HxER Editorx%fx%Page: %px" *.[ch]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f hello makefile.bak .MAP/*.map

run:
	hello


