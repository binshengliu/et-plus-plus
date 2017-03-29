#include "X11.h"

#include "X11/Xatom.h"
#include "X11/keysym.h"
#include "X11/cursorfont.h"

#include "XWindowPort.h"
#include "XWinSystem.h"
#include "XFont.h"
#include "XBitmap.h"
#include "String.h"
#include "EvtHandler.h"

extern int lasttime;
extern char *gProgname;
extern bool fullscreen;
extern int gArgc;
extern char **gArgv;

static bool allow;
static int buttons;
static Atom WMProtocolsAtoms[3];
static XWindow mainwindow;

const u_long dfltMsk= StructureNotifyMask | ExposureMask | VisibilityChangeMask |
		     KeyPressMask |
		     ButtonPressMask | ButtonReleaseMask |
		     EnterWindowMask | LeaveWindowMask |
		     PointerMotionMask | OwnerGrabButtonMask |
		     VisibilityChangeMask | FocusChangeMask;

const u_long dfltMsk2= dfltMsk | PointerMotionHintMask;
const u_long dfltMsk3= ButtonPressMask | ButtonReleaseMask |
		      PointerMotionMask | PointerMotionHintMask;
const bool ownerevents= XFalse;

void Allow()
{
    if (allow) {
	allow= FALSE;
	XAllowEvents(display, AsyncPointer, lasttime);
    }
}

static Pixmap MakePixmap(Pixmap &pm, Point &oldsize, Point newsize)
{
    if (pm == None || newsize.x > oldsize.x || newsize.y > oldsize.y) {
	if (pm)
	    XFreePixmap(display, pm);
	oldsize= newsize;
	pm= XCreatePixmap(display, rootwin, oldsize.x, oldsize.y,
					XDefaultDepth(display, screen));
    }
    return pm;
}

static XCursor UseCursor(GrCursor c);

//---- window creation/destruction ---------------------------------------------

MetaImpl0(XWindowPort);

XWindowPort::XWindowPort(InpHandlerFun nf, void *priv1, bool ov, bool bl, bool wm)
					: WindowPort(nf, priv1, ov, bl, wm)
{
    XSetWindowAttributes setWAttr;
    
    isvisible= ismapped= FALSE;

    setWAttr.background_pixmap= None;
    setWAttr.border_pixmap= None;
    setWAttr.cursor= UseCursor(cursor);
    setWAttr.override_redirect= (XBool) wm;
    setWAttr.save_under= (XBool) ov;
    if (ov && bl)
	setWAttr.override_redirect= XTrue;
    dontgrab= ! (bool) setWAttr.override_redirect;

    winid= XCreateWindow(display, XRootWindow(display, screen),
	    0, 0, 1, 1, 0, XDefaultDepth(display, screen), InputOutput,
		    (Visual*) CopyFromParent, 
		CWBackPixmap | CWBorderPixmap | CWCursor | CWOverrideRedirect
						    | CWSaveUnder, &setWAttr);
    if (mainwindow == 0)
	mainwindow= winid;
    if (! ov)
	DevSetTitle(gProgname);

    XSelectInput(display, winid, dfltMsk);
    id= winid;
    gc= XCreateGC(display, winid, 0, NULL);
    cmap= XDefaultColormap(display, screen);    

    if (! ov) {
	if (WMProtocolsAtoms[0] == 0) {
	    WMProtocolsAtoms[0]= XInternAtom(display, "WM_TAKE_FOCUS", XFalse);
	    WMProtocolsAtoms[1]= XInternAtom(display, "WM_SAVE_YOURSELF", XFalse);
	    WMProtocolsAtoms[2]= XInternAtom(display, "WM_DELETE_WINDOW", XFalse);
	}
	XSetWMProtocols(display, winid, WMProtocolsAtoms, 3);

	XWMHints *wmhints= XAllocWMHints();
	wmhints->input= XTrue;
	wmhints->initial_state= NormalState;
	wmhints->flags= InputHint | StateHint;
	if (winid != mainwindow) {
	    wmhints->window_group= mainwindow;
	    wmhints->flags= WindowGroupHint;
	}
	
	XClassHint *ch= XAllocClassHint();
	ch->res_name= gProgname;
	ch->res_class= "ET++";
	XSetWMProperties(display, winid, 0, 0, gArgv, gArgc, 0, wmhints, ch);
	XFree((char*) wmhints);
	XFree((char*) ch);
    }
    if (ov || bl)
	XSetTransientForHint(display, winid, mainwindow);
}

