.SUFFIXES: .C

ETCC      =   etCC -none $(CCFLAGS)
MAKE      =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   makemap.o

all:    install

makemap:  $(OFILES)
	$(ETCC) $(LDFLAGS) -o $@ $(OFILES)

depend:
	etmakedep $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	rm -f makemap ../../bin/makemap makefile.bak

install:    makemap
	cp makemap ../../bin


