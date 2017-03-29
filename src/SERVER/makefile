CCFLAGS =   -I.. -I../SUNWINDOW -I../SUNOS
LIB     =   -lsuntool -lsunwindow -lpixrect -lm ../IO/libCC.a

.SUFFIXES: .C

.C.o:
	etCC $(CCFLAGS) -c $<

OFILES  =   ServerSystem.o \
	    ServerBitmap.o \
	    ServerFont.o \
	    ServerConnection.o \
	    ServerPort.o

all:    sunserver.o

sunserver.o:   $(OFILES)
	$(LD) -X -r -o sunserver.o $(OFILES)
	
#-------------------------------------------------------------------------------
# sunwindow server
#-------------------------------------------------------------------------------

SERVERO =   Server.o \
	    ../SUNWINDOW/sun.o \
	    ../ofiles2.o \
	    ../ofiles1.o

server: $(SERVERO)
	etCC -o server $(SERVERO) $(LIB)

#--------------------------------------------------------------------------------

depend:
	etmakedep $(CCFLAGS) *.[cC]

clean:
	rm -f core $(OFILES) *..c errs 

clobber:    clean
	rm -f sunserver.o server makefile.bak .MAP/*.map

touch:
	$(MAKE) -t


# DO NOT DELETE THIS LINE -- make depend depends on it.

Server.o: ../String.h ../Types.h ../IO/stream.h ../Types.h ../Storage.h
Server.o: ../CType.h ../Error.h ../IO/stdio.h ../SUNOS/SunSystem.h
Server.o: ../System.h ../Object.h ../Root.h ../CmdNo.h ../Class.h
Server.o: ../Directory.h ../SUNWINDOW/sunsockets.h
Server.o: ../SUNWINDOW/SunWindowPort.h ../WindowPort.h ../Port.h ../Point.h
Server.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
Server.o: ../Token.h ../WindowSystem.h ../SUNWINDOW/SunWindowSystem.h
Server.o: ../SUNWINDOW/SunBitmap.h ../DevBitmap.h ../SUNWINDOW/SunFont.h
Server.o: Server.h
ServerBitmap.o: ../DevBitmap.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
ServerBitmap.o: ../Point.h ../Class.h ../Object.h ../CmdNo.h ServerBitmap.h
ServerBitmap.o: ../Bitmap.h ../Port.h ../System.h ../Metric.h ../Rectangle.h
ServerBitmap.o: ../Font.h ../Ink.h ../Token.h ServerConnection.h
ServerBitmap.o: ../WindowPort.h ../WindowSystem.h Server.h
ServerConnection.o: ServerConnection.h ../WindowPort.h ../Port.h ../System.h
ServerConnection.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h
ServerConnection.o: ../Types.h ../CmdNo.h ../Class.h ../Point.h ../Metric.h
ServerConnection.o: ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h ../Token.h
ServerConnection.o: ../WindowSystem.h Server.h ServerPort.h ../Error.h
ServerConnection.o: ../String.h ../Storage.h ../CType.h ../SUNOS/sunsystem.h
ServerConnection.o: ../SUNWINDOW/sunsockets.h ../IO/stdio.h
ServerFont.o: ServerFont.h ../Font.h ../Root.h ../Rectangle.h ../Metric.h
ServerFont.o: ../Point.h ../Types.h ../IO/stream.h ../Types.h ../Class.h
ServerFont.o: ../Object.h ../CmdNo.h ../String.h ../Storage.h ../CType.h
ServerFont.o: ServerConnection.h ../WindowPort.h ../Port.h ../System.h
ServerFont.o: ../Ink.h ../Bitmap.h ../Token.h ../WindowSystem.h Server.h
ServerPort.o: ServerPort.h ../WindowPort.h ../Port.h ../System.h ../Object.h
ServerPort.o: ../Root.h ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h
ServerPort.o: ../Class.h ../Point.h ../Metric.h ../Rectangle.h ../Font.h
ServerPort.o: ../Ink.h ../Bitmap.h ../Token.h ../WindowSystem.h Server.h
ServerPort.o: ../Error.h ../String.h ../Storage.h ../CType.h ServerFont.h
ServerPort.o: ServerBitmap.h ServerConnection.h
ServerSystem.o: ServerSystem.h ../WindowSystem.h ../Port.h ../System.h
ServerSystem.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
ServerSystem.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
ServerSystem.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ServerPort.h
ServerSystem.o: ../WindowPort.h Server.h ServerFont.h ServerBitmap.h
ServerSystem.o: ServerConnection.h
