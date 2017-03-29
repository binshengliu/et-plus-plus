CFLAGS  = -O 

OFILES  = main.o parse.o pr.o include.o

all:    install

makedepend: $(OFILES)
	cc -o makedepend $(OFILES)

depend:
	makedepend *.c

clean:
	rm -f *.o core

clobber: clean
	rm -f makedepend makefile.bak ../../bin/etmakedep

install:    makedepend
	cp makedepend ../../bin/etmakedep

# DO NOT DELETE THIS LINE -- make depend depends on it.

include.o: def.h
main.o: def.h
parse.o: def.h
pr.o: def.h
