#ifndef ServerBitmap_First
#define ServerBitmap_First

#include "Bitmap.h"

class ServerBitmap: public Bitmap {
    int id;

public: 
    ServerBitmap(Point sz, u_short *data, u_short depth);
    ~ServerBitmap();
    int loadBitmap();
    int GetId()
	{ return (id >= 0) ? id : loadBitmap(); }
};

#endif ServerBitmap_First


