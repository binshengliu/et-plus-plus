.SUFFIXES: .C

CCFLAGS = -I..
LD      = ld
ETCC    = etCC

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

OFILES  =   AbstractViews.o \
	    SrcBrowser.o \
	    ClassItem.o \
	    ClassTree.o \
	    ClassList.o \
	    CollTblView.o \
	    EtProgEnv.o \
	    EtPeDoc.o   \
	    InspItem.o \
	    ObjectView.o \
	    FlatInhView.o \
	    Inspector.o \
	    OStruBrowser.o \
	    Reference.o \
	    MethodB.o \
	    MethodAccessor.o

all:    etprogenv.o

etprogenv.o:  $(OFILES)
	$(LD) -X -r -o etprogenv.o $(OFILES)

depend:
	etmakedep -I.. *.[cC]

clean:
	rm -f core $(OFILES) *..c errs

clobber:    clean
	rm -f etprogenv.o makefile.bak .MAP/*.map

touch:
	$(MAKE) -t


# DO NOT DELETE THIS LINE -- make depend depends on it.

AbstractViews.o: ../ProgEnv.h ../Types.h ../IO/stream.h ../Types.h
AbstractViews.o: ../Object.h ../Root.h ../CmdNo.h ../Class.h ../Window.h
AbstractViews.o: ../BlankWin.h ../Clipper.h ../VObject.h ../EvtHandler.h
AbstractViews.o: ../Port.h ../System.h ../Point.h ../Metric.h ../Rectangle.h
AbstractViews.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../Command.h
AbstractViews.o: ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h
AbstractViews.o: ../ImageItem.h ../TextItem.h ../BorderItems.h ../Scroller.h
AbstractViews.o: ../String.h ../Storage.h ../CType.h CollTblView.h
AbstractViews.o: ../CollView.h ../View.h ../Dictionary.h ../Set.h
AbstractViews.o: ../ObjArray.h ../OrdColl.h ../SeqColl.h
ClassItem.o: ClassItem.h ../VObject.h ../EvtHandler.h ../Object.h ../Root.h
ClassItem.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
ClassItem.o: ../Port.h ../System.h ../Point.h ../Metric.h ../Rectangle.h
ClassItem.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../Command.h
ClassItem.o: ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h
ClassItem.o: ../ImageItem.h ../TextItem.h ../String.h ../Storage.h ../CType.h
ClassList.o: ClassList.h ../CollView.h ../View.h ../VObject.h ../EvtHandler.h
ClassList.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
ClassList.o: ../CmdNo.h ../Class.h ../Port.h ../System.h ../Point.h
ClassList.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
ClassList.o: ../Token.h ../Command.h ../Collection.h ../Iterator.h
ClassList.o: ../Cluster.h ../Filler.h ../ImageItem.h ../TextItem.h
ClassList.o: ../ObjArray.h ClassItem.h ../Menu.h ../LineItem.h
ClassList.o: ../VObjectPair.h ../BorderItems.h EtPeCmdNo.h ../OrdColl.h
ClassList.o: ../SeqColl.h ../ObjList.h ../ClassManager.h ../ObjectTable.h
ClassList.o: ../String.h ../Storage.h ../CType.h
ClassTree.o: ClassTree.h ../TreeView.h ../Dialog.h ../View.h ../VObject.h
ClassTree.o: ../EvtHandler.h ../Object.h ../Root.h ../Types.h ../IO/stream.h
ClassTree.o: ../Types.h ../CmdNo.h ../Class.h ../Port.h ../System.h
ClassTree.o: ../Point.h ../Metric.h ../Rectangle.h ../Font.h ../Ink.h
ClassTree.o: ../Bitmap.h ../Token.h ../Command.h ../Collection.h
ClassTree.o: ../Iterator.h ../Cluster.h ../Filler.h ../ImageItem.h
ClassTree.o: ../TextItem.h ../Expander.h ../BackgroundItem.h ../PopupItem.h
ClassTree.o: ../Buttons.h ../VObjectPair.h ../OrdColl.h ../SeqColl.h
ClassTree.o: ../ObjArray.h ClassItem.h ../ProgEnv.h ../Menu.h ../CollView.h
ClassTree.o: ../LineItem.h ../BorderItems.h ../Alert_e.h ../Scroller.h
ClassTree.o: ../Panner.h ../Slider.h ../Application.h ../Set.h
ClassTree.o: ../ClassManager.h ../AccessMem.h EtPeCmdNo.h ../String.h
ClassTree.o: ../Storage.h ../CType.h
CollTblView.o: CollTblView.h ../CollView.h ../View.h ../VObject.h
CollTblView.o: ../EvtHandler.h ../Object.h ../Root.h ../Types.h
CollTblView.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../Port.h
CollTblView.o: ../System.h ../Point.h ../Metric.h ../Rectangle.h ../Font.h
CollTblView.o: ../Ink.h ../Bitmap.h ../Token.h ../Command.h ../Collection.h
CollTblView.o: ../Iterator.h ../Cluster.h ../Filler.h ../ImageItem.h
CollTblView.o: ../TextItem.h ClassItem.h ../ObjectTable.h ../OrdColl.h
CollTblView.o: ../SeqColl.h ../ObjArray.h ../Alert_e.h ../String.h
CollTblView.o: ../Storage.h ../CType.h ../Dictionary.h ../Set.h
EtPeDoc.o: EtPeDoc.h ../Document.h ../EvtHandler.h ../Object.h ../Root.h
EtPeDoc.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
EtPeDoc.o: ../Port.h ../System.h ../Point.h ../Metric.h ../Rectangle.h
EtPeDoc.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../FileType.h
EtPeDoc.o: ../ByteArray.h ../ProgEnv.h ../ObjectTable.h ../Window.h
EtPeDoc.o: ../BlankWin.h ../Clipper.h ../VObject.h ../Command.h
EtPeDoc.o: ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h
EtPeDoc.o: ../ImageItem.h ../TextItem.h ../BorderItems.h ../Splitter.h
EtPeDoc.o: ../Icon.h ../Buttons.h ../VObjectPair.h ../Application.h
EtPeDoc.o: ../Dialog.h ../View.h Inspector.h ../CollView.h ../Expander.h
EtPeDoc.o: ClassTree.h ../TreeView.h ../ClassManager.h OStruBrowser.h
EtPeDoc.o: ../GraphView.h SrcBrowser.h ../CodeTextView.h ../FixLineTView.h
EtPeDoc.o: ../TextView.h ../StaticTView.h ../Text.h ../String.h ../Storage.h
EtPeDoc.o: ../CType.h ../Mark.h ../OrdColl.h ../SeqColl.h ../ObjArray.h
EtPeDoc.o: FlatInhView.h EtPeCmdNo.h MethodAccessor.h Reference.h
EtPeDoc.o: ../PathLookup.h ../ObjList.h ../images/et++pe_icon.image
EtPeDoc.o: ../images/et++pe_src.image ../images/et++pe_insp.image
EtPeDoc.o: ../images/et++pe_ostru.image ../images/et++pe_hier.image
EtProgEnv.o: EtProgEnv.h ../ProgEnv.h ../Types.h ../IO/stream.h ../Types.h
EtProgEnv.o: ../Object.h ../Root.h ../CmdNo.h ../Class.h ../OrdColl.h
EtProgEnv.o: ../SeqColl.h ../Collection.h ../Iterator.h ../ObjArray.h
EtProgEnv.o: ../Application.h ../Dialog.h ../View.h ../VObject.h
EtProgEnv.o: ../EvtHandler.h ../Port.h ../System.h ../Point.h ../Metric.h
EtProgEnv.o: ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h ../Token.h
EtProgEnv.o: ../Command.h ../Cluster.h ../Filler.h ../ImageItem.h
EtProgEnv.o: ../TextItem.h EtPeDoc.h ../Document.h ../FileType.h
EtProgEnv.o: ../ByteArray.h
FlatInhView.o: ../OrdColl.h ../SeqColl.h ../Collection.h ../Object.h
FlatInhView.o: ../Root.h ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h
FlatInhView.o: ../Class.h ../Iterator.h ../ObjArray.h ../ObjList.h
FlatInhView.o: ../ClassManager.h ../String.h ../Storage.h ../CType.h
FlatInhView.o: EtPeCmdNo.h EtPeDoc.h ../Document.h ../EvtHandler.h ../Port.h
FlatInhView.o: ../System.h ../Point.h ../Metric.h ../Rectangle.h ../Font.h
FlatInhView.o: ../Ink.h ../Bitmap.h ../Token.h ../FileType.h ../ByteArray.h
FlatInhView.o: FlatInhView.h ../CollView.h ../View.h ../VObject.h
FlatInhView.o: ../Command.h ../Cluster.h ../Filler.h ../ImageItem.h
FlatInhView.o: ../TextItem.h EtProgEnv.h ../ProgEnv.h
InspItem.o: InspItem.h ../VObject.h ../EvtHandler.h ../Object.h ../Root.h
InspItem.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
InspItem.o: ../Port.h ../System.h ../Point.h ../Metric.h ../Rectangle.h
InspItem.o: ../Font.h ../Ink.h ../Bitmap.h ../Token.h ../Command.h
InspItem.o: ../Collection.h ../Iterator.h ../Cluster.h ../Filler.h
InspItem.o: ../ImageItem.h ../TextItem.h ../String.h ../Storage.h ../CType.h
InspItem.o: ../Alert_e.h Reference.h ../ClassManager.h EtPeCmdNo.h
Inspector.o: ../ET++.h ../Port.h ../System.h ../Object.h ../Root.h ../Types.h
Inspector.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../Point.h
Inspector.o: ../Metric.h ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h
Inspector.o: ../Token.h ../Application.h ../Dialog.h ../View.h ../VObject.h
Inspector.o: ../EvtHandler.h ../Command.h ../Collection.h ../Iterator.h
Inspector.o: ../Cluster.h ../Filler.h ../ImageItem.h ../TextItem.h
Inspector.o: ../Document.h ../FileType.h ../Window.h ../BlankWin.h
Inspector.o: ../Clipper.h ../BorderItems.h ../Scroller.h ../Splitter.h
Inspector.o: ../Expander.h ../Menu.h ../CollView.h ../LineItem.h
Inspector.o: ../VObjectPair.h ../String.h ../Storage.h ../CType.h ../Error.h
Inspector.o: ../Alert_e.h Inspector.h ../ObjList.h ../SeqColl.h ../OrdColl.h
Inspector.o: ../ObjArray.h ../Dictionary.h ../Set.h ../ObjectTable.h
Inspector.o: ClassItem.h ../ClassManager.h ../AccessMem.h ../WindowSystem.h
Inspector.o: ../Buttons.h ObjectView.h Reference.h InspItem.h ClassList.h
Inspector.o: EtPeDoc.h ../ByteArray.h EtPeCmdNo.h EtProgEnv.h ../ProgEnv.h
MethodAccessor.o: MethodAccessor.h ../Object.h ../Root.h ../Types.h
MethodAccessor.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
MethodAccessor.o: ../OrdColl.h ../SeqColl.h ../Collection.h ../Iterator.h
MethodAccessor.o: ../ObjArray.h ../PathLookup.h ../String.h ../Storage.h
MethodAccessor.o: ../CType.h ../System.h ../CheapText.h ../Text.h ../Font.h
MethodAccessor.o: ../Rectangle.h ../Metric.h ../Point.h ../Port.h ../Ink.h
MethodAccessor.o: ../Bitmap.h ../Token.h ../CodeTextView.h ../FixLineTView.h
MethodAccessor.o: ../TextView.h ../StaticTView.h ../Mark.h ../View.h
MethodAccessor.o: ../VObject.h ../EvtHandler.h ../Command.h ../Cluster.h
MethodAccessor.o: ../Filler.h ../ImageItem.h ../TextItem.h EtPeDoc.h
MethodAccessor.o: ../Document.h ../FileType.h ../ByteArray.h
MethodB.o: MethodB.h ../Dialog.h ../View.h ../VObject.h ../EvtHandler.h
MethodB.o: ../Object.h ../Root.h ../Types.h ../IO/stream.h ../Types.h
MethodB.o: ../CmdNo.h ../Class.h ../Port.h ../System.h ../Point.h ../Metric.h
MethodB.o: ../Rectangle.h ../Font.h ../Ink.h ../Bitmap.h ../Token.h
MethodB.o: ../Command.h ../Collection.h ../Iterator.h ../Cluster.h
MethodB.o: ../Filler.h ../ImageItem.h ../TextItem.h ../CollView.h
MethodB.o: ../ByteArray.h ../ProgEnv.h ../OrdColl.h ../SeqColl.h
MethodB.o: ../ObjArray.h ../RegularExp.h ../ObjectTable.h ../ClassManager.h
MethodB.o: ../String.h ../Storage.h ../CType.h ../Menu.h ../LineItem.h
MethodB.o: ../VObjectPair.h ../BorderItems.h ../Scroller.h ../Buttons.h
MethodB.o: ../EditTextItem.h ../Clipper.h ../TextView.h ../StaticTView.h
MethodB.o: ../Text.h ../Mark.h ../ManyOfCluster.h ../Alert_e.h ../Expander.h
MethodB.o: ../Window.h ../BlankWin.h EtPeCmdNo.h EtPeDoc.h ../Document.h
MethodB.o: ../FileType.h
OStruBrowser.o: ../ET++.h ../Port.h ../System.h ../Object.h ../Root.h
OStruBrowser.o: ../Types.h ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h
OStruBrowser.o: ../Point.h ../Metric.h ../Rectangle.h ../Font.h ../Ink.h
OStruBrowser.o: ../Bitmap.h ../Token.h ../Application.h ../Dialog.h ../View.h
OStruBrowser.o: ../VObject.h ../EvtHandler.h ../Command.h ../Collection.h
OStruBrowser.o: ../Iterator.h ../Cluster.h ../Filler.h ../ImageItem.h
OStruBrowser.o: ../TextItem.h ../Document.h ../FileType.h ../Window.h
OStruBrowser.o: ../BlankWin.h ../Clipper.h ../BorderItems.h ../Scroller.h
OStruBrowser.o: ../Splitter.h ../Expander.h ../Menu.h ../CollView.h
OStruBrowser.o: ../LineItem.h ../VObjectPair.h ../String.h ../Storage.h
OStruBrowser.o: ../CType.h ../Error.h ../Alert_e.h ../Panner.h ../Slider.h
OStruBrowser.o: ../BackgroundItem.h ../Buttons.h ../WindowSystem.h
OStruBrowser.o: ../ObjectTable.h ../AccessMem.h ../ByteArray.h ../OrdColl.h
OStruBrowser.o: ../SeqColl.h ../ObjArray.h OStruBrowser.h ../GraphView.h
OStruBrowser.o: ../TreeView.h EtPeCmdNo.h ClassItem.h
ObjectView.o: ObjectView.h ../CollView.h ../View.h ../VObject.h
ObjectView.o: ../EvtHandler.h ../Object.h ../Root.h ../Types.h ../IO/stream.h
ObjectView.o: ../Types.h ../CmdNo.h ../Class.h ../Port.h ../System.h
ObjectView.o: ../Point.h ../Metric.h ../Rectangle.h ../Font.h ../Ink.h
ObjectView.o: ../Bitmap.h ../Token.h ../Command.h ../Collection.h
ObjectView.o: ../Iterator.h ../Cluster.h ../Filler.h ../ImageItem.h
ObjectView.o: ../TextItem.h EtPeDoc.h ../Document.h ../FileType.h
ObjectView.o: ../ByteArray.h EtPeCmdNo.h ../Application.h ../Dialog.h
ObjectView.o: ../Menu.h ../LineItem.h ../VObjectPair.h ../BorderItems.h
ObjectView.o: ../ObjArray.h ../ObjList.h ../SeqColl.h ../String.h
ObjectView.o: ../Storage.h ../CType.h ../AccessMem.h ../ProgEnv.h InspItem.h
ObjectView.o: Reference.h
Reference.o: Reference.h ../Object.h ../Root.h ../Types.h ../IO/stream.h
Reference.o: ../Types.h ../CmdNo.h ../Class.h ../ObjectTable.h ../System.h
Reference.o: ../String.h ../Storage.h ../CType.h
SrcBrowser.o: ../CodeTextView.h ../FixLineTView.h ../TextView.h
SrcBrowser.o: ../StaticTView.h ../Text.h ../Object.h ../Root.h ../Types.h
SrcBrowser.o: ../IO/stream.h ../Types.h ../CmdNo.h ../Class.h ../String.h
SrcBrowser.o: ../Storage.h ../CType.h ../Font.h ../Rectangle.h ../Metric.h
SrcBrowser.o: ../Point.h ../Port.h ../System.h ../Ink.h ../Bitmap.h
SrcBrowser.o: ../Token.h ../Mark.h ../OrdColl.h ../SeqColl.h ../Collection.h
SrcBrowser.o: ../Iterator.h ../ObjArray.h ../View.h ../VObject.h
SrcBrowser.o: ../EvtHandler.h ../Command.h ../Cluster.h ../Filler.h
SrcBrowser.o: ../ImageItem.h ../TextItem.h ../StyledText.h ../GapText.h
SrcBrowser.o: ../RunArray.h ../WindowSystem.h ../PathLookup.h ../GotoDialog.h
SrcBrowser.o: ../GotoDialog_e.h ../Dialog.h ../ClassManager.h ../Alert_e.h
SrcBrowser.o: ../Menu.h ../CollView.h ../LineItem.h ../VObjectPair.h
SrcBrowser.o: ../BorderItems.h ../Application.h ../Scroller.h ../Expander.h
SrcBrowser.o: ../Clipper.h ../Window.h ../BlankWin.h SrcBrowser.h
SrcBrowser.o: ../Document.h ../FileType.h EtProgEnv.h ../ProgEnv.h
SrcBrowser.o: ClassList.h MethodB.h ../ByteArray.h EtPeCmdNo.h EtPeDoc.h
