#ifndef X11_First
#define X11_First

#define USG
#define __TYPES__

#define Font XFont
#define Window XWindow

typedef char *caddr_t;

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/Xresource.h"

#undef Bool
#undef Font
#undef Window
#undef IsCursorKey
#undef IsFunctionKey

#define XBool int
#define XFalse False
#define XTrue True
#define XDisplay Display
#define XCursor Cursor
#define XColormap Colormap

#endif X11_First

