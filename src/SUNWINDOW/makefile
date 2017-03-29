.SUFFIXES: .C

CCFLAGS =   -I..
LD      =   ld
ETCC    =   etCC
WLIB    =   -lsuntool -lsunwindow -lpixrect

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

# .c files must be compiled with sun-cc (not gcc!), tom@izf.tno.nl
.c.o:
	cc -O -c $<

# window system independent files

OFILES  =   SunBitmap.o \
	    SunFont.o \
	    SunWindowPort.o \
	    SunClipboard.o \
	    SunWindowSystem.o \
	    sunwindow.o \
	    sunsockets.o

all:    sun.o clipboard

sun.o:  $(OFILES)
	$(LD) -X -r -o sun.o $(OFILES) $(WLIB)

#-------------------------------------------------------------------------------
# sunwindow clipboard server
#-------------------------------------------------------------------------------

CLIPBOARDO  =   Clipboard.o sunsockets.o ../ofiles1.o

clipboard:   $(CLIPBOARDO)
	etCC -none -o clipboard $(CLIPBOARDO) ${ET_DIR}/src/IO/libCC.a -lm

#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c errs

clobber:    clean
	rm -f sun.o clipboard makefile.bak .MAP/*.map 

touch:
	$(MAKE) -t

# DO NOT DELETE THIS LINE -- make depend depends on it.

Clipboard.o: ../String.h ../Types.h ../IO/stream.h ../Types.h ../Storage.h
Clipboard.o: ../CType.h ../Error.h Clipboard.h ../SUNOS/SunSystem.h
Clipboard.o: ../System.h ../Object.h ../Root.h ../CmdNo.h ../Class.h
Clipboard.o: ../Directory.h sunsockets.h ../StreamConnection.h
Clipboard.o: ../SUNWINDOW/Clipboard.h
SunBitmap.o: SunBitmap.h ../DevBitmap.h ../Root.h ../Types.h ../IO/stream.h
SunBitmap.o: ../Types.h ../Point.h ../Class.h ../Object.h ../CmdNo.h
SunBitmap.o: ../Error.h ../Ink.h ../String.h ../Storage.h ../CType.h
SunBitmap.o: SunWindowSystem.h ../WindowSystem.h ../Port.h ../System.h
SunBitmap.o: ../Metric.h ../Rectangle.h ../Font.h ../Bitmap.h ../Token.h
SunBitmap.o: SunWindowPort.h ../WindowPort.h pixrect.h
SunClipboard.o: SunClipboard.h ../ClipBoard.h ../Object.h ../Root.h
SunClipboard.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
SunClipboard.o: ../Error.h Clipboard.h ../IO/membuf.h ../IO/streambuf.h
SunClipboard.o: ../FileType.h ../StreamConnection.h ../System.h
SunClipboard.o: ../SUNWINDOW/Clipboard.h sunsockets.h ../String.h
SunClipboard.o: ../Storage.h ../CType.h
SunFont.o: SunFont.h ../Font.h ../Root.h ../Rectangle.h ../Metric.h
SunFont.o: ../Point.h ../Types.h ../IO/stream.h ../Types.h ../Class.h
SunFont.o: ../Object.h ../CmdNo.h SunBitmap.h ../DevBitmap.h ../String.h
SunFont.o: ../Storage.h ../CType.h ../Error.h pixrect.h pixfont.hh
SunWindowPort.o: SunWindowPort.h ../WindowPort.h ../Port.h ../System.h
SunWindowPort.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
SunWindowPort.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
SunWindowPort.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../WindowSystem.h
SunWindowPort.o: SunWindowSystem.h ../String.h ../Storage.h ../CType.h
SunWindowPort.o: SunBitmap.h ../DevBitmap.h SunFont.h ../Error.h pixrect.h
SunWindowPort.o: pixfont.hh pixwin.hh
SunWindowSystem.o: SunWindowSystem.h ../WindowSystem.h ../Port.h ../System.h
SunWindowSystem.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
SunWindowSystem.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h
SunWindowSystem.o: ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h ../Token.h
SunWindowSystem.o: ../Error.h ../String.h ../Storage.h ../CType.h
SunWindowSystem.o: SunWindowPort.h ../WindowPort.h SunFont.h SunBitmap.h
SunWindowSystem.o: ../DevBitmap.h SunClipboard.h ../ClipBoard.h pixrect.h
SunWindowSystem.o: pixfont.hh pixwin.hh
