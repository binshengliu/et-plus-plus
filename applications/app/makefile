.SUFFIXES: .C

CC      =   etCC $(CCFLAGS)
MAKE    =   make

.C.o:
	etCC $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------
# generic application
#-------------------------------------------------------------------------------

OFILES  =   app.o

all:    app 

app:    $(ET_DIR)/src/et.o $(OFILES)
	$(CC) -Bstatic -o $@ $(OFILES) 

depend:
	etmakedep -I$(ET_DIR)/src $(CCFLAGS) *.[cC]     

clean:
	rm -f core *.o *..c

clobber:    clean
	etdeldep
	rm -f app makefile.bak .MAP/*.map


