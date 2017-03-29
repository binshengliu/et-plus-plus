#ifndef sunbitmap_First
#define sunbitmap_First

#include <stdio.h>
#include <osfcn.h>

typedef char *caddr_t;

#ifndef coord
#define coord   short
#endif

struct colormap_t {
	int              type;
	int              length;
	unsigned char   *map[3];
};

struct rect {
	coord   r_left, r_top;
	short   r_width, r_height;
};

struct  rectnode {
	struct  rectnode *rn_next;
	struct  rect rn_rect;
};

struct  rectlist {
	coord   rl_x, rl_y;
	struct  rectnode *rl_head;
	struct  rectnode *rl_tail;
	struct  rect rl_bound;

};

struct pr_size {
	int     x, y;
};

struct pr_pos {
	int     x, y;
};

struct pixrect {
	struct  pixrectops *pr_ops;
	struct  pr_size pr_size;
	int     pr_depth;
	struct mpr_data *pr_data;
};

struct mpr_data {
	int     md_linebytes;   /* number of bytes from one line to next */
	short   *md_image;      /* word address */
	struct  pr_pos md_offset;
	short   md_primary;
	short   md_flags;       /* Flag bits, see below */
};

struct pixrectops {
	int     (*pro_rop)( ... );
	int     (*pro_stencil)( ... );
	int     (*pro_batchrop)( ... );
	int     (*pro_nop)( ... );
	int     (*pro_destroy)( ... );
	int     (*pro_get)( ... );
	int     (*pro_put)( ... );
	int     (*pro_vector)( ... );
	struct pixrect* (*pro_region)( ... );
	int     (*pro_putcolormap)( ... );
	int     (*pro_getcolormap)( ... );
	int     (*pro_putattributes)( ... );
	int     (*pro_getattributes)( ... );
};

struct pr_chain {
	struct pr_chain *next;
	struct pr_size  size;
	int             *bits;
};

struct pr_fall {
	struct pr_pos   pos;
	struct pr_chain *chain;
};

struct pr_trap {
	struct pr_fall  *left, *right;
	int y0, y1;
};

#define pr_rop(dpr, dx, dy, w, h, op, spr, sx, sy)                      \
	(*(dpr)->pr_ops->pro_rop)(dpr, dx, dy, w, h, op, spr, sx, sy)
#define pr_batchrop(dpr, x, y, op, sbp, n)                              \
	(*(dpr)->pr_ops->pro_batchrop)(dpr, x, y, op, sbp, n)
#define pr_stencil(dpr, dx, dy, w, h, op, stpr, stx, sty, spr, sx, sy)  \
	(*(dpr)->pr_ops->pro_stencil)(dpr,dx,dy,w,h,op,stpr,stx,sty,spr,sx,sy)
#define pr_destroy(pr)                                                  \
	(*(pr)->pr_ops->pro_destroy)(pr)
#define pr_close(pr)    pr_destroy(pr)
#define pr_get(pr, x, y)                                                \
	(*(pr)->pr_ops->pro_get)(pr, x, y)
#define pr_put(pr, x, y, val)                                           \
	(*(pr)->pr_ops->pro_put)(pr, x, y, val)
#define pr_vector(pr, x0, y0, x1, y1, op, color)                        \
	(*(pr)->pr_ops->pro_vector)(pr, x0, y0, x1, y1, op, color)
#define pr_region(pr, x, y, w, h)                                       \
	(*(pr)->pr_ops->pro_region)(pr, x, y, w, h)

#define PIX_SRC         (0xC << 1)
#define PIX_DST         (0xA << 1)
#define PIX_NOT(op)     ((op) ^ 0x1E)
#define PIX_CLR         (0x0 << 1)
#define PIX_SET         (0xF << 1)
#define PIX_ERASE       (PIX_NOT(PIX_SRC) & PIX_DST)

#define PIX_DONTCLIP    0x1

#define PIX_OR          (PIX_SRC | PIX_DST)

#define PIX_COLOR(c)    ((c)<<5)

extern "C" {
    struct pixrect *mem_create(int, int, int);              
    struct pixrect *mem_point(int, int, int, short*);
    extern struct pixrect *pr_load(FILE*, colormap_t*);
    extern int pr_load_colormap(FILE*, struct rasterfile*, colormap_t*);
    extern int pr_load_header(FILE*, struct rasterfile*);
    extern struct pixrect *pr_load_image(FILE*, struct rasterfile*, colormap_t*);
    extern struct pixrect *pr_load_std_image(FILE*, struct rasterfile*, colormap_t*);
    extern void pr_replrop(pixrect*, int xw, int yw,
				int width, int height, int op,
				pixrect*, int xr, int yr);
    extern void pr_polygon_2(pixrect*,int,int,int,int*,struct pr_pos*,int,pixrect*,int,int);
    extern void pr_traprop(pixrect*,int,int,struct pr_trap,int,pixrect*,int,int);
    extern void pr_polypoint(pixrect*,int,int,int,struct pr_pos *,int);
#ifdef __GNUG__
    extern void cc_pr_traprop(pixrect*,int,int,struct pr_trap*,int,pixrect*,int,int);
#   define pr_traprop(a1,a2,a3,a4,a5,a6,a7,a8) cc_pr_traprop((a1),(a2),(a3),&(a4),(a5),(a6),(a7),(a8))
#endif
}

#endif sunbitmap_First

