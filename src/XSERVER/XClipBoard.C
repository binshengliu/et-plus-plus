#include "X11.h"

#include "X11/Xatom.h"

#include "XClipBoard.h"
#include "XWinSystem.h"
#include "IO/membuf.h"
#include "String.h"
#include "Error.h"
#include "View.h"
#include "FileType.h"

char *cEtProperty= "_ET",
     *cEtType    = "_ET_OIO";   // selection is in object input/output format

//---- XClipBoard ------------------------------------------------------------

XClipBoard::XClipBoard()
{
    awin= XCreateWindow(display, XRootWindow(display, screen),
	    0, 0, 1, 1, 0, XDefaultDepth(display, screen), InputOutput,
					    (Visual*) CopyFromParent, 0, 0);
    XSelectInput(display, awin, PropertyChangeMask);
    etprop= XInternAtom(display, cEtProperty, XFalse);
    etformat= XInternAtom(display, cEtType, XFalse);
}

XClipBoard::~XClipBoard()
{
    XDestroyWindow(display, awin);
}

bool XClipBoard::ProcessEvent(XEvent &xe)
{
    if (xe.xany.window != awin)
	return TRUE;

    switch (xe.type) {
    case SelectionClear:
	if (xe.xselectionclear.selection == XA_PRIMARY) {
	    XDeleteProperty(display, awin, etprop);
	    char *t= (char*) GetType();
	    cerr << "NotOwner(" << t << ")\n";
	    NotOwner(t);
	}
	break;

    case SelectionRequest:
	SendClipBoard((XSelectionRequestEvent*) &xe);
	break;
	
    default:
	break;
    }
    return FALSE;
}

char *XClipBoard::DevGetType()
{
    XWindow selwin= XGetSelectionOwner(display, XA_PRIMARY);
    if (selwin && selwin != awin) {
	Atom actual_type_return;
	int actual_format_return;
	u_char *prop_return;
	u_long nitems_return, bytes_after_return;
	char *t;
	XGetWindowProperty(display, selwin, etprop, 0L, 0L, XFalse,
		etformat, &actual_type_return, &actual_format_return,
			&nitems_return, &bytes_after_return, &prop_return);
	if (actual_type_return == None && actual_format_return == 0)
	    t= "None";
	else
	    t= (char*) cDocTypeET;
	return t;
    }
    return ClipBoard::DevGetType();
}

long XClipBoard::GetCurrentServerTime()
{
    XEvent ev;
    XChangeProperty(display, awin, XA_WM_NAME, XA_STRING, 8, PropModeAppend, (byte*) "", 0);
    XWindowEvent(display, awin, PropertyChangeMask, &ev);
    return ((XPropertyEvent*)&ev)->time;
}

void XClipBoard::ScratchChanged(char*)
{
    XSetSelectionOwner(display, XA_PRIMARY, awin, GetCurrentServerTime());
    if (XGetSelectionOwner(display, XA_PRIMARY) != awin)
	cerr << "can't get selection\n";
    XChangeProperty(display, awin, etprop, etformat, 8, PropModeReplace,
								(byte*) "", 0);
}

static XBool IsSelectionNotify(XDisplay*, XEvent *xe, char*)
{
    return (xe->type == SelectionNotify);
}

membuf *XClipBoard::MakeBuf(char *type)
{
    membuf *mb= 0;
    XEvent ev;
    XSelectionEvent *se;
    Atom actual_type_return, wishtype;
    int actual_format_return;
    u_char *prop_return;
    u_long nitems_return, bytes_after_return;
    
    if (type == cDocTypeET)
	wishtype= etformat;
    else
	wishtype= XA_STRING;
	
    XConvertSelection(display, XA_PRIMARY, wishtype, None, awin, CurrentTime);
    XIfEvent(display, &ev, IsSelectionNotify, 0);
    se= (XSelectionEvent*) &ev;
    
    if (se->property == None) {
	cerr << "can't convert selection\n";
	return mb;
    }
    u_long len;
    mb= new membuf(0);
    
    XGetWindowProperty(display, awin, se->property, 0L, 0L, XFalse,
	    AnyPropertyType, &actual_type_return, &actual_format_return,
		&nitems_return, &bytes_after_return, &prop_return);
	    
    len= bytes_after_return;
    
    XGetWindowProperty(display, awin, se->property, 0L, (len-1)/4+1, XTrue,
	    AnyPropertyType, &actual_type_return, &actual_format_return,
		&nitems_return, &bytes_after_return, &prop_return);
    
    mb->setbuf((char*)strsave((char*)prop_return, (int)(len+1)), (int)(len+1));
    XFree((char*) prop_return);
    mb->seek(len, TRUE);

    return mb;
}

static XBool IsPropertyDeleted(XDisplay*, XEvent *xe, char *vp)
{
    if (xe->type == PropertyNotify) {
	XPropertyEvent *pe= (XPropertyEvent*) xe;
	if (pe->window == *((XWindow*)vp) && pe->atom == XA_PRIMARY
						&& pe->state == PropertyDelete)
	    return TRUE;
    }
    return FALSE;
}

void XClipBoard::SendClipBoard(XSelectionRequestEvent *sre)
{
    XSelectionEvent sev;
    XEvent ev;
    
    sev.type= SelectionNotify;
    sev.display= sre->display;
    sev.selection= sre->selection;
    sev.target= sre->target;
    sev.time= sre->time;
    sev.property= sre->property;
    sev.requestor= sre->requestor;
    
    if (sev.target == etformat) {
    } else if (sev.target == XA_STRING) {
    } else {
	char *nm= XGetAtomName(display, sev.target);
	Error("XClipBoard::SendClipBoard", "can't convert to %s", nm);
	XFree(nm);
    }
       
    if (sev.property == None) {
	sev.property= XA_PRIMARY;
	sev.target= XA_STRING;
    }
	
    XChangeProperty(display, sev.requestor, sev.property, sev.target, 8,
	  PropModeReplace, (byte*) GetBuf()->Base(), (int) GetBuf()->tell(TRUE));
      
    XSelectInput(display, sev.requestor, PropertyChangeMask);
    XSendEvent(display, sev.requestor, XFalse, 0, (XEvent*) &sev);
    XFlush(display);
    XCheckIfEvent(display, &ev, IsPropertyDeleted, (char*) &sev.requestor);
}

