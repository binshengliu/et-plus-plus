# Makefile for ET++

MAKE    = make
ETCC    = etCC

#---- C++2.0
CCFLAGS =
CFLAGS  = 
CC      = cc
LD      = ld
 
#---- g++
#CCFLAGS= -O -fstrength-reduce -W 
#CFLAGS= -O -fstrength-reduce -W  
#CC     = /usr/local/bin/gcc
#LD     = /usr/local/lib/gcc-ld

.SUFFIXES: .C

.C.o:
	$(ETCC) $(CCFLAGS) -c $<

#-------------------------------------------------------------------------------
# memory management
#-------------------------------------------------------------------------------
MALLOC_DIR      =   MALLOC
MALLOC          =   $(MALLOC_DIR)/storage.o

#-------------------------------------------------------------------------------
# supported window systems
#-------------------------------------------------------------------------------
SUNSERVER       =   SERVER/sunserver.o  # a server for sunwindows/sunview
XSERVER         =   XSERVER/xserver.o   # X11.3
NEWSSERVER      =   NEWS/newsserver.o   # NeWS 1.1
SUNWINDOW       =   SUNWINDOW/sun.o     # sunwindow/sunview

#-------------------------------------------------------------------------------
# supported operating systems
#-------------------------------------------------------------------------------
SUNOS           =   SUNOS/sunos.o

#-------------------------------------------------------------------------------
# supported printers
#-------------------------------------------------------------------------------
POSTSCRIPT      =   POSTSCRIPT/postscript.o 
PIC             =   PIC/pic.o           # pic output to be used with troff
PICT            =   PICT/pict.o         # pic output to be used with troff

#-------------------------------------------------------------------------------
# programming environment
#-------------------------------------------------------------------------------

ET_PROGENV      =   PROGENV/etprogenv.o

#-------------------------------------------------------------------------------
# configuration
#-------------------------------------------------------------------------------

#---- window system

WS_OFILES       =   $(SUNSERVER) $(NEWSSERVER) $(XSERVER) $(SUNWINDOW)
WS_IFDEFS       =   -DWS_SUNSERVER -DWS_NEWS -DWS_X -DWS_SUNWINDOW
WS_DIRS         =   SUNWINDOW XSERVER SERVER NEWS 

WS_OFILES       =   $(SUNWINDOW) $(XSERVER)
WS_IFDEFS       =   -DWS_SUNWINDOW -DWS_X 
WS_DIRS         =   SUNWINDOW XSERVER

#---- printer

PR_OFILES       =   $(POSTSCRIPT) $(PIC) $(PICT)
PR_IFDEFS       =   -DPR_POSTSCRIPT -DPR_PIC -DPR_PICT
PR_DIRS         =   POSTSCRIPT PIC PICT

#---- operating system

OS_OFILES       =   $(SUNOS)
OS_DIRS         =   SUNOS
OS_IFDEFS       =   -DOS_SUNOS

#---- programming environment

PE_OFILES       =   $(ET_PROGENV)
PE_IFDEFS       =   -DET_PROGENV
PE_DIRS         =   PROGENV

#-------------------------------------------------------------------------------
# end of configuration
#-------------------------------------------------------------------------------

STORAGEO        =   $(MALLOC)

DIRS            =   $(WS_DIRS) $(PR_DIRS) $(PE_DIRS) $(OS_DIRS) $(MALLOC_DIR) IO 

#-------------------------------------------------------------------------------

all:        et col

#-------------------------------------------------------------------------------
# basic data structures (non graphical classes)
#-------------------------------------------------------------------------------

OFILES1=    \
	    AccessMem.o \
	    Error.o \
	    FileType.o \
	    ProgEnv.o \
	    FixedStorage.o \
	    Iterator.o \
	    Root.o \
	    CType.o \
	    String.o \
	    Types.o \
	    Math.o \
	    regex.o \
	    $(OS_OFILES) \
	    BitSet.o \
	    ObjFloat.o \
	    Object.o \
	    IdDictionary.o \
	    Dictionary.o \
	    ObjInt.o \
	    PathLookup.o \
	    ByteArray.o \
	    Collection.o \
	    Bag.o \
	    SortedOList.o \
	    RegularExp.o \
	    OrdColl.o \
	    PttyConnection.o \
	    StreamConnection.o \
	    Directory.o \
	    System.o \
	    ObjList.o \
	    SeqColl.o \
	    Metric.o \
	    Rectangle.o \
	    Point.o \
	    ObjectTable.o \
	    Set.o \
	    ObjArray.o \
	    Class.o \
	    ClassManager.o \
	    $(STORAGEO) \
	    Storage.o \
	    IO/etio.o

ofiles1.o:  $(OFILES1)
	    $(LD) -X -r -o $@ $(OFILES1)

ObjectTable.o:     ObjectTable.C
	    $(ETCC) -c $(PE_IFDEFS) $<

#-------------------------------------------------------------------------------
# graphics interface
#-------------------------------------------------------------------------------

OFILES2 =   \
	    Ink.o \
	    Font.o \
	    Bitmap.o \
	    DevBitmap.o \
	    PictPort.o \
	    Picture.o \
	    PrintPort.o \
	    Token.o \
	    WindowPort.o \
	    WindowSystem.o \
	    Port.o

ofiles2.o:  $(OFILES2)
	    $(LD) -X -r -o $@ $(OFILES2)

#-------------------------------------------------------------------------------
# graphical classes
#-------------------------------------------------------------------------------

OFILES3=    \
	    Form.o \
	    ClipBoard.o \
	    GraphView.o \
	    MenuBar.o \
	    ShellTView.o \
	    VObjectTView.o \
	    VObjectText.o \
	    CodeTextView.o \
	    FixLineTView.o \
	    GotoDialog.o \
	    Splitter.o \
	    TreeView.o \
	    Icon.o \
	    View.o \
	    PrintDialog.o \
	    Printer.o \
	    Application.o \
	    TextView.o \
	    ChangeDialog.o \
	    FindDialog.o \
	    TextCmd.o \
	    FileDialog.o \
	    Document.o \
	    CmdHistDoc.o \
	    Panner.o \
	    Scroller.o \
	    ScrollBar.o \
	    Expander.o \
	    Slider.o \
	    Menu.o \
	    CollView.o \
	    Alert.o \
	    StyledText.o \
	    GapText.o \
	    RunArray.o \
	    EditTextItem.o \
	    LineItem.o \
	    EnumItem.o \
	    CycleItem.o \
	    BackgroundItem.o \
	    OneOfCluster.o \
	    ManyOfCluster.o \
	    ShadowItem.o \
	    NumItem.o \
	    FloatItem.o \
	    Buttons.o \
	    PopupItem.o \
	    CheapText.o \
	    Dialog.o \
	    RestrTView.o \
	    StaticTView.o \
	    Text.o \
	    Mark.o \
	    Window.o \
	    BorderItems.o \
	    Cluster.o \
	    Clipper.o \
	    BlankWin.o \
	    VObjectPair.o \
	    Filler.o \
	    TextItem.o \
	    ImageItem.o \
	    VObject.o \
	    EvtHandler.o \
	    TextFormatter.o \
	    Command.o

