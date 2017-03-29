#ifndef PostScriptPort_First
#define PostScriptPort_First

#include <stdio.h>
#include "PrintPort.h"

#define SIZE_PRNNAME 30

struct PostScriptState {
    int resolution;
    bool portrait, smooth, wysiwyg, prolog;
    char prnname[SIZE_PRNNAME+1];
};

//---- PostScriptPort ----------------------------------------------------------

class PostScriptPort: public PrintPort {
    PostScriptState *pst;
    bool tofile;
    float xorigin, yorigin;
    FILE *pfp, *prfp;
    int fix, lastpsz, fid, maxfont, bcnt, pagecnt, currpage;
    struct fts *fonts;
    char *PrintTmpFile;
    FontPtr lbfont, lfont;
    byte bbuf[MaxTextBatchCnt];
    
protected:
    void DrawObject(char, Rectangle*, Point, int, GrLineCap);
    void DrawPolygon(char, Rectangle*, Point*, int, GrPolyType, int, GrLineCap);
    void Printf(char *fmt, ...);
    void SetPenSize(int pensize);
    int EnrollFont(FontPtr fd);
    void flushfonts();
    void downloadfont(FontPtr fd, char *fname, int *ia);
    void loadlib(FILE *ofp, char *name);

public:
    MetaDef(PostScriptPort);
    PostScriptPort(char *name, PostScriptState *pst);
    ~PostScriptPort();
    
    void DevOpenPage(int pn);
    void DevClosePage();
    
    void DevSetPattern(class DevBitmap*);
    void DevSetOther(int);
    void DevSetGrey(float f);
    
    void DevClip(Rectangle, Point);
    
    void DevStrokeLine(int, Rectangle*, GrLineCap, Point, Point);
    void DevShowBitmap(Rectangle*, struct Bitmap*);
    bool DevShowChar(FontPtr fdp, Point pos, byte c, bool isnew, Point);
    void DevShowTextBatch(Rectangle*, Point);
    void DevGiveHint(int, int, void*);
};

#endif PostScriptPort_First
