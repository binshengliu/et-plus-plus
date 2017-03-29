.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   SunFileType.o \
	    SunSystem.o sunsystem.o \
	    SunPtty.o sunptty.o \
	    dynlink.o

all:    sunos.o

sunos.o:  $(OFILES)
	$(LD) -X -r -o sunos.o $(OFILES) $(WLIB)

#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c errs 

clobber:    clean
	rm -f sunos.o makefile.bak .MAP/*.map

touch:
	$(MAKE) -t


# DO NOT DELETE THIS LINE -- make depend depends on it.

SunFileType.o: SunFileType.h ../FileType.h ../Root.h ../Types.h
SunFileType.o: ../IO/stream.h ../Types.h ../System.h ../Object.h ../CmdNo.h
SunFileType.o: ../Class.h ../String.h ../Storage.h ../CType.h sunsystem.h
SunPtty.o: SunPtty.h ../PttyConnection.h ../Types.h ../IO/stream.h ../Types.h
SunPtty.o: ../Root.h sunptty.h
SunSystem.o: SunSystem.h ../System.h ../Object.h ../Root.h ../Types.h
SunSystem.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../Directory.h
SunSystem.o: ../Error.h ../String.h ../Storage.h ../CType.h ../ObjList.h
SunSystem.o: ../SeqColl.h ../Collection.h ../Iterator.h SunFileType.h
SunSystem.o: ../FileType.h SunPtty.h ../PttyConnection.h dynlink.h
SunSystem.o: sunsystem.h
