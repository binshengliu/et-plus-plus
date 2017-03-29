.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   Pic.o

all:    pic.o

pic.o:   $(OFILES)
	$(LD) -X -r -o pic.o $(OFILES)
	
#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c 

clobber:    clean
	rm -f pic.o makefile.bak .MAP/*.map 

touch:
	$(MAKE) -t

Pic.o:  Pic.h


# DO NOT DELETE THIS LINE -- make depend depends on it.

Pic.o: ../Port.h ../System.h ../Object.h ../Root.h ../Types.h ../IO/stream.h
Pic.o: ../Types.h ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
Pic.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h Pic.h ../Printer.h
Pic.o: ../Dialog.h ../View.h ../VObject.h ../EvtHandler.h ../Command.h
Pic.o: ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h ../ImageItem.h
Pic.o: ../TextItem.h ../Error.h ../String.h ../Storage.h ../CType.h
Pic.o: ../PrintPort.h