XWindowPort::~XWindowPort()
{
    XFreeColormap(display, cmap);
    XFreeGC(display, gc);
    XDestroyWindow(display, winid);
}

//---- Patterns & Rasterops ----------------------------------------------------

void XWindowPort::DevSetPattern(struct DevBitmap *bm)
{
    solid= FALSE;
    XSetStipple(display, gc, ((XBitmap*)bm)->GetPixmap());
    XSetPlaneMask(display, gc, (u_long) AllPlanes);
    XSetFunction(display, gc, GXcopy);
    XSetForeground(display, gc, XBlackPixel(display, screen));
    XSetBackground(display, gc, XWhitePixel(display, screen));
    XSetFillStyle(display, gc, FillOpaqueStippled);
}

bool XWindowPort::DevSetColor(RGBColor *cp)
{
    if (cp->GetId() < 0 || cp->GetPrec() == MaxWord) {
	XColor xc;
	xc.red= cp->GetRed() << 8;
	xc.green= cp->GetGreen() << 8;
	xc.blue= cp->GetBlue() << 8;
	xc.flags= DoRed | DoGreen | DoBlue;
	if (cp->GetId() < 0) {
	    if (cp->GetPrec() == MaxWord) { // read/write cell
		u_long planemask[1], pixels[1];
		if (XAllocColorCells(display, cmap, XFalse, planemask, 0, pixels, 1) == 0) {
		    cerr << "new cmap\n";
		    cmap= XCreateColormap(display, winid, XDefaultVisual(display, screen), AllocNone);
		    // cmap= XCopyColormapAndFree(display, cmap);
		    XSetWindowColormap(display, winid, cmap);
		    if (XAllocColorCells(display, cmap, XFalse, planemask, 0, pixels, 1) == 0) {
			cerr << "can't get new color\n";
		    }
		}
		cp->SetId(pixels[0]);
	    } else {                        // readonly cell
		XAllocColor(display, cmap, &xc);
		cp->SetId(xc.pixel);
	    }
	}
	if (cp->GetPrec() == MaxWord) {
	    xc.pixel= cp->GetId();
	    XStoreColor(display, cmap, &xc);
	}
    }
    solid= TRUE;
    XSetFillStyle(display, gc, FillSolid);
    XSetPlaneMask(display, gc, AllPlanes);
    XSetFunction(display, gc, GXcopy);
    XSetForeground(display, gc, cp->GetId());
    return FALSE;
}

void XWindowPort::DevSetOther(int pid)
{
    solid= TRUE;
    XSetFillStyle(display, gc, FillSolid);
    switch (pid) {
    case -1:
	XSetPlaneMask(display, gc,
		XWhitePixel(display, screen) ^ XBlackPixel(display, screen));
	XSetForeground(display, gc, XBlackPixel(display, screen));
	XSetFunction(display, gc, GXinvert);
	break;
    case 1:
    case 2:
	XSetPlaneMask(display, gc, AllPlanes);
	XSetFunction(display, gc, GXcopy);
	if (pid == 1)
	    XSetForeground(display, gc, XWhitePixel(display, screen));
	else
	    XSetForeground(display, gc, XBlackPixel(display, screen));
	break;
    }
}

void XWindowPort::SetLine(int psz)
{
    if (psz <= 0 && !solid)
	psz= 1;
	/*
    if (psz == 1 && solid)
	psz= 0;
	*/
    XSetLineAttributes(display, gc, psz, LineSolid, CapButt, JoinMiter);
}

//---- Cursors -----------------------------------------------------------------

const int cMaxCursors= 30;
static XCursor CurMap[cMaxCursors];

