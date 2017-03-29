#include "DevBitmap.h"
#include "ServerBitmap.h"

#include "ServerConnection.h"

ServerBitmap::ServerBitmap(Point sz, u_short *im, u_short dep)
{
    id= -1;
}

ServerBitmap::~ServerBitmap()
{
}

int ServerBitmap::loadBitmap()
{
    gConnection->SendMsg(eMsgMakeBitmap, -1, "pbsI", &dbm->size,
				ShortsPerLine()*2*dbm->size.y, dbm->image, dbm->depth, &id);
     return id;
}
