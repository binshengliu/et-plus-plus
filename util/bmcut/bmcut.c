/*
 * reads in SUN's file format for icons and cursors and extracts the 
 * specified region
 * Arguments: bmcut filename width height
 */

#include <stdio.h>
#include <suntool/tool_hs.h>
#include <suntool/icon_load.h>

main(argc, argv)
int argc;
char **argv;
{
    struct pixrect *lpr;
    short *image;
    int x, y, w, h, spl1, spl2;
    
    lpr= icon_load_mpr(argv[1], "error during icon_load_mpr");
    
#ifdef i386
    /* force flip */
    mpr_d(lpr)->md_flag &= ~MP_I386;
    pr_flip(lpr);
#endif i386
    
    spl1= (lpr->pr_size.x-1)/16+1;
    
    w= atoi(argv[2]);
    h= atoi(argv[3]);
    spl2= (w-1)/16+1;
      
    image= mpr_d(lpr)->md_image;
    
    printf("/* Format_version=1, Width=%d, Height=%d, Depth=%d, Valid_bits_per_item=16\n", w, h, 1);
    printf(" */\n");
    
    for (y= 0; y < h; y++) {
	printf("\t");
	for (x= 0; x < spl2; x++)
	    printf("0x%04x,", (unsigned short) image[y*spl1+x]);
	printf("\n");
    }
}
