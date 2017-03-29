hello
#define _CPS_SEENDEFS
#ifndef FILE
#include <stdio.h>
#endif
#ifndef PSFILE
#include <psio.h>
#endif
extern PSFILE *PostScript, *PostScriptInput;
PSFILE *ps_open_PostScript();
#define ps_flush_PostScript() (psio_flush(PostScript),(psio_error(PostScript) ? exit(0) : 0))
#define ps_close_PostScript() (psio_close(PostScript),PostScript = 0)
#define ps_moveto(P__0,P__1)  pprintf(PostScript, _CPS_ps_moveto, 5, P__0, P__1)
extern char _CPS_ps_moveto[];
#define ps_rmoveto(P__0,P__1)  pprintf(PostScript, _CPS_ps_rmoveto, 5, P__0, P__1)
extern char _CPS_ps_rmoveto[];
#define ps_lineto(P__0,P__1)  pprintf(PostScript, _CPS_ps_lineto, 5, P__0, P__1)
extern char _CPS_ps_lineto[];
#define ps_rlineto(P__0,P__1)  pprintf(PostScript, _CPS_ps_rlineto, 5, P__0, P__1)
extern char _CPS_ps_rlineto[];
#define ps_closepath() psio_putc('\263', PostScript)
#define ps_arc(P__0,P__1,P__2,P__3,P__4)  pprintf(PostScript, _CPS_ps_arc, 11, P__0, P__1, P__2, P__3, P__4)
extern char _CPS_ps_arc[];
#define ps_stroke() psio_putc('\305', PostScript)
#define ps_fill() psio_putc('\267', PostScript)
#define ps_show(P__0)  pprintf(PostScript, _CPS_ps_show, 3, P__0)
extern char _CPS_ps_show[];
#define ps_cshow(P__0,L__0)  pprintf(PostScript, _CPS_ps_cshow, 4, L__0, P__0)
extern char _CPS_ps_cshow[];
#define ps_findfont(P__0)  pprintf(PostScript, _CPS_ps_findfont, 3, P__0)
extern char _CPS_ps_findfont[];
#define ps_scalefont(P__0)  pprintf(PostScript, _CPS_ps_scalefont, 3, P__0)
extern char _CPS_ps_scalefont[];
#define ps_setfont() psio_putc('\302', PostScript)
#define ps_gsave() psio_putc('\272', PostScript)
#define ps_grestore() psio_putc('\271', PostScript)
extern int ps_next_user_token;
#define ps_define_stack_token(u) ps_DO_defuser(u/**/_token = ps_next_user_token++)
#define ps_define_value_token(u) (pprintf(PostScript, "/u load ", sizeof "/u load " - 1), \
ps_define_stack_token(u))
#define ps_define_word_token(u) (pprintf(PostScript, "/u cvx ", sizeof "/u cvx " - 1), \
ps_define_stack_token(u))
#define ps_DO_defuser(P__0)  pprintf(PostScript, _CPS_ps_DO_defuser, 4, P__0)
extern char _CPS_ps_DO_defuser[];
#define ps_DO_finddef(P__0,P__1)  pprintf(PostScript, _CPS_ps_DO_finddef, 7, P__0, P__1)
extern char _CPS_ps_DO_finddef[];
#define ps_finddef(font,usertoken) ps_DO_finddef(font,(usertoken)=ps_next_user_token++)
#define ps_DO_scaledef(P__0,P__1,P__2)  pprintf(PostScript, _CPS_ps_DO_scaledef, 9, P__0, P__1, P__2)
extern char _CPS_ps_DO_scaledef[];
#define ps_scaledef(font,scale,usertoken) ps_DO_scaledef(font,scale,(usertoken) = ps_next_user_token++)
#define ps_usetfont(P__0)  pprintf(PostScript, _CPS_ps_usetfont, 3, P__0)
extern char _CPS_ps_usetfont[];
