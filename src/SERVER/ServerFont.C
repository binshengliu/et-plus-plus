#include "ServerFont.h"
#include "String.h"
#include "ServerConnection.h"

ServerFont::ServerFont(char*, GrFont fid, int ps, GrFace fc)
{
    family= fid;
    size= (short) ps;
    face= fc;
    id= -1;
}

ServerFont::~ServerFont()
{
}

bool ServerFont::Loaded()
{
    return id >= 0;
}

bool ServerFont::LoadFont()
{
    gConnection->SendMsg(eMsgMakeFont, -1, "sssIB", (short) family, (short) size,
			(short) face, &id, sizeof(Font)-sizeof(void*), cw);
    return FALSE;
}

Font *ServerFont::MakeFont(Font**, GrFace fc)
{
    Font *nfd= new ServerFont(0, (GrFont) family, size, (GrFace)(face | fc));
    nfd->LoadFont();
    return nfd;
}

Font* ServerFontManager::MakeFont(char *vname, GrFont fid, int ps, GrFace fc)
{
    return new ServerFont(vname, fid, ps, fc);
}
