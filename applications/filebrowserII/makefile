.SUFFIXES: .C

CC      =   etCC $(CCFLAGS) -g
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

OFILES  =   \
	    filebrowser.o \
	    BrowserView.o \
	    BrowserDoc.o \
	    BrowserItems.o \
	    ExtScroller.o \
	    Preferences.o \
	    SharedDocObjects.o \
	    ChangeDirDiag.o

all:    filebrowser filebrowserimages

filebrowserimages:
	cd images; $(MAKE) all

filebrowser:   $(ET_DIR)/src/et.o $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $(OFILES)

depend: filebrowserimages
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f filebrowser makefile.bak .MAP/*.map


