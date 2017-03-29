.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)

.C.o:
	$(CC) $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   trofftool.o font.o
FONTS   =   R S 

all:    trofftool DESC.out

trofftool:  $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

DESC.out:   DESC $(FONTS) makedev
	makedev DESC
	
DESC:   makedesc makedev $(FONTS)
	makedesc

makedev: makedev.c dev.h
	cc $(CFLAGS) -o makedev makedev.c

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f trofftool makedev makefile.bak .MAP/*.map


