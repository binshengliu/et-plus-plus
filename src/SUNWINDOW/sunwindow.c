#include <sunwindow/window_hs.h>
#include <fcntl.h>
#include <sunwindow/win_ioctl.h>
#include <sys/ioctl.h>

typedef int BOOL;
  

void _sunwindow_setmask(fd)
int fd;
{
    register short i;
    struct inputmask Mask;

    input_imnull(&Mask);
    Mask.im_flags= IM_ASCII | IM_NEGEVENT | IM_META;

    for(i = KEY_LEFTFIRST; i <= VKEY_LASTFUNC; i++)
	win_setinputcodebit(&Mask,i);
    for(i = BUT_FIRST; i <= BUT_LAST; i++)
	win_setinputcodebit(&Mask,i);
    win_setinputcodebit(&Mask, LOC_WINENTER);
    win_setinputcodebit(&Mask, LOC_WINEXIT);
    win_setinputcodebit(&Mask, LOC_MOVE);
    win_setinputcodebit(&Mask, LOC_MOVEWHILEBUTDOWN);
    win_setinputmask(fd, &Mask, (struct inputmask*) 0, WIN_NULLLINK);
}

void _sunwindow_setcursor(fd, crs)
int fd;
struct _cursor *crs;
{
    ioctl(fd, WINSETCURSOR, crs);
}

void _sunwindow_setmouse(fd, x, y, m)
int fd, x, y;
BOOL m;
{
    if (m) {
	x+= win_get_vuid_value(fd, LOC_X_ABSOLUTE);
	y+= win_get_vuid_value(fd, LOC_Y_ABSOLUTE);
    }
    win_setmouseposition(fd, x, y);
}

/* CC stub routines, needed for g++ */

extern void pr_traprop();

void cc_pr_traprop(pr,i1,i2,pt,i3,pr2,i4,i5)
struct pixrect *pr, *pr2;
struct pr_trap *pt;
int i1, i2, i3, i4, i5;
{
    struct pr_trap npt;
    npt= *pt;
    pr_traprop(pr,i1,i2,npt,i3,pr2,i4,i5);
}

extern void pw_traprop();

void cc_pw_traprop(pw,i1,i2,pt,i3,pr2,i4,i5)
struct pixwin* pw;
int i1, i2, i3, i4, i5;
struct pr_trap *pt;
struct pixrect* pr2;
{
    struct pr_trap npt;
    npt= *pt;
    pw_traprop(pw,i1,i2,npt,i3,pr2,i4,i5);
}

extern void win_bell();

void cc_win_bell(i, tv, pw)
int i;
struct timeval *tv;
struct pixwin* pw;
{
    struct timeval newtv;

    newtv= *tv;
    win_bell(i, newtv, pw);
}

