#ifndef pixwin_First
#define pixwin_First

#include "pixrect.h"

#define PWCD_SET_CMAP_SIZE      0x40
#define PIX_MAX_PLANE_GROUPS    12
#define RECTS_SORTS             4
#define BITSPERBYTE             8
#define VKEY_KBD_CODES          128
#define VKEY_CODES              VKEY_KBD_CODES

#define IM_CODEARRAYSIZE        (VKEY_CODES/((sizeof(char))*BITSPERBYTE))
#define IM_SHIFTARRAYSIZE       (sizeof(short)*BITSPERBYTE)

#define IE_NEGEVENT     (0x01)
#define SHIFTMASK       0x000E
#define CTRLMASK        0x0030
#define META_SHIFT_MASK 0x0040

#define WL_PARENT               0
#define WL_OLDERSIB             1
#define WL_YOUNGERSIB           2
#define WL_OLDESTCHILD          3
#define WL_YOUNGESTCHILD        4

#define WL_ENCLOSING            WL_PARENT
#define WL_COVERED              WL_OLDERSIB
#define WL_COVERING             WL_YOUNGERSIB
#define WL_BOTTOMCHILD          WL_OLDESTCHILD
#define WL_TOPCHILD             WL_YOUNGESTCHILD

#define WIN_LINKS               5
#define WIN_NULLLINK            -1
#define WIN_NAMESIZE            20

enum pw_batch_type {
	PW_NONE=0x7FFFFFFF,
	PW_ALL=0x7FFFFFFE,
	PW_SHOW=0x7FFFFFFD,
};

struct  pixwin {
	struct  pixrectops *pw_ops;
	caddr_t pw_opshandle;
	int     pw_opsx;
	int     pw_opsy;
	struct  rectlist pw_fixup;
	struct  pixrect *pw_pixrect;
	struct  pixrect *pw_prretained;
	struct  pixwin_clipops *pw_clipops;
	struct  pixwin_clipdata *pw_clipdata;
	char    pw_cmsname[20];
};

struct  pixwin_clipops {
	int     (*pwco_lock)( ... );
	int     (*pwco_unlock)( ... );
	int     (*pwco_reset)( ... );
	int     (*pwco_getclipping)( ... );
};

struct  pixwin_clipdata {
	int     pwcd_windowfd;
	short   pwcd_state;
	struct  rectlist pwcd_clipping;
	int     pwcd_clipid;
	int     pwcd_damagedid;
	int     pwcd_lockcount;
	struct  pixrect *pwcd_prmulti;
	struct  pixrect *pwcd_prsingle;
	struct  pixwin_prlist *pwcd_prl;
	struct  rectlist pwcd_clippingsorted[RECTS_SORTS];
	struct  rect *pwcd_regionrect;
	int     pwcd_x_offset;
	int     pwcd_y_offset;
	int     pwcd_flags;
	caddr_t pwcd_client;
	enum    pw_batch_type pwcd_batch_type;
	int     pwcd_op_count;
	int     pwcd_op_limit;
	struct  rect pwcd_batchrect;
	int             pwcd_winnum;
	struct win_lock_block   *pwcd_wl;
	int     pwcd_screen_x;
	int     pwcd_screen_y;
	char    pwcd_plane_group;
	char    pwcd_plane_groups_available[PIX_MAX_PLANE_GROUPS];
};

struct pw_pixel_cache {
	struct rect r;
	struct pixrect *plane_group[PIX_MAX_PLANE_GROUPS];
};

struct _cursor {
	short   cur_xhot, cur_yhot;
	int     cur_function;
	struct  pixrect *cur_shape;
	int     flags;

	short   horiz_hair_thickness;
	int     horiz_hair_op;
	int     horiz_hair_color;
	short   horiz_hair_length;
	short   horiz_hair_gap;

	short   vert_hair_thickness;
	int     vert_hair_op;
	int     vert_hair_color;
	short   vert_hair_length;
	short   vert_hair_gap;
};

