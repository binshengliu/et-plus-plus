#ifndef NeWSBitmap_First
#define NeWSBitmap_First

#include "Bitmap.h"

class NeWSBitmap: public Bitmap {
    int id;

private:    
    int Download();

public: 
    NeWSBitmap(Point sz, u_short *data, u_short dep);
    NeWSBitmap(const char *name);
    ~NeWSBitmap();
    
    int GetPixmap()
	{ return id >= 0 ? id : Download(); }
};

#endif NeWSBitmap_First