static XCursor UseCursor(GrCursor c)
{
    if (CurMap[c] == 0) {
	unsigned int xc;
	
	switch (c) {
	case eCrsNone:          xc= XC_xterm; break;
	case eCrsCross:         xc= XC_crosshair; break;
	case eCrsBoldCross:     xc= XC_cross; break;
	case eCrsUpArrow:       xc= XC_sb_up_arrow; break;
	case eCrsDownArrow:     xc= XC_sb_down_arrow; break;
	case eCrsLeftArrow:     xc= XC_sb_left_arrow; break;
	case eCrsRightArrow:    xc= XC_sb_right_arrow; break;
	case eCrsHourglass:     xc= XC_watch; break;
	case eCrsPen:           xc= XC_pencil; break;
	case eCrsMoveHand:      xc= XC_hand2; break;
	case eCrsHand:          xc= XC_hand2; break;
	case eCrsCrossHair:     xc= XC_crosshair; break;
	case eCrsMoveStretch:   xc= XC_hand2; break;
	case eCrsIBeam:         xc= XC_xterm; break;
	case eCrsLeftRightArrow: xc= XC_sb_h_double_arrow; break;
	case eCrsUpDownArrow:   xc= XC_sb_v_double_arrow; break;
	default:
	    xc= XC_left_ptr;
	    break;
	}
	CurMap[c]= XCreateFontCursor(display, xc);
    }
    return CurMap[c];
}

void XWindowPort::DevSetCursor(GrCursor c)
{
    XDefineCursor(display, winid, UseCursor(c));
}

//---- Event mapping -----------------------------------------------------------

static void StateToFlags(Token *t, unsigned int state)
{
    if (state & ShiftMask)
	t->Flags|= eFlgShiftKey;
    if (state & ControlMask)
	t->Flags|= eFlgCntlKey;
    if (state & Mod1Mask)
	t->Flags|= eFlgMetaKey;
}

static bool active, focus;