struct inputmask {
	short   im_flags;
	char    im_inputcode[IM_CODEARRAYSIZE];
	short   im_shifts;
	short   im_shiftcodes[IM_SHIFTARRAYSIZE];
};

struct fullscreen_t {
	int     fs_windowfd;
	struct  rect fs_screenrect;
	struct  pixwin *fs_pixwin;
	struct  _cursor fs_cachedcursor;
	struct  inputmask fs_cachedim;
	int     fs_cachedinputnext;
	struct  inputmask fs_cachedkbdim;
};

struct  inputevent {
	short   ie_code;
	short   ie_flags;
	short   ie_shiftmask;
	short   ie_locx, ie_locy;
	struct  timeval ie_time;
};

#define event_id(event)         ((event)->ie_code)
#define event_flags(event)      ((event)->ie_flags)
#define event_shiftmask(event)  ((event)->ie_shiftmask)
#define event_x(event)          ((event)->ie_locx)
#define event_y(event)          ((event)->ie_locy)
#define event_time(event)       ((event)->ie_time)
#define event_shiftmask(event)          ((event)->ie_shiftmask)
#define event_shift_is_down(event)      (event_shiftmask(event) & SHIFTMASK)
#define event_ctrl_is_down(event)       (event_shiftmask(event) & CTRLMASK)
#define event_meta_is_down(event)       (event_shiftmask(event) & META_SHIFT_MASK)

#define pw_rop(dpw, dx, dy, w, h, op, sp, sx, sy)                       \
	(*(dpw)->pw_ops->pro_rop)((dpw)->pw_opshandle,                  \
	    (dx)-(dpw)->pw_opsx, (dy)-(dpw)->pw_opsy, (w), (h), (op),   \
	    (sp), (sx), (sy))
#define pw_batchrop(dpw, x, y, op, sbp, n)                              \
	(*(dpw)->pw_ops->pro_batchrop)((dpw)->pw_opshandle,             \
	    (x)-(dpw)->pw_opsx, (y)-(dpw)->pw_opsy, (op), (sbp), (n))
#define pw_stencil(dpw, x, y, w, h, op, stpr, stx, sty, spr, sy, sx)    \
	(*(dpw)->pw_ops->pro_stencil)((dpw)->pw_opshandle,              \
	    x-(dpw)->pw_opsx, y-(dpw)->pw_opsy, (w), (h), (op),         \
	    (stpr), (stx), (sty), (spr), (sy), (sx))
#define pw_destroy(pw)                                                  \
	(*pw_opsstd_ptr->pro_destroy)((pw))
#define pw_get(pw, x, y)                                                \
	(*pw_opsstd_ptr->pro_get)((pw),                                 \
	    (x)-(pw)->pw_clipdata->pwcd_x_offset,                       \
	    (y)-(pw)->pw_clipdata->pwcd_y_offset)
#define pw_put(pw, x, y, val)                                           \
	(*(pw)->pw_ops->pro_put)((pw)->pw_opshandle,                    \
	    (x)-(pw)->pw_opsx, (y)-(pw)->pw_opsy, (val))
#define pw_vector(pw, x0, y0, x1, y1, op, val)                          \
	(*(pw)->pw_ops->pro_vector)((pw)->pw_opshandle,                 \
	    (x0)-(pw)->pw_opsx, (y0)-(pw)->pw_opsy,                     \
	    (x1)-(pw)->pw_opsx, (y1)-(pw)->pw_opsy, (op), (val))
#define pw_region(pw, x, y, w, h)                                       \
	(struct pixwin *)(*pw_opsstd_ptr->pro_region)((pw),             \
	    (x), (y), (w), (h))
#define pw_lock(pixwin,rect)                                            \
	(*(pixwin)->pw_clipops->pwco_lock)((pixwin), (rect));
