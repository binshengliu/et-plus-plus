#include "NeWSFont.h"
#include "NeWSWindowSystem.h"
#include "String.h"
#include "psio.h"
#include "news.h"

NeWSServerFont::NeWSServerFont(char*, GrFont fid, int ps, GrFace fc)
{
    family= (byte) fid;
    size= (byte) ps;
    face= (byte) fc;
    id= -1;
}

bool NeWSServerFont::Loaded()
{
    return id > -1;
}

bool NeWSServerFont::LoadFont()
{
    ps_makefont((int)family, (int)size, (int)face, &id, cw, 256);
    
    ht= 10;
    bs= 3;
    ils= ht + bs + 3;
    
    return FALSE;
}

Font *NeWSServerFont::MakeFont(Font**, GrFace fc)
{
    NeWSServerFont *nfd= new NeWSServerFont(0, family, size, face | fc);
    nfd->LoadFont();
    return nfd;
}

NeWSServerFont::~NeWSServerFont()
{
}

Font* NeWSFontManager::MakeFont(char *vname, GrFont fid, int ps, GrFace fc)
{
    return new NeWSServerFont(vname, fid, ps, fc);
}
