#include <vfont.h>
#include <stdio.h>
#include <pixrect/pixrect_hs.h>
#include <ctype.h>
#include "Font.h"

Font *vfont2bdf(ifp, name)
FILE *ifp;
char *name;
{
    int ils, diff, i, size;
    Pixfont *pf;
    byte *bitmaps;
    char *cp;
    struct header hdr;
    struct dispatch dsp[NUM_DISPATCH], *dp;
    Font *font= 0;
    
    fread(&hdr, sizeof (struct header), 1, ifp);
    
    if (hdr.magic != VFONT_MAGIC) {
	fprintf(stderr, "%s not a vfont\n", name);
	return 0;
    }
    
    pf= pf_open_private(name);
    ils= pf->pf_defaultsize.y;
    pf_close(pf);
    
    fread(dsp, sizeof (struct dispatch), NUM_DISPATCH, ifp);
    bitmaps= (unsigned char*) malloc(hdr.size);
    fread(bitmaps, hdr.size, 1, ifp);
    
    for (cp= name; *cp; cp++)
	if (isdigit(*cp))
	    break;
    if (*cp)
	size= atoi(cp);
    else
	size= 12;
    
    font= NewFont(name, size, 72, 72, 9999, 9999, 9999);
    
    for (dp= dsp, i= 0; i < NUM_DISPATCH; i++, dp++)
	if (dp->nbytes)
	    AddGlyph(font, i, dp->width, dp->up, dp->down, dp->left, dp->right,
			    dp->nbytes, &bitmaps[dp->addr], adobename(i), 0);
    return font;
}

main(argc, argv)
int argc;
char *argv[];
{
    Font *fp;
    FILE *ifp;
    
    ifp= fopen(argv[1], "r");
    if (ifp) {
	fp= vfont2bdf(ifp, argv[1]);
	if (fp) {
	    FontBBox(fp);
	    DumpAsBdf(stdout, fp);
	    FreeFont(fp);
	}
	fclose(ifp);
    }
}
