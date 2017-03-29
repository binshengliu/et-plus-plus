.SUFFIXES: .C .cps

CFLAGS  =   -O
CCFLAGS =   -I.. -I$(NEWSHOME)/include
ETCC    =   etCC

CPS     =   $(NEWSHOME)/bin/cps -i
WLIB    =   -lcps


.C.o:
	$(ETCC) $(CCFLAGS) -c $<

.cps.c:
	$(CPS) $<

.cps.h:
	$(CPS) $<

OFILES  =   NeWSBitmap.o \
	    NeWSFont.o \
	    NeWSWindowSystem.o \
	    NeWSWindowPort.o \
	    news.o

all:        newsserver.o

news.h:     news.cps
news.c:     news.cps

newsserver.o:   $(OFILES)
	$(LD) -X -r -o newsserver.o $(OFILES) $(WLIB)
	
#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c

clobber:    clean
	rm -f newsserver.o news.[ch] news.[ch].BAK makefile.bak .Map/*.map errs

touch:
	$(MAKE) -t

# DO NOT DELETE THIS LINE -- make depend depends on it.

NeWSBitmap.o: ../Port.h ../System.h ../Object.h ../Root.h ../Types.h
NeWSBitmap.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../Point.h
NeWSBitmap.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
NeWSBitmap.o: ../Token.h NeWSBitmap.h NeWSWindowSystem.h ../WindowSystem.h
NeWSBitmap.o: ../SUNWINDOW/SunBitmap.h ../DevBitmap.h psio.h news.h
NeWSFont.o: NeWSFont.h ../Font.h ../Root.h ../Rectangle.h ../Metric.h
NeWSFont.o: ../Point.h ../Types.h ../IO/stream.h ../Types.h ../Class.h
NeWSFont.o: ../Object.h ../CmdNo.h NeWSWindowSystem.h ../WindowSystem.h
NeWSFont.o: ../Port.h ../System.h ../Ink.h ../Bitmap.h ../Token.h ../String.h
NeWSFont.o: ../Storage.h ../CType.h psio.h news.h
NeWSWindowPort.o: NeWSWindowPort.h ../WindowPort.h ../Port.h ../System.h
NeWSWindowPort.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
NeWSWindowPort.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
NeWSWindowPort.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../WindowSystem.h
NeWSWindowPort.o: NeWSWindowSystem.h NeWSFont.h NeWSBitmap.h ../String.h
NeWSWindowPort.o: ../Storage.h ../CType.h psio.h news.h
NeWSWindowSystem.o: NeWSWindowSystem.h ../WindowSystem.h ../Port.h
NeWSWindowSystem.o: ../System.h ../Object.h ../Root.h ../Types.h
NeWSWindowSystem.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
NeWSWindowSystem.o: ../Point.h ../Metric.h ../Rectangle.h ../Font.h ../Ink.h
NeWSWindowSystem.o: ../Bitmap.h ../Token.h NeWSWindowPort.h ../WindowPort.h
NeWSWindowSystem.o: NeWSFont.h NeWSBitmap.h ../String.h ../Storage.h
NeWSWindowSystem.o: ../CType.h psio.h news.h
