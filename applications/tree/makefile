.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   tree.o

all:    treeimages tree

tree:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

treeimages:
	cd images; $(MAKE) all

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

hier:
	etmakehier > import

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f tree makefile.bak .MAP/*.map


