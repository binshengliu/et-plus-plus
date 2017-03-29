.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------

DRAWOF  =   Shape.o \
	    Group.o \
	    BoxShape.o \
	    RcBoxShape.o \
	    OvalShape.o \
	    LineShape.o \
	    ArcShape.o \
	    PolyShape.o \
	    TextShape.o \
	    RegionShape.o \
	    BezierShape.o \
	    Commands.o \
	    ImageShape.o \
	    PictureShape.o \
	    Connection.o \
	    DrawController.o \
	    DrawDocument.o \
	    MenuItems.o \
	    DrawView.o \
	    draw.o

DRAWDF  =   DynShape.o \
	    DynShape2.o

all:    drawimages draw

draw:   $(ET_DIR)/src/et.o $(DRAWOF) $(DRAWDF)
	$(CC) $(LDFLAGS) -o $@ $(DRAWOF)

drawimages:
	cd images; $(MAKE) all

depend: drawimages
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]

clean:
	cd images; $(MAKE) clean
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f draw makefile.bak .MAP/*.map


