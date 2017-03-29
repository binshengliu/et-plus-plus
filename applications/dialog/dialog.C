//$myDialogView,myDocument,dialog$
#include "ET++.h"

#include "Dialog.h"
#include "DialogItems.h"
#include "Form.h"
#include "PopupItem.h"
#include "CheapText.h"
#include "StyledText.h"
#include "TextView.h"
#include "ObjList.h"
#include "CycleItem.h"

//---- myDialogView ------------------------------------------------------------

class myDialogView: public DialogView {
    Menu *mymenu1, *mymenu2, *mymenu3;
    ObjList *mylist;
    CollectionView *collview;
    StyledText *st;
    TextView *tv;
public:
    myDialogView(Document *dp);
    VObject *DoCreateDialog();
};

myDialogView::myDialogView(Document *dp) : DialogView(dp)
{
    mymenu1= new Menu("Baud", FALSE, 0, 1, FALSE);
    mymenu1->AppendItems("50", 50,
			"110", 51,
			"150", 52,
			"300", 53,
			"600", 54,
			"1200", 55,
			"2400", 56,
			"3600", 57,
			"4800", 58,
			"7200", 59,
			"9600", 60,
			"19200", 61,
			"110", 151,
			"150", 152,
			"300", 153,
			"600", 154,
			"1200", 155,
			"2400", 156,
			"3600", 157,
			"4800", 158,
			"7200", 159,
			"9600", 160,
			"19200", 161,
			0);
    mymenu2= new Menu("Parity", FALSE, 0, 1, FALSE);
    mymenu2->AppendItems("Even", 50,
			"Odd", 51,
			"Mark", 52,
			"Space", 53,
			0);
    mymenu3= new Menu("Stopbits", FALSE, 0, 1, FALSE);
    mymenu3->AppendItems("0", 50,
			"1", 51,
			"2", 52,
			"jehfgsdhfg", 162,
			0);
    mymenu3->AppendMenu(mymenu1);
    mymenu2->AppendMenu(mymenu3);
			
    mylist= new ObjList;
    TextItem *ti;
    for (int i= 1; i < 22; i++) {
	mylist->Add(ti= new TextItem((char*)0));
	ti->SetFString(FALSE, "Item %d", i);
    }
    
    collview= new CollectionView(0, mylist, eCVDontStuckToBorder);
    collview->SetMinExtent(Point(400,0));
    
    st= new StyledText(gSysFont, "An @BExample@B of a @IStyledText@I with several lines of text");
    tv= new TextView(this, Rectangle(0, 0, 200, cFit), st, eCenter);
}