ofiles3.o:  $(OFILES3)
	    $(LD) -X -r -o $@ $(OFILES3)

MFLAGS  = \
	CFLAGS="$(CFLAGS)" \
	CCFLAGS="$(CCFLAGS)" \
	CC="$(CC)" \
	MAKE="$(MAKE)" \
	ETCC="$(ETCC)" \
	LD="$(LD)" 
	
#-------------------------------------------------------------------------------
# et
#-------------------------------------------------------------------------------

#ETO     =   $(PE_OFILES) ofiles3.o Init.o $(WS_OFILES) $(PR_OFILES) ofiles2.o ofiles1.o  
# workaround for ld bug in SunOS 4.1
ETO     =   $(PE_OFILES) $(OFILES3) Init.o $(WS_OFILES) $(PR_OFILES) $(OFILES2) $(OFILES1)  

ETDIRS  =   IO MALLOC $(DIRS) images

et:     etdirs et.o

etdirs: 
	-@for i in $(ETDIRS); do (echo $$i; cd $$i; $(MAKE) $(MFLAGS)); done

et.o:   $(ETO) FRC
	$(LD) -X -r -o $@ $(ETO)

libet.a:   $(ETO) FRC
	ar rv $@ $(ETO)
	ranlib $@

config:
	touch Init.C ObjectTable.C
	$(MAKE)

Init.o: Init.C
	$(ETCC) -c $(WS_IFDEFS) $(PE_IFDEFS) $(OS_IFDEFS) $(PR_IFDEFS) $<

#-------------------------------------------------------------------------------
# col
#-------------------------------------------------------------------------------

COLO    =   InitCol.o ofiles1.o
COLDIRS =   MALLOC SUNOS

col:    coldirs col.o InitCol.o

coldirs:
	-@for i in $(COLDIRS); do (echo $$i; cd $$i; $(MAKE)); done

col.o:  $(COLO) FRC
	$(LD) -X -r -o col.o $(COLO)

InitCol.o:  InitCol.C
	    $(ETCC) -c $(OS_IFDEFS) $<

#-------------------------------------------------------------------------------

depend:
	etmakedep $(CCFLAGS) *.[cC]
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $@); done

clean:
	rm -f core *..c errs .rsrc $(ETO)
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $@); done

