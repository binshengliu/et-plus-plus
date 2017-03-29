.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   PostScriptPrinter.o PostScriptPort.o

all:    postscript.o

postscript.o:   $(OFILES)
	$(LD) -X -r -o postscript.o $(OFILES)
	
#--------------------------------------------------------------------------------

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c 

clobber:    clean
	rm -f postscript.o makefile.bak .MAP/*.map

touch:
	$(MAKE) -t

# DO NOT DELETE THIS LINE -- make depend depends on it.

PostScriptPort.o: PostScriptPort.h ../PrintPort.h ../Port.h ../System.h
PostScriptPort.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
PostScriptPort.o: ../CmdNo.h ../Class.h ../Point.h ../Metric.h ../Rectangle.h
PostScriptPort.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../Error.h
PostScriptPort.o: ../String.h ../Storage.h ../CType.h ../DevBitmap.h
PostScriptPrinter.o: ../Port.h ../System.h ../Object.h ../Root.h ../Types.h
PostScriptPrinter.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
PostScriptPrinter.o: ../Point.h ../Metric.h ../Rectangle.h ../Font.h ../Ink.h
PostScriptPrinter.o: ../Bitmap.h ../Token.h PostScriptPrinter.h ../Printer.h
PostScriptPrinter.o: ../Dialog.h ../View.h ../VObject.h ../EvtHandler.h
PostScriptPrinter.o: ../Command.h ../Collection.h ../Iterator.h ../Cluster.h
PostScriptPrinter.o: ../Filler.h ../ImageItem.h ../TextItem.h
PostScriptPrinter.o: PostScriptPort.h ../PrintPort.h ../OneOfCluster.h
PostScriptPrinter.o: ../ManyOfCluster.h ../EditTextItem.h ../Clipper.h
PostScriptPrinter.o: ../TextView.h ../StaticTView.h ../Text.h ../String.h
PostScriptPrinter.o: ../Storage.h ../CType.h ../Mark.h ../OrdColl.h
PostScriptPrinter.o: ../SeqColl.h ../ObjArray.h ../Expander.h
PostScriptPrinter.o: ../BorderItems.h ../Buttons.h ../VObjectPair.h