bool XWindowPort::MapEvent(XEvent &xe, Token *tp)
{
    Rectangle newrect;
    Token t(eEvtNone);
    char buffer[40];
    int ccnt, bufsize= 40;
    KeySym _keysym;
    XComposeStatus compose;
    Rectangle windowRect;
    int x, y, keysym;
    XWindow child;

    switch (xe.type) {
    case NoExpose:
	return FALSE;
	
    case VisibilityNotify:
	isvisible= (xe.xvisibility.state == VisibilityUnobscured);
	break;

    case GraphicsExpose:
    case Expose:
	newrect= Rectangle(xe.xexpose.x, xe.xexpose.y, xe.xexpose.width,
								xe.xexpose.height);
	if (xe.xexpose.count > 0) {
	    if (ismapped)
		Damage(eFlgDamage3, &newrect);
	} else {
	    if (ismapped)
		Damage(eFlgDamage2, &newrect);  // last exposure in sequence
	    else {
		ismapped= TRUE;
		Damage(eFlgDamage1, &rect);
	    }
	}
	return FALSE;

    case ConfigureNotify:
	XTranslateCoordinates(display, winid, rootwin, 0, 0, &x, &y, &child);
	rect= Rectangle(x, y, xe.xconfigure.width, xe.xconfigure.height);
	if (ismapped)
	    Damage(eFlgDamage1, &rect);
	return FALSE;
    
    case MapNotify:
	return FALSE;
    
    case UnmapNotify:
	isvisible= ismapped= FALSE;
	return FALSE;
    
    case KeyPress:
	ccnt= XLookupString(&xe.xkey, buffer, bufsize, &_keysym, &compose);
	keysym= (int) _keysym;
	
	if (keysym >= XK_KP_0 && keysym < XK_KP_9)
	    keysym-= (XK_KP_0 - XK_KP_9);
	
	switch ((int)keysym) {
	case XK_KP_Space:   t.Code= ' ';            break;
	case XK_KP_Tab:     t.Code= '\t';           break;
	case XK_KP_Enter:   t.Code= '\n';           break;
	case XK_KP_Equal:   t.Code= '=';            break;
	case XK_KP_Multiply:t.Code= '*';            break;
	case XK_KP_Add:     t.Code= '+';            break;
	case XK_KP_Separator:t.Code= ',';           break;
	case XK_KP_Subtract:t.Code= '-';            break;
	case XK_KP_Decimal: t.Code= '.';            break;
	case XK_KP_Divide:  t.Code= '/';            break;

	case XK_Left:       t.Code= eEvtCursorLeft; break;
	case XK_Up:         t.Code= eEvtCursorUp;   break;
	case XK_Right:      t.Code= eEvtCursorRight;break;
	case XK_Down:       t.Code= eEvtCursorDown; break;
	case XK_Tab:        t.Code= '\t';           break;
	case XK_BackSpace:  t.Code= '\b';           break;
	case XK_Linefeed:   t.Code= '\n';           break;
	case XK_Return:     t.Code= '\r';           break;

	default:
	    if (keysym > 0 && keysym < 256)
		t.Code= keysym;
	    else if (keysym >= XK_F1 && keysym <= XK_F35)
		t.Code= (short) (eEvtPfkFirst + (keysym - XK_F1));
	    else
		return FALSE;
	    break; 
	}
	StateToFlags(&t, xe.xkey.state);
	if (t.Flags & eFlgCntlKey) {
	    if (t.Flags & eFlgShiftKey)
		t.Code-= 0x40;
	    else
		t.Code-= 0x60;
	}
	if (t.Flags & eFlgMetaKey)
	    t.Code+= 0x80;
	t.Pos= Point(xe.xkey.x, xe.xkey.y);
	t.At= xe.xkey.time;
	break;
	
    case ButtonPress:
    case ButtonRelease:
	t.Code= eEvtButtons + xe.xbutton.button - 1;
	StateToFlags(&t, xe.xbutton.state);
	t.At= xe.xbutton.time;
	t.Pos= Point(xe.xbutton.x, xe.xbutton.y);
	
	if (xe.type == ButtonPress) {
	    if (buttons == 0) {
		allow= TRUE;
		XGrabPointer(display, winid, ownerevents, (u_int)dfltMsk3,
		    GrabModeSync, GrabModeAsync, None, None, xe.xbutton.time);
	    }
	    buttons|= (1 << xe.xbutton.button-1);
	}
	if (xe.type == ButtonRelease) {
	    t.Flags|= eFlgButDown;
	    buttons&= ~(1 << xe.xbutton.button-1);
	    if (buttons == 0) {
		allow= FALSE;
		XUngrabPointer(display, xe.xbutton.time);
		XFlush(display);
	    }
	}
	break;
	
    case MotionNotify:
	if (xe.xmotion.is_hint) {
	    while(XCheckTypedEvent(display, MotionNotify, &xe))
		;   // overread MotionNotifies
	    t.Pos= getMousePos(xe.xmotion.state);
	    if (t.Pos == lastpos)
		return FALSE;
	} else
	    t.Pos= Point(xe.xmotion.x, xe.xmotion.y);
	t.Code= eEvtLocMove;
	t.At= xe.xmotion.time;
	StateToFlags(&t, xe.xmotion.state);
	break;
	
    case LeaveNotify:
    case EnterNotify:
	if (xe.type == EnterNotify) {
	    t.Code= eEvtEnter;
	    active= xe.xcrossing.focus ? TRUE : FALSE;
	} else {
	    t.Code= eEvtExit;
	    focus= active;
	}
	t.At= xe.xcrossing.time;
	t.Pos= Point(xe.xcrossing.x, xe.xcrossing.y);
	if (cmap != XDefaultColormap(display, screen)) {
	    if (xe.type == EnterNotify)
		XInstallColormap(display, cmap);
	    else
		XUninstallColormap(display, cmap);
	}
	if (gDebug)
	    cerr << (focus ? "in focus/" : "out of focus/") << (active ? "active\n" : "not active\n");
	break;

    case FocusIn:
	focus= active= TRUE;
	if (gDebug)
	    cerr << (focus ? "in focus/" : "out of focus/") << (active ? "active\n" : "not active\n");
	return FALSE;

    case FocusOut:
	focus= active= FALSE;
	if (gDebug)
	    cerr << (focus ? "in focus/" : "out of focus/") << (active ? "active\n" : "not active\n");
	return FALSE;

    case ClientMessage:
	if (WMProtocolsAtoms[0] == xe.xclient.data.l[0]) {        // WM_TAKE_FOCUS
	    if (gDebug)
		cerr << "WM_TAKE_FOCUS\n";
	    XSetInputFocus(display, winid, RevertToParent, lasttime);
	} else if (WMProtocolsAtoms[1] == xe.xclient.data.l[0]) { // WM_SAVE_YOURSELF
	    if (gDebug)
		cerr << "WM_SAVE_YOURSELF\n";
	    XSetCommand(display, mainwindow, gArgv, gArgc);
	} else if (WMProtocolsAtoms[2] == xe.xclient.data.l[0]) { // WM_DELETE_WINDOW
	    if (gDebug)
		cerr << "WM_DELETE_WINDOW\n";
	    ((EvtHandler*)privdata)->Control(555, 555, 0);
	}
	return FALSE;

    default:
	break;
    }

    if (tp)
	*tp= t;
    return TRUE;
}