VObject *myDialogView::DoCreateDialog()
{
    Form *fp= new Form(cIdNone, eVObjHCenter, 2, 
	"Reported By:",     new EditTextItem(cIdNone, "Your Name", 150),      
	"Date:",            new EditTextItem(cIdNone, "12.12.93", 80),
	"E-mail Address:",  new EditTextItem(cIdNone, "xxx@ifi.unizh.ch", 120),   
	"Version:",         new EditTextItem(cIdNone, "2.0 (Beta)", 60),
	0);
		
    return new BorderItem(
	new RadioButton,
	new Cluster(1000, eVObjVTop, 20,
	    new Cluster(1001, eVObjHLeft, 20,
		new ShadowItem(999, new ActionButton(3333, "abc")),
		new ShadowItem(999, new TextItem("abc")),
		new EditTextItem(1002, "123456789"),
		new EnumItem(1109, eVObjVCenter, new TextItem("abc")),
		new BorderItem("Baudrate",
		    new OneOfCluster(1003, (VObjAlign)(eVObjHLeft|eVObjVTop), Point(15, 5),
			"50", "110", "150", "300", "600", "1200", "2400",
			"3600", "4800", "7200", "9600", "19200", 0
		    )
		),
		new Cluster(1100, eVObjVBase, 10,
		    new TextItem("Find:"),
		    new BorderItem(new EditTextItem(1101, "3 Zeilen", 200, 3)),
		    0
		),
		new Cluster(1102, eVObjVBase, 10,
		    new TextItem("Size:"),
		    new ShadowItem(999, new TextItem("fgi")),
		    new BorderItem(new FloatItem(1103, 10.5, 0.0, 100.0)),
		    new TextItem("cm"),
		    0
		),
		new Cluster(1104, eVObjVBase, 0,
		    new Cluster(1105, eVObjHCenter, 0,
			new TextItem("over"),
			new TextItem("under"),
			0 
		    ),
		    new Button(999, new TextItem("button")),
		    new BorderItem(new TextItem("abc"), Point(0)),
		    new ShadowItem(999, new TextItem("shadow"), 0),
		    new BorderItem(new EditTextItem(1106, "abc"), Point(0)),
		    new EditTextItem(1107, "ghi"),
		    new BorderItem(
			new StaticTextView ((View*)0, Rectangle(Point(cFit)),
			    new CheapText((byte*)"abc"), eLeft),
			Point(0)
		    ),
		    new BorderItem(
			new TextItem("abc", new_Font(eFontHelvetica, 18, eFacePlain)),
			Point(0)
		    ),
		    0
		),
		new BorderItem (
		    new Cluster(1108, eVObjVBase, 10,
			new EnumItem(1109, eVObjVBase, new NumItem(558, 1, 0, 30)),
			new EnumItem(1110, eVObjVBase,
			    new CycleItem(1111, (VObjAlign)(eVObjHCenter|eVObjVBase),
				new TextItem("Jan"),
				new TextItem("Feb"),
				new TextItem("Mar"),
				new TextItem("Apr"),
				new TextItem("May"),
				new TextItem("Jun"),
				new TextItem("Jul"),
				new TextItem("Aug"),
				new TextItem("Sep"),
				new TextItem("Oct"),
				new TextItem("Nov"),
				new TextItem("Dec"),
				0
			    )
			),
			new EnumItem(1112, eVObjVBase,
			    new NumItem(1113, 1900, 1900, 2000)
			),
			0
		    )
		),
		new Cluster(1114, eVObjVBase, 10,
		    new ActionButton(cIdYes, "Yes", TRUE),
		    new ActionButton(cIdNo, "No"),
		    new ActionButton(cIdCancel, "Cancel"),
		    new ActionButton(1116, new RadioButton),
		    0
		),
		0
	    ),
	    new Cluster(1256, eVObjHLeft, 10,
		new PopupButton(1117, 50, mymenu1),
		new PopupItem(1117, 50, mymenu1),
		new PopupItem(1118, 50, mymenu2),
		new PopupItem(1119, 50, mymenu3),
		0
	    ),
	    new Cluster(1120, eVObjHLeft, 20,
		new StaticTextView((View*)0,Rectangle(Point(200,cFit)),
		    new CheapText((byte*)
			"To use help push the button over the item you want help for. Press the left mouse button now etc..."
		    ), eJustified
		),
		new BorderItem(
		  new BackgroundItem(ePatGrey12,
		    new EnumItem(1126, eVObjVCenter,
			new CycleItem(1127, (VObjAlign)(eVObjHCenter|eVObjVCenter),
			    new BorderItem(new TextItem("Start"), Point(0), 3),
			    new EnumItem(1128, eVObjVBase,
				new CycleItem(1129, (VObjAlign)(eVObjHCenter|eVObjVBase),
				    new EnumItem(1130, eVObjVBase,
					new NumItem(1131, 1, 0, 30)
				    ),
				    new EnumItem(1132, eVObjVBase,
					new ToggleButton
				    ),
				    0
				)
			    ),
			    new TextItem("rot"),
			    new TextItem("blau"),
			    new ToggleButton,
			    new TextItem("gruen"),
			    new BorderItem(new EditTextItem(1133, "text3")),
			    new TextItem("dunkelblau"),
			    new RadioButton,
			    new BorderItem(new TextItem("End"), Point(0)),
			    new LabeledButton(1134, "50"),
			    new BorderItem("Baudrate",
				new OneOfCluster(1135, (VObjAlign)(eVObjHLeft|eVObjVTop), Point(15, 5),
				    "50", "110", "150", "300", "600", "1200", "2400",
				    "3600", "4800", "7200", "9600", "19200", 0
				)
			    ),
			    new BorderItem(
				new BorderItem(new TextItem("Style"),gBorder, 2), 
				new OneOfCluster(1200, eVObjHLeft, 5,
				    new ActionButton(1201, "Plain"),
				    new ManyOfCluster(1202, eVObjHLeft, 5,
					"Bold",
					"Italic",
					"Underline", 
					"Shadow",
					"Outline",
					0
				    ),
				    0
				)
			    ),
			    0
			)
		    )
		  )
		),
		new BorderItem (
		    new EnumItem(1302, eVObjVBase,
			new EditTextItem(1303, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10",
									200, 2))
		),
		new BorderItem("Form", fp),
		0
	    ),
	    new Cluster(1304, eVObjHCenter, 10,
		new BorderItem(new EditTextItem(1305, "text1")),
		new BorderItem(new EditTextItem(1306, "text2")),
		new BorderItem("Text", new EditTextItem(1307, "text3")),
		new Scroller(collview, Point(100), 1308),
		//new Scroller(collview, Point(100), 1309),
		new Scroller(tv, Point(100), 1309),
		0
	    ),
	    new BorderItem(new BorderItem(new TextItem("Style")), 
		new OneOfCluster(1313, eVObjHLeft, 5,
		    new ActionButton(1314, "Plain"),
		    new ManyOfCluster(1315, eVObjHLeft, 5,
			"Bold", "Italic", "Underline", "Shadow", "Outline", 0
		    ),
		    0
		)
	    ),
	    0
	),
	gBorder,
	2
    );
}

//---- myDocument --------------------------------------------------------------

class myDocument : public Document {
    View *view;
public:
    myDocument()
	{ }
    ~myDocument()
	{ SafeDelete(view); }
    Window *DoMakeWindows(); 
};

Window *myDocument::DoMakeWindows()
{
    view= new myDialogView(this);
    return new Window(this, Point(500, 250), eWinDefault, new Clipper(view));
}

//---- dialog ------------------------------------------------------------------

class dialog: public Application { 
public:
    dialog(int argc, char **argv) : Application(argc, argv)
	{ ApplInit(); }
    Document *DoMakeDocuments(const char *)
	{ return new myDocument; }
};

//---- main ---------------------------------------------------------------------

main(int argc, char **argv)
{
    dialog adialog(argc, argv);
    
    return adialog.Run();
}
