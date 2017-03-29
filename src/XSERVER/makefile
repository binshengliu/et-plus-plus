.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC
XHOME   = /local/X11
#XHOME   = /local/openwin

.C.o:
	$(ETCC) $(CCFLAGS) -I$(XHOME)/include -c $<

OFILES  =   XBitmap.o \
	    XFont.o \
	    XWinSystem.o \
	    XWindowPort.o \
	    XClipBoard.o

all:    xserver.o

xserver.o:   $(OFILES)
	$(LD) -X -r -o xserver.o $(OFILES) -L$(XHOME)/lib -lX11
	
#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c errs

clobber:    clean
	rm -f xserver.o makefile.bak .MAP/*.map 

touch:
	$(MAKE) -t


# DO NOT DELETE THIS LINE -- make depend depends on it.

XBitmap.o: X11.h ../Port.h ../System.h ../Object.h ../Root.h ../Types.h
XBitmap.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../Point.h
XBitmap.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
XBitmap.o: ../Token.h XBitmap.h ../DevBitmap.h XWinSystem.h ../WindowSystem.h
XClipBoard.o: X11.h XClipBoard.h ../ClipBoard.h ../Object.h ../Root.h
XClipBoard.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
XClipBoard.o: XWinSystem.h ../WindowSystem.h ../Port.h ../System.h ../Point.h
XClipBoard.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
XClipBoard.o: ../Token.h ../IO/membuf.h ../IO/streambuf.h ../String.h
XClipBoard.o: ../Storage.h ../CType.h ../Error.h ../View.h ../VObject.h
XClipBoard.o: ../EvtHandler.h ../Command.h ../Collection.h ../Iterator.h
XClipBoard.o: ../Cluster.h ../Filler.h ../ImageItem.h ../TextItem.h
XClipBoard.o: ../FileType.h
XFont.o: X11.h XFont.h ../Font.h ../Root.h ../Rectangle.h ../Metric.h
XFont.o: ../Point.h ../Types.h ../IO/stream.h ../Types.h ../Class.h
XFont.o: ../Object.h ../CmdNo.h XWinSystem.h ../WindowSystem.h ../Port.h
XFont.o: ../System.h ../Ink.h ../Bitmap.h ../Token.h ../String.h ../Storage.h
XFont.o: ../CType.h ../Error.h XBitmap.h ../DevBitmap.h
XWinSystem.o: X11.h XWinSystem.h ../WindowSystem.h ../Port.h ../System.h
XWinSystem.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
XWinSystem.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
XWinSystem.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h XWindowPort.h
XWinSystem.o: ../WindowPort.h XFont.h XBitmap.h ../DevBitmap.h XClipBoard.h
XWinSystem.o: ../ClipBoard.h ../String.h ../Storage.h ../CType.h ../Error.h
XWindowPort.o: X11.h XWindowPort.h ../WindowPort.h ../Port.h ../System.h
XWindowPort.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
XWindowPort.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
XWindowPort.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../WindowSystem.h
XWindowPort.o: XWinSystem.h XFont.h XBitmap.h ../DevBitmap.h ../String.h
XWindowPort.o: ../Storage.h ../CType.h ../EvtHandler.h
