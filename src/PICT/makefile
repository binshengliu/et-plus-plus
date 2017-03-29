.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   Pict.o

all:    pict.o

pict.o:   $(OFILES)
	$(LD) -X -r -o pict.o $(OFILES)
	
#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c 

clobber:    clean
	rm -f pict.o makefile.bak .MAP/*.map

touch:
	$(MAKE) -t

Pic.o:  Pict.h


# DO NOT DELETE THIS LINE -- make depend depends on it.

Pict.o: ../Port.h ../System.h ../Object.h ../Root.h ../Types.h ../IO/stream.h
Pict.o: ../Types.h ../CmdNo.h ../Class.h ../Point.h ../Metric.h
Pict.o: ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h ../Token.h Pict.h
Pict.o: ../Printer.h ../Dialog.h ../View.h ../VObject.h ../EvtHandler.h
Pict.o: ../Command.h ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h
Pict.o: ../ImageItem.h ../TextItem.h ../PictPort.h ../PrintPort.h
Pict.o: ../Picture.h ../FileType.h ../ObjList.h ../SeqColl.h
