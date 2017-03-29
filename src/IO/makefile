.SUFFIXES: .C

CCFLAGS = 
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   filebuf.o streambuf.o membuf.o \
	    stream.o istream.o ostream.o

all:    etio.o

etio.o: $(OFILES)
	$(LD) -X -r -o $@ $(OFILES)

#--------------------------------------------------------------------------------

depend:
	etmakedep $(CCFLAGS) *.[cC]

clean:
	rm -f core $(OFILES)

clobber:    clean
	etdeldep
	rm -f etio.o makefile.bak

touch:
	$(MAKE) -t 