//---- window management -------------------------------------------------------

void XWindowPort::DevSetTitle(char *name)
{
    XTextProperty tp;
    XStringListToTextProperty(&name, 1, &tp);
    if (isicon)
	XSetWMIconName(display, winid, &tp);
    else
	XSetWMName(display, winid, &tp);
}

void XWindowPort::DevShow(WindowPort *fp, Rectangle r)
{
    XEvent ev;
    
    father= (XWindowPort*) fp;
    
    if (dontgrab)
	Allow();

    if (father == 0) {
	XSizeHints *sizehints= XAllocSizeHints();
	sizehints->flags = PPosition | PSize | PMinSize;
	sizehints->x = r.origin.x;
	sizehints->y = r.origin.y;
	sizehints->width = r.extent.x;
	sizehints->height = r.extent.y;
	sizehints->min_width = r.extent.x;
	sizehints->min_height = r.extent.y;
	XSetNormalHints(display, winid, sizehints);
	// XSetWMSizeHints(display, winid, &sizehints);
	XFree((char*) sizehints);
    }
    
    XMoveResizeWindow(display, winid, r.origin.x, r.origin.y, r.extent.x, r.extent.y);
    XMapRaised(display, winid);
    
    if (buttons)
	XGrabPointer(display, winid, ownerevents, (u_int)dfltMsk3,
	   GrabModeAsync, GrabModeAsync, None, None, lasttime);

    do {
	XWindowEvent(display, winid, dfltMsk, &ev);
	MapEvent(ev);
    } while (! ismapped);
}

void XWindowPort::DevHide()
{     
    XEvent ev;
    
    XUnmapWindow(display, winid);
    
    do {
	XWindowEvent(display, winid, dfltMsk, &ev);
	MapEvent(ev);
    } while (ismapped);

    if (buttons && father)
	XGrabPointer(display, father->winid, ownerevents, (u_int)dfltMsk3,
	    GrabModeAsync, GrabModeAsync, None, None, lasttime);
    allow= FALSE;
}

void XWindowPort::DevTop(bool top)
{
    XEvent ev;
    
    Allow();
    if (top) {
	if (! isvisible) {
	    XRaiseWindow(display, winid);
	    do {
		XWindowEvent(display, winid, ExposureMask, &ev);
		MapEvent(ev);
	    } while (ev.type != Expose || ev.xexpose.count > 0);
	}
    } else
	XLowerWindow(display, winid);
    // XWindowEvent(display, winid, StructureNotifyMask, &ev);
}

void XWindowPort::DevSetExtent(Point e)
{
    XResizeWindow(display, winid, e.x, e.y);
}

Rectangle XWindowPort::DevGetRect()
{
    return rect;
}

void XWindowPort::DevSetOrigin(Point o)
{
    if (rect.origin != o)
	XMoveWindow(display, winid, o.x, o.y);
}