#define pw_unlock(pixwin)                                               \
	(*(pixwin)->pw_clipops->pwco_unlock)((pixwin));
#define pw_read(dpr, dx, dy, w, h, op, spw, sx, sy)                     \
	(*pw_opsstd_ptr->pro_rop)((dpr), (dx), (dy), (w), (h),          \
	    (op), (spw), (sx)-(spw)->pw_clipdata->pwcd_x_offset,        \
	    (sy)-(spw)->pw_clipdata->pwcd_y_offset)
#define pw_copy(dpw, dx, dy, w, h, op, spw, sx, sy)                     \
	(*pw_opsstd_ptr->pro_rop)((dpw),                                \
	    (dx)-(dpw)->pw_clipdata->pwcd_x_offset,                     \
	    (dy)-(dpw)->pw_clipdata->pwcd_y_offset, (w), (h), (op),     \
	    (spw), (sx)-(spw)->pw_clipdata->pwcd_x_offset,              \
	    (sy)-(spw)->pw_clipdata->pwcd_y_offset)
#define pw_putcolormap(pw, index, count, red, green, blue)              \
	(*pw_opsstd_ptr->pro_putcolormap)((pw),                         \
	    (index), (count), (red), (green), (blue))

extern "C" {
    extern pixwin *pw_open(int);
    extern void pw_polygon_2(pixwin*,int,int,int,int*,pr_pos*,int,pixrect*,int,int);
    extern int pw_set_region_rect(pixwin*, rect*, u_int, u_int);
    extern void pw_set_xy_offset(pixwin*, int xoff, int yoff);
    extern void pw_replrop(pixwin*, int xw, int yw,
				int width, int height, int op,
				struct pixrect *, int xr, int yr);
    extern void pw_traprop(pixwin*,int,int,struct pr_trap,int,pixrect*,int,int);
    extern void pw_polypoint(pixwin*,int,int,int,struct pr_pos *,int);
    extern void pw_close(pixwin*);
    extern void pw_damaged(pixwin*);
    extern void pw_donedamaged(pixwin*);
    extern void pw_setcmsname(pixwin*, char*);    


    pw_pixel_cache *pw_save_pixels(pixwin*, struct rect *r);
    void pw_restore_pixels(pixwin*, pw_pixel_cache*);

    extern void wmgr_top(int, int);
    extern void wmgr_bottom(int, int);
    extern void wmgr_completechangerect(int, rect*, rect*, int, int);
    extern int  wmgr_iswindowopen(int);

    extern int win_getnewwindow();
    extern void win_getrect(int fd, struct rect*);
    extern void win_setrect(int fd, struct rect*);
    extern void win_grabio(int);
    extern void win_releaseio(int);
    extern void win_remove(int);
    extern int win_nametonumber(char*);
    extern int win_getlink(int fd, int linkname);
    extern void win_setlink(int fd, int linkname, int number);
    extern void win_insert(int fd);
    extern void win_bell(int, struct timeval, pixwin*);
#   define win_inputnegevent(ie)   ((ie)->ie_flags&IE_NEGEVENT)

    extern struct fullscreen_t *fullscreen_init(int windowfd);
    extern void fullscreen_destroy(struct fullscreen_t *fs);

    extern bool we_getparentwindow(char *);
    extern void input_readevent(int fd, inputevent *ie);

#ifdef __GNUG__
    extern void cc_pw_traprop(pixwin*,int,int,struct pr_trap*,int,pixrect*,int,int);
    extern void cc_win_bell(int, struct timeval *, pixwin*);
#   define pw_traprop(a1,a2,a3,a4,a5,a6,a7,a8) cc_pw_traprop((a1),(a2),(a3),&(a4),(a5),(a6),(a7),(a8))
#   define win_bell(a1,a2,a3) cc_win_bell((a1),&(a2),(a3))
#endif

}

extern  struct  pixrectops *pw_opsstd_ptr;

#endif pixwin_First

