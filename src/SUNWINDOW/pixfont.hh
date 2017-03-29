#ifndef pixfont_First
#define pixfont_First

#include "pixrect.h"

struct pr_prpos {
    struct pixrect *pr;
    struct  pr_pos pos;
};

struct pixchar {
    struct pixrect *pc_pr;
    struct pr_pos pc_home;
    struct pr_pos pc_adv;
};

typedef struct pixfont {
    struct pr_size pf_defaultsize;
    struct pixchar pf_char[256];
} Pixfont;

extern "C" {
    struct pixfont *pf_open(char*);
    struct pixfont *pf_open_private(char*);
    struct pixfont *pf_default();
    void pf_close(struct pixfont*);
}

#endif pixfont_First

