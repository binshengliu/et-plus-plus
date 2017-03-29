.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#--------------------------------------------------------------------------------

OFILES  =   AnnotatedItem.o \
	    VobEditDoc.o \
	    vobedit.o \
	    MenuTextView.o \
	    MickyView.o \
	    myVObjectTextView.o

all:    vobeditimages vobedit

vobedit:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

vobeditimages:
	cd images; $(MAKE) all

depend: vobeditimages
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f vobedit makefile.bak .MAP/*.map