void XWindowPort::DevBell(long d)
{
    static long lastd= 0;

    if (d != lastd) {
	XKeyboardControl kb_ctrl;
	lastd= d;
	kb_ctrl.bell_duration= (int) d;
	XChangeKeyboardControl(display, KBBellDuration, &kb_ctrl);
    }
    XBell(display, 0);
}

//---- graphical primitives ----------------------------------------------------

void XWindowPort::DevClip(Rectangle r, Point p)
{
    if (fullscreen) {
	XSetClipMask(display, gc, None);
	XSetTSOrigin(display, gc, 0, 0);
    } else {
	XSetClipRectangles(display, gc, 0, 0, (XRectangle*)&r, 1, YXBanded);
	XSetTSOrigin(display, gc, p.x, p.y);
    }
} 

void XWindowPort::DevStrokeLine2(int psz, Rectangle*, GrLineCap, Point start, Point end)
{
    SetLine(psz);
    XDrawLine(display, id, gc, start.x, start.y, end.x, end.y);
}

static void AdjustRect(int psz, Rectangle *r)
{
    if (psz > 1) {
	r->origin+= psz/2;
	r->extent-= psz;
    } else
	r->extent-= 1;
}

void XWindowPort::DevStrokeRect2(int psz, Rectangle *r)
{
    SetLine(psz);
    AdjustRect(psz, r);
    XDrawRectangle(display, id, gc, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void XWindowPort::DevFillRect(Rectangle *r)
{
    XFillRectangle(display, id, gc, r->origin.x, r->origin.y, r->extent.x, r->extent.y);
}

void XWindowPort::DevStrokeRRect2(int psz, Rectangle *r, Point dia)
{
    XArc a[4];
    XSegment s[4];
    
    AdjustRect(psz, r);
    Point o= r->origin;
    Point e= r->extent-dia;
    
    for (int i= 0; i < 4; i++) {
	a[i].angle1= (i*90) << 6;
	a[i].angle2= 90 << 6;
	a[i].width= dia.x;
	a[i].height= dia.y;
	a[i].x= o.x;
	a[i].y= o.y;
    }
    a[0].x+= e.x;
    a[3].x+= e.x;
    a[2].y+= e.y;
    a[3].y+= e.y;
    
    s[0].x1= s[3].x1= o.x+dia.x/2;
    s[0].x2= s[3].x2= o.x+r->extent.x-dia.x/2;
    s[1].x1= s[1].x2= o.x;
    s[2].x1= s[2].x2= o.x+r->extent.x;
    
    s[0].y1= s[0].y2= o.y;
    s[1].y1= s[2].y1= o.y+dia.y/2;
    s[1].y2= s[2].y2= o.y+r->extent.y-dia.y/2;
    s[3].y1= s[3].y2= o.y+r->extent.y;
    
    SetLine(psz);
    XDrawArcs(display, id, gc, a, 4);
    XDrawSegments(display, id, gc, s, 4);
}

void XWindowPort::DevFillRRect2(Rectangle *r, Point dia)
{
    XArc a[4];
    XRectangle rr[3];
    Point o= r->origin;
    Point e= r->extent-dia;
    
    for (int i= 0; i < 4; i++) {
	a[i].angle1= (i*90) << 6;
	a[i].angle2= 90 << 6;
	a[i].width= dia.x;
	a[i].height= dia.y;
	a[i].x= o.x;
	a[i].y= o.y;
    }
    a[0].x+= e.x;
    a[3].x+= e.x;
    a[2].y+= e.y;
    a[3].y+= e.y;
    
    rr[0].x= rr[2].x= o.x + dia.x/2;
    rr[1].x= o.x;
    
    rr[0].y= o.y;
    rr[1].y= o.y+dia.y/2;
    rr[2].y= o.y+r->extent.y-dia.y/2;
    
    rr[0].width= rr[2].width= e.x;
    rr[1].width= r->extent.x;
    
    rr[0].height= rr[2].height= dia.y/2;
    rr[1].height= e.y;
    
    XFillArcs(display, id, gc, a, 4);
    XFillRectangles(display, id, gc, rr, 3);
}

void XWindowPort::DevStrokeOval2(int psz, Rectangle *r)
{
    SetLine(psz);
    AdjustRect(psz, r);
    XDrawArc(display, id, gc, r->origin.x, r->origin.y,
				r->extent.x, r->extent.y, 0, 360*64);
}

void XWindowPort::DevFillOval2(Rectangle *r)
{
    XFillArc(display, id, gc, r->origin.x, r->origin.y,
					r->extent.x, r->extent.y, 0, 360*64);
}

void XWindowPort::DevStrokeWedge2(int psz, GrLineCap, Rectangle *r, int s, int len)
{
    SetLine(psz);
    AdjustRect(psz, r);
    XDrawArc(display, id, gc, r->origin.x, r->origin.y,
		r->extent.x, r->extent.y, ((450-s) % 360)*64, -len*64);
}

void XWindowPort::DevFillWedge2(Rectangle *r, int s, int len)
{
    XFillArc(display, id, gc, r->origin.x, r->origin.y,
			r->extent.x, r->extent.y, ((450-s) % 360)*64, -len*64);
}

void XWindowPort::DevStrokePolygon2(Rectangle *r,
			Point *pts, int npts, GrPolyType, int psz, GrLineCap)
{
    register int i;
    Point *xp= (Point*) alloca(npts * sizeof(Point));
    
    SetLine(psz);
    Point at= r->origin;
    for (i= 0; i < npts; i++)
	xp[i]= pts[i]+at;
    XDrawLines(display, id, gc, (XPoint*) xp, npts, CoordModeOrigin);
}

void XWindowPort::DevFillPolygon2(Rectangle *r, Point *pts, int npts, GrPolyType)
{
    register int i;
    Point *xp= (Point*) alloca(npts * sizeof(Point));
    
    Point at= r->origin;
    for (i= 0; i < npts; i++)
	xp[i]= pts[i]+at;
    XFillPolygon(display, id, gc, (XPoint*) xp, npts,
				(npts <= 3) ? Convex : Complex, CoordModeOrigin);
}

void XWindowPort::DevShowBitmap(Rectangle *r, Bitmap *bm)
{
    XBitmap *xbm= (XBitmap*) bm->GetDevBitmap();
    if (xbm->Depth() > 1) {
	XCopyArea(display, xbm->GetPixmap(), id, gc,
		0, 0, r->extent.x, r->extent.y, r->origin.x, r->origin.y);
	
    } else {
	XSetStipple(display, gc, xbm->GetPixmap());
	XSetTSOrigin(display, gc, r->origin.x, r->origin.y);
	XSetFillStyle(display, gc, FillStippled);
	XFillRectangle(display, id, gc, r->origin.x, r->origin.y,
						r->extent.x, r->extent.y);
    }
}

//---- text batching -----------------------------------------------------------

static XTextItem ti[100], *tip= ti;
static byte tbc[MaxTextBatchCnt], *tbp= tbc;
static Font *lastfont;

bool XWindowPort::DevShowChar(FontPtr fdp, Point delta, byte c, bool isnew, Point)
{
    if (delta.y || tip >= &ti[100-1])   // X cannot handle multiple lines !!
	return TRUE;
    if (isnew) {                // first
	tip= ti;
	tip->delta= 0;
	tbp= tbc;
	tip->chars= (char*)tbp;
	tip->nchars= 0;
	tip->font= ((XServerFont*)fdp)->GetId();
	lastfont= fdp;
    } else if (fdp != lastfont || delta.x) {   // switch to next
	tip++;
	tip->delta= delta.x;
	tip->chars= (char*)tbp;
	tip->nchars= 0;
	if (fdp != lastfont)
	    tip->font= ((XServerFont*)fdp)->GetId();
	else
	    tip->font= (XFont) None;
	lastfont= fdp;
    }
    
    *tbp++= c;
    tip->nchars++;
    return FALSE;
}

void XWindowPort::DevShowTextBatch(Rectangle*, Point pos)
{
    XDrawText(display, id, gc, pos.x, pos.y, ti, (tip-ti)+1);
} 

//---- scrolling ---------------------------------------------------------------

void XWindowPort::DevScrollRect(Rectangle r, Point delta)
{
    XEvent ev;
    
    DevSetOther(2);
    XSetGraphicsExposures(display, gc, XTrue);
    XCopyArea(display, id, id, gc, r.origin.x - delta.x, r.origin.y - delta.y,
			      r.extent.x, r.extent.y, r.origin.x, r.origin.y);
    do {
	XWindowEvent(display, winid, ExposureMask, &ev);
	MapEvent(ev);
    } while (ev.type == GraphicsExpose && ev.xgraphicsexpose.count > 0);
    XSetGraphicsExposures(display, gc, XFalse);
}

//---- input -------------------------------------------------------------------

void XWindowPort::DevGetEvent(Token *t, int timeout, bool)
{   
    XEvent ev;

    Allow();
    do {
	if (timeout < 0)
	     XWindowEvent(display, winid, dfltMsk2, &ev);
	else {
	if (! XCheckWindowEvent(display, winid, dfltMsk2, &ev)) {
	    if (gSystem->CanRead(display->fd, timeout)) {       // timeout
		t->Code= eEvtNone;
		t->Flags= 0;
		return;
	    }
	    lasttime+= timeout;
	    timeout= 0;
	    XWindowEvent(display, winid, dfltMsk2, &ev);
	}
	}
    } while (! MapEvent(ev, t));
}

Point XWindowPort::getMousePos(unsigned int &state)
{
    XWindow rootW, childW;
    int x0, y0, wx, wy;
    
    XQueryPointer(display, winid, &rootW, &childW, &x0, &y0, &wx, &wy, &state);
    return Point(wx, wy);
}

void XWindowPort::DevSetMousePos(Point p, bool m)
{
    XWarpPointer(display, None, m ? (XWindow)None : winid, 0, 0, 0, 0, p.x, p.y);
}

void XWindowPort::DevGrab(bool mode, bool fs)
{
    if (fs) {
	Allow();
	if (mode) {
	    id= rootwin;
	    origin+= rect.origin;
	    XGrabServer(display);
	    XSetSubwindowMode(display, gc, IncludeInferiors);
	    XSetClipMask(display, gc, None);
	} else {
	    origin-= rect.origin;
	    XUndefineCursor(display, id);  // restore root cursor
	    id= winid;
	    XSetSubwindowMode(display, gc, ClipByChildren);
	    XUngrabServer(display);
	    XFlush(display);
	}
    }
}

//---- double buffering --------------------------------------------------------

static Point batchSize;
static Rectangle batchRect; 
static bool inBatch;

void XWindowPort::DevGiveHint(int code, int, void *vp)
{
    switch (code) {    
    case eHintBatch:
	if (overlay || (id != winid))
	    return;
	inBatch= TRUE;
	batchRect= *((Rectangle*)vp);
	id= MakePixmap(batchPixmap, batchSize, batchRect.origin+batchRect.extent);
	break;
	
    case eHintUnbatch:
	if (!inBatch)
	    return;
	inBatch= FALSE;
	id= winid;
	DevSetOther(2);
	XCopyArea(display, batchPixmap, id, gc,
	    batchRect.origin.x, batchRect.origin.y,
		batchRect.extent.x, batchRect.extent.y,
		    batchRect.origin.x, batchRect.origin.y);
	break;

    case eHintFlush:
	XFlush(display);
	break;
    }
}

void XWindowPort::DevImageCacheEnd2(DevBitmap *b, Rectangle *r)
{
    XBitmap *xbm= (XBitmap*) b;
    MakePixmap(xbm->pm, xbm->size, r->extent);
    DevSetOther(2);
    XCopyArea(display, id, xbm->pm, gc, r->origin.x, r->origin.y,
					r->extent.x, r->extent.y, 0, 0);
}

void XWindowPort::DevImageCacheCopy2(DevBitmap *b, Rectangle *r, Point p)
{
    DevSetOther(2);
    XCopyArea(display, ((XBitmap*)b)->pm, id, gc, p.x, p.y,
			r->extent.x, r->extent.y, r->origin.x, r->origin.y);
}