clobber:    clean
	rm -f *.o .MAP/*.map
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $@); done

touch:
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $@); done
	$(MAKE) -t
	rm -f et.o col.o etdirs coldirs
	$(MAKE)
	
FRC:

# DO NOT DELETE THIS LINE -- make depend depends on it.

AccessMem.o: AccessMem.h Types.h IO/stream.h Types.h System.h Object.h Root.h
AccessMem.o: CmdNo.h Class.h
Alert.o: Alert.h Dialog.h View.h VObject.h EvtHandler.h Object.h Root.h
Alert.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Alert.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Alert.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Alert.o: Alert_e.h Buttons.h VObjectPair.h BorderItems.h CheapText.h Text.h
Alert.o: String.h Storage.h CType.h StaticTView.h Mark.h OrdColl.h SeqColl.h
Alert.o: ObjArray.h StyledText.h GapText.h RunArray.h ObjList.h
Alert.o: WindowSystem.h BlankWin.h Clipper.h ObjectTable.h images/note.image
Alert.o: images/caution.image images/stop.image images/sun.image
Alert.o: images/error.image
Application.o: ET++.h Port.h System.h Object.h Root.h Types.h IO/stream.h
Application.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h
Application.o: Ink.h Bitmap.h Token.h Application.h Dialog.h View.h VObject.h
Application.o: EvtHandler.h Command.h Collection.h Iterator.h Cluster.h
Application.o: Filler.h ImageItem.h TextItem.h Document.h FileType.h Window.h
Application.o: BlankWin.h Clipper.h BorderItems.h Scroller.h Splitter.h
Application.o: Expander.h Menu.h CollView.h LineItem.h VObjectPair.h String.h
Application.o: Storage.h CType.h Error.h Alert_e.h ObjList.h SeqColl.h
Application.o: IdDictionary.h Dictionary.h Set.h ObjArray.h WindowSystem.h
Application.o: FileDialog.h ObjectTable.h ClassManager.h ClipBoard.h
Application.o: ProgEnv.h FixedStorage.h Buttons.h
BackgroundItem.o: BackgroundItem.h VObject.h EvtHandler.h Object.h Root.h
BackgroundItem.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
BackgroundItem.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
BackgroundItem.o: Command.h Collection.h Iterator.h Cluster.h Filler.h
BackgroundItem.o: ImageItem.h TextItem.h
Bag.o: Bag.h Set.h ObjArray.h Collection.h Object.h Root.h Types.h
Bag.o: IO/stream.h Types.h CmdNo.h Class.h Iterator.h Error.h
BitSet.o: BitSet.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
BitSet.o: Class.h
Bitmap.o: Bitmap.h Port.h System.h Object.h Root.h Types.h IO/stream.h
Bitmap.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h Ink.h
Bitmap.o: Token.h DevBitmap.h Error.h Storage.h WindowSystem.h String.h
Bitmap.o: CType.h
BlankWin.o: BlankWin.h Clipper.h VObject.h EvtHandler.h Object.h Root.h
BlankWin.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
BlankWin.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
BlankWin.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
BlankWin.o: TextItem.h Error.h WindowPort.h WindowSystem.h Menu.h CollView.h
BlankWin.o: View.h LineItem.h VObjectPair.h BorderItems.h ObjectTable.h
BlankWin.o: ClassManager.h FixedStorage.h Storage.h
BorderItems.o: BorderItems.h VObject.h EvtHandler.h Object.h Root.h Types.h
BorderItems.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
BorderItems.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
BorderItems.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
BorderItems.o: TextItem.h String.h Storage.h CType.h
Buttons.o: Buttons.h VObject.h EvtHandler.h Object.h Root.h Types.h
Buttons.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Buttons.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Buttons.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Buttons.o: VObjectPair.h Dialog.h View.h WindowSystem.h
Buttons.o: images/RadioButtonOn.image images/RadioButtonOff.image
Buttons.o: images/RadioButtonFeedback.image images/CheckmarkOn.image
Buttons.o: images/CheckmarkOff.image images/CheckmarkInv.image
ByteArray.o: ByteArray.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
ByteArray.o: Class.h String.h Storage.h CType.h
CType.o: CType.h
ChangeDialog.o: RegularExp.h Object.h Root.h Types.h IO/stream.h Types.h
ChangeDialog.o: CmdNo.h Class.h ChangeDialog.h FindDialog.h Dialog.h View.h
ChangeDialog.o: VObject.h EvtHandler.h Port.h System.h Point.h Metric.h
ChangeDialog.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
ChangeDialog.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
ChangeDialog.o: TextItem.h Alert_e.h String.h Storage.h CType.h TextView.h
ChangeDialog.o: StaticTView.h Text.h Mark.h OrdColl.h SeqColl.h ObjArray.h
ChangeDialog.o: BorderItems.h OneOfCluster.h ManyOfCluster.h EditTextItem.h
ChangeDialog.o: Clipper.h Buttons.h VObjectPair.h Window.h BlankWin.h
ChangeDialog.o: Document.h FileType.h Expander.h Form.h
CheapText.o: CheapText.h Text.h Object.h Root.h Types.h IO/stream.h Types.h
CheapText.o: CmdNo.h Class.h String.h Storage.h CType.h Font.h Rectangle.h
CheapText.o: Metric.h Point.h Port.h System.h Ink.h Bitmap.h Token.h
CheapText.o: RegularExp.h Error.h
Class.o: Class.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Error.h
Class.o: Storage.h String.h CType.h ObjArray.h Collection.h Iterator.h
Class.o: OrdColl.h SeqColl.h ObjectTable.h ClassManager.h AccessMem.h
ClassManager.o: ClassManager.h Object.h Root.h Types.h IO/stream.h Types.h
ClassManager.o: CmdNo.h Class.h Error.h Storage.h String.h CType.h ObjArray.h
ClassManager.o: Collection.h Iterator.h OrdColl.h SeqColl.h SortedOList.h
ClassManager.o: ObjList.h Set.h ObjectTable.h
ClipBoard.o: ClipBoard.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
ClipBoard.o: Class.h View.h VObject.h EvtHandler.h Port.h System.h Point.h
ClipBoard.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
ClipBoard.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
ClipBoard.o: TextItem.h IO/membuf.h IO/streambuf.h ClassManager.h FileType.h
Clipper.o: Clipper.h VObject.h EvtHandler.h Object.h Root.h Types.h
Clipper.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Clipper.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Clipper.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Clipper.o: View.h Error.h BlankWin.h ObjList.h SeqColl.h String.h Storage.h
Clipper.o: CType.h WindowPort.h WindowSystem.h
Cluster.o: Cluster.h VObject.h EvtHandler.h Object.h Root.h Types.h
Cluster.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Cluster.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Cluster.o: Collection.h Iterator.h Filler.h ImageItem.h TextItem.h Math.h
Cluster.o: Storage.h
CmdHistDoc.o: CmdHistDoc.h Document.h EvtHandler.h Object.h Root.h Types.h
CmdHistDoc.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
CmdHistDoc.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h FileType.h
CmdHistDoc.o: ObjList.h SeqColl.h Collection.h Iterator.h Menu.h CollView.h
CmdHistDoc.o: View.h VObject.h Command.h Cluster.h Filler.h ImageItem.h
CmdHistDoc.o: TextItem.h LineItem.h VObjectPair.h BorderItems.h Window.h
CmdHistDoc.o: BlankWin.h Clipper.h
CodeTextView.o: Port.h System.h Object.h Root.h Types.h IO/stream.h Types.h
CodeTextView.o: CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h Ink.h
CodeTextView.o: Bitmap.h Token.h FixLineTView.h TextView.h StaticTView.h
CodeTextView.o: Text.h String.h Storage.h CType.h Mark.h OrdColl.h SeqColl.h
CodeTextView.o: Collection.h Iterator.h ObjArray.h View.h VObject.h
CodeTextView.o: EvtHandler.h Command.h Cluster.h Filler.h ImageItem.h
CodeTextView.o: TextItem.h CodeTextView.h StyledText.h GapText.h RunArray.h
CollView.o: CollView.h View.h VObject.h EvtHandler.h Object.h Root.h Types.h
CollView.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
CollView.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
CollView.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
CollView.o: Menu.h LineItem.h VObjectPair.h BorderItems.h String.h Storage.h
CollView.o: CType.h Error.h Math.h
Collection.o: Error.h Types.h IO/stream.h Types.h Collection.h Object.h
Collection.o: Root.h CmdNo.h Class.h Iterator.h OrdColl.h SeqColl.h
Collection.o: ObjArray.h Set.h Bag.h ObjList.h SortedOList.h String.h
Collection.o: Storage.h CType.h
Command.o: Command.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
Command.o: Class.h Point.h Error.h String.h Storage.h CType.h Port.h System.h
Command.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h ObjectTable.h
CycleItem.o: CycleItem.h VObject.h EvtHandler.h Object.h Root.h Types.h
CycleItem.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
CycleItem.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
CycleItem.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
CycleItem.o: TextItem.h
DevBitmap.o: DevBitmap.h Root.h Types.h IO/stream.h Types.h Point.h Class.h
DevBitmap.o: Object.h CmdNo.h Ink.h
Dialog.o: Dialog.h View.h VObject.h EvtHandler.h Object.h Root.h Types.h
Dialog.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Dialog.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Dialog.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Dialog.o: Buttons.h VObjectPair.h OrdColl.h SeqColl.h ObjArray.h Menu.h
Dialog.o: CollView.h LineItem.h BorderItems.h Error.h Window.h BlankWin.h
Dialog.o: Clipper.h
Dictionary.o: Dictionary.h Set.h ObjArray.h Collection.h Object.h Root.h
Dictionary.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Iterator.h Bag.h
Dictionary.o: Error.h
Directory.o: Directory.h Root.h
Document.o: ET++.h Port.h System.h Object.h Root.h Types.h IO/stream.h
Document.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h Ink.h
Document.o: Bitmap.h Token.h Application.h Dialog.h View.h VObject.h
Document.o: EvtHandler.h Command.h Collection.h Iterator.h Cluster.h Filler.h
Document.o: ImageItem.h TextItem.h Document.h FileType.h Window.h BlankWin.h
Document.o: Clipper.h BorderItems.h Scroller.h Splitter.h Expander.h Menu.h
Document.o: CollView.h LineItem.h VObjectPair.h String.h Storage.h CType.h
Document.o: Error.h Alert_e.h FileDialog.h Icon.h ObjList.h SeqColl.h
Document.o: ObjectTable.h ClassManager.h images/file.im
EditTextItem.o: EditTextItem.h Clipper.h VObject.h EvtHandler.h Object.h
EditTextItem.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h
EditTextItem.o: System.h Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h
EditTextItem.o: Token.h Command.h Collection.h Iterator.h Cluster.h Filler.h
EditTextItem.o: ImageItem.h TextItem.h TextView.h StaticTView.h Text.h
EditTextItem.o: String.h Storage.h CType.h Mark.h OrdColl.h SeqColl.h
EditTextItem.o: ObjArray.h View.h CheapText.h RestrTView.h BlankWin.h
EditTextItem.o: Dialog.h
EnumItem.o: EnumItem.h Cluster.h VObject.h EvtHandler.h Object.h Root.h
EnumItem.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
EnumItem.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
EnumItem.o: Command.h Collection.h Iterator.h Filler.h ImageItem.h TextItem.h
EnumItem.o: Expander.h Buttons.h VObjectPair.h ObjectTable.h
EnumItem.o: images/SliderButtonUp.image images/SliderButtonDown.image
EnumItem.o: images/SliderButtonUpInv.image images/SliderButtonDownInv.image
Error.o: Error.h Types.h IO/stream.h Types.h String.h Storage.h CType.h
Error.o: System.h Object.h Root.h CmdNo.h Class.h
EvtHandler.o: EvtHandler.h Object.h Root.h Types.h IO/stream.h Types.h
EvtHandler.o: CmdNo.h Class.h Port.h System.h Point.h Metric.h Rectangle.h
EvtHandler.o: Font.h Ink.h Bitmap.h Token.h Command.h View.h VObject.h
EvtHandler.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
EvtHandler.o: TextItem.h Clipper.h
Expander.o: Expander.h VObject.h EvtHandler.h Object.h Root.h Types.h
Expander.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Expander.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Expander.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
FileDialog.o: ET++.h Port.h System.h Object.h Root.h Types.h IO/stream.h
FileDialog.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h
FileDialog.o: Ink.h Bitmap.h Token.h Application.h Dialog.h View.h VObject.h
FileDialog.o: EvtHandler.h Command.h Collection.h Iterator.h Cluster.h
FileDialog.o: Filler.h ImageItem.h TextItem.h Document.h FileType.h Window.h
FileDialog.o: BlankWin.h Clipper.h BorderItems.h Scroller.h Splitter.h
FileDialog.o: Expander.h Menu.h CollView.h LineItem.h VObjectPair.h String.h
FileDialog.o: Storage.h CType.h Error.h Alert_e.h FileDialog.h PopupItem.h
FileDialog.o: Buttons.h EditTextItem.h TextView.h StaticTView.h Text.h Mark.h
FileDialog.o: OrdColl.h SeqColl.h ObjArray.h ScrollBar.h CheapText.h
FileDialog.o: Directory.h images/directory.im images/file.im images/exec.im
FileDialog.o: images/ascii.im
FileType.o: FileType.h Root.h Types.h IO/stream.h Types.h System.h Object.h
FileType.o: CmdNo.h Class.h String.h Storage.h CType.h Error.h
Filler.o: Filler.h VObject.h EvtHandler.h Object.h Root.h Types.h IO/stream.h
Filler.o: Types.h CmdNo.h Class.h Port.h System.h Point.h Metric.h
Filler.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h Collection.h
Filler.o: Iterator.h Cluster.h ImageItem.h TextItem.h
FindDialog.o: RegularExp.h Object.h Root.h Types.h IO/stream.h Types.h
FindDialog.o: CmdNo.h Class.h FindDialog.h Dialog.h View.h VObject.h
FindDialog.o: EvtHandler.h Port.h System.h Point.h Metric.h Rectangle.h
FindDialog.o: Font.h Ink.h Bitmap.h Token.h Command.h Collection.h Iterator.h
FindDialog.o: Cluster.h Filler.h ImageItem.h TextItem.h Alert_e.h String.h
FindDialog.o: Storage.h CType.h BorderItems.h OneOfCluster.h ManyOfCluster.h
FindDialog.o: EditTextItem.h Clipper.h TextView.h StaticTView.h Text.h Mark.h
FindDialog.o: OrdColl.h SeqColl.h ObjArray.h Buttons.h VObjectPair.h Window.h
FindDialog.o: BlankWin.h Document.h FileType.h Expander.h Form.h
FixLineTView.o: FixLineTView.h TextView.h StaticTView.h Text.h Object.h
FixLineTView.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h String.h
FixLineTView.o: Storage.h CType.h Font.h Rectangle.h Metric.h Point.h Port.h
FixLineTView.o: System.h Ink.h Bitmap.h Token.h Mark.h OrdColl.h SeqColl.h
FixLineTView.o: Collection.h Iterator.h ObjArray.h View.h VObject.h
FixLineTView.o: EvtHandler.h Command.h Cluster.h Filler.h ImageItem.h
FixLineTView.o: TextItem.h
FixedStorage.o: FixedStorage.h Types.h IO/stream.h Types.h Storage.h String.h
FixedStorage.o: CType.h Error.h
FloatItem.o: FloatItem.h EditTextItem.h Clipper.h VObject.h EvtHandler.h
FloatItem.o: Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
FloatItem.o: Port.h System.h Point.h Metric.h Rectangle.h Font.h Ink.h
FloatItem.o: Bitmap.h Token.h Command.h Collection.h Iterator.h Cluster.h
FloatItem.o: Filler.h ImageItem.h TextItem.h TextView.h StaticTView.h Text.h
FloatItem.o: String.h Storage.h CType.h Mark.h OrdColl.h SeqColl.h ObjArray.h
FloatItem.o: View.h CheapText.h RestrTView.h RegularExp.h Alert_e.h
Font.o: Font.h Root.h Rectangle.h Metric.h Point.h Types.h IO/stream.h
Font.o: Types.h Class.h Object.h CmdNo.h Error.h String.h Storage.h CType.h
Font.o: WindowSystem.h Port.h System.h Ink.h Bitmap.h Token.h
Form.o: Form.h Cluster.h VObject.h EvtHandler.h Object.h Root.h Types.h
Form.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h Metric.h
Form.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h Collection.h
Form.o: Iterator.h Filler.h ImageItem.h TextItem.h BorderItems.h OrdColl.h
Form.o: SeqColl.h ObjArray.h
GapText.o: GapText.h Text.h Object.h Root.h Types.h IO/stream.h Types.h
GapText.o: CmdNo.h Class.h String.h Storage.h CType.h Font.h Rectangle.h
GapText.o: Metric.h Point.h Port.h System.h Ink.h Bitmap.h Token.h
GapText.o: RegularExp.h Error.h
GotoDialog.o: GotoDialog.h GotoDialog_e.h Dialog.h View.h VObject.h
GotoDialog.o: EvtHandler.h Object.h Root.h Types.h IO/stream.h Types.h
GotoDialog.o: CmdNo.h Class.h Port.h System.h Point.h Metric.h Rectangle.h
GotoDialog.o: Font.h Ink.h Bitmap.h Token.h Command.h Collection.h Iterator.h
GotoDialog.o: Cluster.h Filler.h ImageItem.h TextItem.h BlankWin.h Clipper.h
GotoDialog.o: NumItem.h EditTextItem.h TextView.h StaticTView.h Text.h
GotoDialog.o: String.h Storage.h CType.h Mark.h OrdColl.h SeqColl.h
GotoDialog.o: ObjArray.h BorderItems.h Buttons.h VObjectPair.h EnumItem.h
GotoDialog.o: Document.h FileType.h WindowSystem.h ObjectTable.h
GraphView.o: ObjList.h SeqColl.h Collection.h Object.h Root.h Types.h
GraphView.o: IO/stream.h Types.h CmdNo.h Class.h Iterator.h IdDictionary.h
GraphView.o: Dictionary.h Set.h ObjArray.h OrdColl.h Dialog.h View.h
GraphView.o: VObject.h EvtHandler.h Port.h System.h Point.h Metric.h
GraphView.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h Cluster.h
GraphView.o: Filler.h ImageItem.h TextItem.h GraphView.h TreeView.h
Icon.o: Icon.h BlankWin.h Clipper.h VObject.h EvtHandler.h Object.h Root.h
Icon.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Icon.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Icon.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Icon.o: Menu.h CollView.h View.h LineItem.h VObjectPair.h BorderItems.h
Icon.o: String.h Storage.h CType.h
IdDictionary.o: IdDictionary.h Dictionary.h Set.h ObjArray.h Collection.h
IdDictionary.o: Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
IdDictionary.o: Iterator.h
ImageItem.o: ImageItem.h VObject.h EvtHandler.h Object.h Root.h Types.h
ImageItem.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
ImageItem.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
ImageItem.o: Collection.h Iterator.h Cluster.h Filler.h TextItem.h
Init.o: System.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
Init.o: WindowSystem.h Port.h Point.h Metric.h Rectangle.h Font.h Ink.h
Init.o: Bitmap.h Token.h ProgEnv.h Error.h
InitCol.o: System.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
InitCol.o: Class.h ProgEnv.h Error.h
Ink.o: Ink.h Types.h IO/stream.h Types.h Object.h Root.h CmdNo.h Class.h
Ink.o: String.h Storage.h CType.h Port.h System.h Point.h Metric.h
Ink.o: Rectangle.h Font.h Bitmap.h Token.h WindowSystem.h
Iterator.o: Iterator.h Root.h Types.h IO/stream.h Types.h Collection.h
Iterator.o: Object.h CmdNo.h Class.h Error.h
LineItem.o: LineItem.h VObject.h EvtHandler.h Object.h Root.h Types.h
LineItem.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
LineItem.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
LineItem.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
ManyOfCluster.o: ManyOfCluster.h Cluster.h VObject.h EvtHandler.h Object.h
ManyOfCluster.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h
ManyOfCluster.o: System.h Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h
ManyOfCluster.o: Token.h Command.h Collection.h Iterator.h Filler.h
ManyOfCluster.o: ImageItem.h TextItem.h Buttons.h VObjectPair.h OrdColl.h
ManyOfCluster.o: SeqColl.h ObjArray.h
Mark.o: Mark.h OrdColl.h SeqColl.h Collection.h Object.h Root.h Types.h
Mark.o: IO/stream.h Types.h CmdNo.h Class.h Iterator.h ObjArray.h
Math.o: Math.h Types.h IO/stream.h Types.h
Menu.o: Menu.h CollView.h View.h VObject.h EvtHandler.h Object.h Root.h
Menu.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Menu.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Menu.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Menu.o: LineItem.h VObjectPair.h BorderItems.h SortedOList.h ObjList.h
Menu.o: SeqColl.h OrdColl.h ObjArray.h String.h Storage.h CType.h BlankWin.h
Menu.o: Clipper.h WindowSystem.h
MenuBar.o: MenuBar.h Clipper.h VObject.h EvtHandler.h Object.h Root.h Types.h
MenuBar.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
MenuBar.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
MenuBar.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
MenuBar.o: Menu.h CollView.h View.h LineItem.h VObjectPair.h BorderItems.h
MenuBar.o: OrdColl.h SeqColl.h ObjArray.h
Metric.o: Metric.h Point.h Types.h IO/stream.h Types.h Class.h Object.h
Metric.o: Root.h CmdNo.h String.h Storage.h CType.h
NumItem.o: NumItem.h EditTextItem.h Clipper.h VObject.h EvtHandler.h Object.h
NumItem.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
NumItem.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
NumItem.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
NumItem.o: TextItem.h TextView.h StaticTView.h Text.h String.h Storage.h
NumItem.o: CType.h Mark.h OrdColl.h SeqColl.h ObjArray.h View.h CheapText.h
NumItem.o: RestrTView.h RegularExp.h Alert_e.h
ObjArray.o: ObjArray.h Collection.h Object.h Root.h Types.h IO/stream.h
ObjArray.o: Types.h CmdNo.h Class.h Iterator.h Error.h Storage.h
ObjArray.o: FixedStorage.h String.h CType.h
ObjFloat.o: ObjFloat.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
ObjFloat.o: Class.h String.h Storage.h CType.h
ObjInt.o: ObjInt.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
ObjInt.o: Class.h String.h Storage.h CType.h
ObjList.o: ObjList.h SeqColl.h Collection.h Object.h Root.h Types.h
ObjList.o: IO/stream.h Types.h CmdNo.h Class.h Iterator.h FixedStorage.h
ObjList.o: System.h Storage.h
Object.o: Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
Object.o: OrdColl.h SeqColl.h Collection.h Iterator.h ObjArray.h
Object.o: IdDictionary.h Dictionary.h Set.h ObjInt.h Error.h String.h
Object.o: Storage.h CType.h ProgEnv.h System.h ObjectTable.h ClassManager.h
Object.o: AccessMem.h IO/membuf.h IO/streambuf.h
ObjectTable.o: Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
ObjectTable.o: OrdColl.h SeqColl.h Collection.h Iterator.h ObjArray.h Set.h
ObjectTable.o: ObjectTable.h System.h Error.h ClassManager.h AccessMem.h
ObjectTable.o: String.h Storage.h CType.h
OneOfCluster.o: OneOfCluster.h Cluster.h VObject.h EvtHandler.h Object.h
OneOfCluster.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h
OneOfCluster.o: System.h Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h
OneOfCluster.o: Token.h Command.h Collection.h Iterator.h Filler.h
OneOfCluster.o: ImageItem.h TextItem.h Buttons.h VObjectPair.h OrdColl.h
OneOfCluster.o: SeqColl.h ObjArray.h
OrdColl.o: OrdColl.h SeqColl.h Collection.h Object.h Root.h Types.h
OrdColl.o: IO/stream.h Types.h CmdNo.h Class.h Iterator.h ObjArray.h Error.h
OrdColl.o: FixedStorage.h
Panner.o: Panner.h Slider.h VObject.h EvtHandler.h Object.h Root.h Types.h
Panner.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Panner.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Panner.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Panner.o: Clipper.h
PathLookup.o: ByteArray.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
PathLookup.o: Class.h PathLookup.h OrdColl.h SeqColl.h Collection.h
PathLookup.o: Iterator.h ObjArray.h System.h String.h Storage.h CType.h
PictPort.o: PictPort.h PrintPort.h Port.h System.h Object.h Root.h Types.h
PictPort.o: IO/stream.h Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h
PictPort.o: Font.h Ink.h Bitmap.h Token.h Picture.h
Picture.o: Picture.h Port.h System.h Object.h Root.h Types.h IO/stream.h
Picture.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h Ink.h
Picture.o: Bitmap.h Token.h String.h Storage.h CType.h ObjArray.h
Picture.o: Collection.h Iterator.h
Point.o: Point.h Types.h IO/stream.h Types.h Class.h Object.h Root.h CmdNo.h
Point.o: Math.h String.h Storage.h CType.h
PopupItem.o: PopupItem.h Buttons.h VObject.h EvtHandler.h Object.h Root.h
PopupItem.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
PopupItem.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
PopupItem.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
PopupItem.o: TextItem.h VObjectPair.h ShadowItem.h Menu.h CollView.h View.h
PopupItem.o: LineItem.h BorderItems.h
Port.o: Port.h System.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
Port.o: Class.h Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
Port.o: String.h Storage.h CType.h Error.h Picture.h WindowSystem.h
Port.o: DevBitmap.h
PrintDialog.o: PrintDialog.h Dialog.h View.h VObject.h EvtHandler.h Object.h
PrintDialog.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h
PrintDialog.o: System.h Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h
PrintDialog.o: Token.h Command.h Collection.h Iterator.h Cluster.h Filler.h
PrintDialog.o: ImageItem.h TextItem.h CheapText.h Text.h String.h Storage.h
PrintDialog.o: CType.h OrdColl.h SeqColl.h ObjArray.h ObjectTable.h Error.h
PrintDialog.o: Window.h BlankWin.h Clipper.h BorderItems.h PrintPort.h
PrintDialog.o: Document.h FileType.h FileDialog.h VObjectPair.h ObjList.h
PrintDialog.o: CollView.h Scroller.h Printer.h Alert_e.h NumItem.h
PrintDialog.o: EditTextItem.h TextView.h StaticTView.h Mark.h Buttons.h
PrintPort.o: PrintPort.h Port.h System.h Object.h Root.h Types.h IO/stream.h
PrintPort.o: Types.h CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h
PrintPort.o: Ink.h Bitmap.h Token.h String.h Storage.h CType.h Error.h
Printer.o: Printer.h Dialog.h View.h VObject.h EvtHandler.h Object.h Root.h
Printer.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
Printer.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
Printer.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
Printer.o: TextItem.h BlankWin.h Clipper.h Buttons.h VObjectPair.h
Printer.o: BorderItems.h PrintPort.h
ProgEnv.o: ProgEnv.h Types.h IO/stream.h Types.h Object.h Root.h CmdNo.h
ProgEnv.o: Class.h
PttyConnection.o: PttyConnection.h Types.h IO/stream.h Types.h Root.h
Rectangle.o: Rectangle.h Metric.h Point.h Types.h IO/stream.h Types.h Class.h
Rectangle.o: Object.h Root.h CmdNo.h Error.h Math.h String.h Storage.h
Rectangle.o: CType.h
RegularExp.o: Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
RegularExp.o: RegularExp.h String.h Storage.h CType.h ObjectTable.h
RestrTView.o: RestrTView.h TextView.h StaticTView.h Text.h Object.h Root.h
RestrTView.o: Types.h IO/stream.h Types.h CmdNo.h Class.h String.h Storage.h
RestrTView.o: CType.h Font.h Rectangle.h Metric.h Point.h Port.h System.h
RestrTView.o: Ink.h Bitmap.h Token.h Mark.h OrdColl.h SeqColl.h Collection.h
RestrTView.o: Iterator.h ObjArray.h View.h VObject.h EvtHandler.h Command.h
RestrTView.o: Cluster.h Filler.h ImageItem.h TextItem.h RegularExp.h
RestrTView.o: BlankWin.h Clipper.h
Root.o: Root.h
RunArray.o: RunArray.h Collection.h Object.h Root.h Types.h IO/stream.h
RunArray.o: Types.h CmdNo.h Class.h Iterator.h Error.h String.h Storage.h
RunArray.o: CType.h
ScrollBar.o: ScrollBar.h Expander.h VObject.h EvtHandler.h Object.h Root.h
ScrollBar.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
ScrollBar.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
ScrollBar.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
ScrollBar.o: TextItem.h Slider.h Buttons.h VObjectPair.h ObjectTable.h
ScrollBar.o: images/UpArrow.image images/UpArrowInv.image
ScrollBar.o: images/DownArrow.image images/DownArrowInv.image
ScrollBar.o: images/LeftArrow.image images/LeftArrowInv.image
ScrollBar.o: images/RightArrow.image images/RightArrowInv.image
Scroller.o: Scroller.h Cluster.h VObject.h EvtHandler.h Object.h Root.h
Scroller.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
Scroller.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
Scroller.o: Command.h Collection.h Iterator.h Filler.h ImageItem.h TextItem.h
Scroller.o: ScrollBar.h Expander.h Clipper.h View.h
SeqColl.o: SeqColl.h Collection.h Object.h Root.h Types.h IO/stream.h Types.h
SeqColl.o: CmdNo.h Class.h Iterator.h
Set.o: Set.h ObjArray.h Collection.h Object.h Root.h Types.h IO/stream.h
Set.o: Types.h CmdNo.h Class.h Iterator.h Error.h Math.h
ShadowItem.o: ShadowItem.h VObject.h EvtHandler.h Object.h Root.h Types.h
ShadowItem.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
ShadowItem.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
ShadowItem.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
ShadowItem.o: TextItem.h
ShellTView.o: Port.h System.h Object.h Root.h Types.h IO/stream.h Types.h
ShellTView.o: CmdNo.h Class.h Point.h Metric.h Rectangle.h Font.h Ink.h
ShellTView.o: Bitmap.h Token.h Iterator.h ShellTView.h FixLineTView.h
ShellTView.o: TextView.h StaticTView.h Text.h String.h Storage.h CType.h
ShellTView.o: Mark.h OrdColl.h SeqColl.h Collection.h ObjArray.h View.h
ShellTView.o: VObject.h EvtHandler.h Command.h Cluster.h Filler.h ImageItem.h
ShellTView.o: TextItem.h PttyConnection.h StyledText.h GapText.h RunArray.h
ShellTView.o: CheapText.h Alert_e.h Menu.h CollView.h LineItem.h
ShellTView.o: VObjectPair.h BorderItems.h BlankWin.h Clipper.h WindowSystem.h
ShellTView.o: images/revCaret.image
Slider.o: Slider.h VObject.h EvtHandler.h Object.h Root.h Types.h IO/stream.h
Slider.o: Types.h CmdNo.h Class.h Port.h System.h Point.h Metric.h
Slider.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h Collection.h
Slider.o: Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
SortedOList.o: SortedOList.h ObjList.h SeqColl.h Collection.h Object.h Root.h
SortedOList.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Iterator.h
Splitter.o: Splitter.h VObject.h EvtHandler.h Object.h Root.h Types.h
Splitter.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
Splitter.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
Splitter.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h TextItem.h
Splitter.o: Scroller.h
StaticTView.o: StaticTView.h Text.h Object.h Root.h Types.h IO/stream.h
StaticTView.o: Types.h CmdNo.h Class.h String.h Storage.h CType.h Font.h
StaticTView.o: Rectangle.h Metric.h Point.h Port.h System.h Ink.h Bitmap.h
StaticTView.o: Token.h Mark.h OrdColl.h SeqColl.h Collection.h Iterator.h
StaticTView.o: ObjArray.h View.h VObject.h EvtHandler.h Command.h Cluster.h
StaticTView.o: Filler.h ImageItem.h TextItem.h BlankWin.h Clipper.h
StaticTView.o: FixedStorage.h TextFormatter.h
Storage.o: Storage.h Types.h IO/stream.h Types.h System.h Object.h Root.h
Storage.o: CmdNo.h Class.h Error.h FixedStorage.h MALLOC/storage.h
StreamConnection.o: StreamConnection.h System.h Object.h Root.h Types.h
StreamConnection.o: IO/stream.h Types.h CmdNo.h Class.h SUNWINDOW/Clipboard.h
StreamConnection.o: Error.h
String.o: String.h Types.h IO/stream.h Types.h Storage.h CType.h Error.h
StyledText.o: StyledText.h GapText.h Text.h Object.h Root.h Types.h
StyledText.o: IO/stream.h Types.h CmdNo.h Class.h String.h Storage.h CType.h
StyledText.o: Font.h Rectangle.h Metric.h Point.h Port.h System.h Ink.h
StyledText.o: Bitmap.h Token.h RunArray.h Collection.h Iterator.h Mark.h
StyledText.o: OrdColl.h SeqColl.h ObjArray.h Error.h ObjectTable.h
System.o: System.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h
System.o: Class.h ObjList.h SeqColl.h Collection.h Iterator.h FileType.h
System.o: ObjectTable.h String.h Storage.h CType.h Error.h Directory.h
System.o: OrdColl.h ObjArray.h
Text.o: Text.h Object.h Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h
Text.o: String.h Storage.h CType.h Font.h Rectangle.h Metric.h Point.h Port.h
Text.o: System.h Ink.h Bitmap.h Token.h Error.h RegularExp.h Mark.h OrdColl.h
Text.o: SeqColl.h Collection.h Iterator.h ObjArray.h
TextCmd.o: TextView.h StaticTView.h Text.h Object.h Root.h Types.h
TextCmd.o: IO/stream.h Types.h CmdNo.h Class.h String.h Storage.h CType.h
TextCmd.o: Font.h Rectangle.h Metric.h Point.h Port.h System.h Ink.h Bitmap.h
TextCmd.o: Token.h Mark.h OrdColl.h SeqColl.h Collection.h Iterator.h
TextCmd.o: ObjArray.h View.h VObject.h EvtHandler.h Command.h Cluster.h
TextCmd.o: Filler.h ImageItem.h TextItem.h TextCmd.h StyledText.h GapText.h
TextCmd.o: RunArray.h
TextFormatter.o: TextFormatter.h Object.h Root.h Types.h IO/stream.h Types.h
TextFormatter.o: CmdNo.h Class.h StaticTView.h Text.h String.h Storage.h
TextFormatter.o: CType.h Font.h Rectangle.h Metric.h Point.h Port.h System.h
TextFormatter.o: Ink.h Bitmap.h Token.h Mark.h OrdColl.h SeqColl.h
TextFormatter.o: Collection.h Iterator.h ObjArray.h View.h VObject.h
TextFormatter.o: EvtHandler.h Command.h Cluster.h Filler.h ImageItem.h
TextFormatter.o: TextItem.h
TextItem.o: TextItem.h VObject.h EvtHandler.h Object.h Root.h Types.h
TextItem.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
TextItem.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
TextItem.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h String.h
TextItem.o: Storage.h CType.h
TextView.o: TextView.h StaticTView.h Text.h Object.h Root.h Types.h
TextView.o: IO/stream.h Types.h CmdNo.h Class.h String.h Storage.h CType.h
TextView.o: Font.h Rectangle.h Metric.h Point.h Port.h System.h Ink.h
TextView.o: Bitmap.h Token.h Mark.h OrdColl.h SeqColl.h Collection.h
TextView.o: Iterator.h ObjArray.h View.h VObject.h EvtHandler.h Command.h
TextView.o: Cluster.h Filler.h ImageItem.h TextItem.h TextCmd.h StyledText.h
TextView.o: GapText.h RunArray.h BlankWin.h Clipper.h Menu.h CollView.h
TextView.o: LineItem.h VObjectPair.h BorderItems.h Error.h ChangeDialog.h
TextView.o: FindDialog.h Dialog.h Document.h FileType.h RegularExp.h
TextView.o: ClipBoard.h TextFormatter.h ObjectTable.h images/Caret.image
Token.o: Token.h Rectangle.h Metric.h Point.h Types.h IO/stream.h Types.h
Token.o: Class.h Object.h Root.h CmdNo.h
TreeView.o: TreeView.h Dialog.h View.h VObject.h EvtHandler.h Object.h Root.h
TreeView.o: Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
TreeView.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
TreeView.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
TreeView.o: TextItem.h Menu.h CollView.h LineItem.h VObjectPair.h
TreeView.o: BorderItems.h String.h Storage.h CType.h OrdColl.h SeqColl.h
TreeView.o: ObjArray.h
Types.o: Types.h IO/stream.h Types.h String.h Storage.h CType.h Class.h
Types.o: Object.h Root.h CmdNo.h
VObject.o: VObject.h EvtHandler.h Object.h Root.h Types.h IO/stream.h Types.h
VObject.o: CmdNo.h Class.h Port.h System.h Point.h Metric.h Rectangle.h
VObject.o: Font.h Ink.h Bitmap.h Token.h Command.h Collection.h Iterator.h
VObject.o: Cluster.h Filler.h ImageItem.h TextItem.h Menu.h CollView.h View.h
VObject.o: LineItem.h VObjectPair.h BorderItems.h BlankWin.h Clipper.h
VObject.o: WindowSystem.h String.h Storage.h CType.h OrdColl.h SeqColl.h
VObject.o: ObjArray.h
VObjectPair.o: VObjectPair.h VObject.h EvtHandler.h Object.h Root.h Types.h
VObjectPair.o: IO/stream.h Types.h CmdNo.h Class.h Port.h System.h Point.h
VObjectPair.o: Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h Command.h
VObjectPair.o: Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
VObjectPair.o: TextItem.h
VObjectTView.o: VObjectTView.h TextView.h StaticTView.h Text.h Object.h
VObjectTView.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h String.h
VObjectTView.o: Storage.h CType.h Font.h Rectangle.h Metric.h Point.h Port.h
VObjectTView.o: System.h Ink.h Bitmap.h Token.h Mark.h OrdColl.h SeqColl.h
VObjectTView.o: Collection.h Iterator.h ObjArray.h View.h VObject.h
VObjectTView.o: EvtHandler.h Command.h Cluster.h Filler.h ImageItem.h
VObjectTView.o: TextItem.h VObjectText.h StyledText.h GapText.h RunArray.h
VObjectTView.o: Clipper.h
VObjectText.o: VObjectText.h StyledText.h GapText.h Text.h Object.h Root.h
VObjectText.o: Types.h IO/stream.h Types.h CmdNo.h Class.h String.h Storage.h
VObjectText.o: CType.h Font.h Rectangle.h Metric.h Point.h Port.h System.h
VObjectText.o: Ink.h Bitmap.h Token.h RunArray.h Collection.h Iterator.h
VObjectText.o: Mark.h OrdColl.h SeqColl.h ObjArray.h TextView.h StaticTView.h
VObjectText.o: View.h VObject.h EvtHandler.h Command.h Cluster.h Filler.h
VObjectText.o: ImageItem.h TextItem.h
View.o: View.h VObject.h EvtHandler.h Object.h Root.h Types.h IO/stream.h
View.o: Types.h CmdNo.h Class.h Port.h System.h Point.h Metric.h Rectangle.h
View.o: Font.h Ink.h Bitmap.h Token.h Command.h Collection.h Iterator.h
View.o: Cluster.h Filler.h ImageItem.h TextItem.h Error.h Document.h
View.o: FileType.h Menu.h CollView.h LineItem.h VObjectPair.h BorderItems.h
View.o: ClipBoard.h PrintDialog.h Dialog.h Alert_e.h Clipper.h OrdColl.h
View.o: SeqColl.h ObjArray.h String.h Storage.h CType.h
Window.o: Window.h BlankWin.h Clipper.h VObject.h EvtHandler.h Object.h
Window.o: Root.h Types.h IO/stream.h Types.h CmdNo.h Class.h Port.h System.h
Window.o: Point.h Metric.h Rectangle.h Font.h Ink.h Bitmap.h Token.h
Window.o: Command.h Collection.h Iterator.h Cluster.h Filler.h ImageItem.h
Window.o: TextItem.h BorderItems.h Error.h Menu.h CollView.h View.h
Window.o: LineItem.h VObjectPair.h String.h Storage.h CType.h Buttons.h
Window.o: WindowSystem.h WindowPort.h ObjectTable.h images/StretchBoxNE.image
Window.o: images/StretchBoxNW.image images/StretchBoxSE.image
Window.o: images/StretchBoxSW.image
WindowPort.o: WindowPort.h Port.h System.h Object.h Root.h Types.h
WindowPort.o: IO/stream.h Types.h CmdNo.h Class.h Point.h Metric.h
WindowPort.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h WindowSystem.h
WindowPort.o: Error.h Math.h Storage.h
WindowSystem.o: WindowSystem.h Port.h System.h Object.h Root.h Types.h
WindowSystem.o: IO/stream.h Types.h CmdNo.h Class.h Point.h Metric.h
WindowSystem.o: Rectangle.h Font.h Ink.h Bitmap.h Token.h String.h Storage.h
WindowSystem.o: CType.h Error.h WindowPort.h ClipBoard.h DevBitmap.h
regex.o: regex.h
