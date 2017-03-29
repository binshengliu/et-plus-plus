#include <stdio.h>
#include <vfont.h>
#include "Font.h"

void bdftovfont(ofp, font)
FILE *ofp;
Font *font;
{
    struct header hdr;
    struct dispatch dsp[NUM_DISPATCH];
    int i, totalbytes;
    Glyph *gp;
    int maxx, maxy;
    
    bzero(&hdr, sizeof hdr);
    bzero(dsp, sizeof dsp);
    
    for (totalbytes= i= 0; i < MAXCHARS; i++) {
	if ((gp= font->glyphs[i]) == 0)
	    continue;
	dsp[i].width= gp->width;
	dsp[i].addr= totalbytes;
	if (gp->nbytes > 0) {
	    dsp[i].nbytes= gp->nbytes;
	    dsp[i].up= gp->up;
	    dsp[i].down= gp->down;
	    dsp[i].left= gp->left;
	    dsp[i].right= gp->right;
	} else {
	    dsp[i].nbytes= dsp[i].up= dsp[i].right= 1;
	    dsp[i].down= dsp[i].left= 0;
	}
	totalbytes+= dsp[i].nbytes;
    }

    hdr.magic= VFONT_MAGIC;
    hdr.size= totalbytes;
    
    MaxExtent(font, &maxx, &maxy);
    hdr.maxx= maxx;
    hdr.maxy= maxy;
    
    fwrite(&hdr, sizeof(struct header), 1, ofp);
    fwrite(dsp, sizeof(struct dispatch), NUM_DISPATCH, ofp);
    
    for (i= 0; i < MAXCHARS; i++) {
	if (gp= font->glyphs[i]) {
	    if (gp->nbytes > 0 && gp->bits)
		fwrite(gp->bits, 1, gp->nbytes, ofp);
	    else
		fwrite("", 1, 1, ofp);
	}
    }
}

main(argc, argv)
int argc;
char *argv[];
{
    Font *fp;
    
    fp= ReadBdf(argv[1]);
    if (fp) {
	FontBBox(fp);
	bdftovfont(stdout, fp);
    }
    exit(0);
}
