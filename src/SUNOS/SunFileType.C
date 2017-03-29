#include "SunFileType.h" 
#include "System.h"
#include "String.h"
#include "sunsystem.h"


SunFileType::SunFileType(char *path, bool shallow) : FileType(path, shallow)
{
    statbuf= getstatbuf(path);
}

SunFileType::~SunFileType()
{
    freestatbuf(statbuf);
}

long SunFileType::SizeHint()
{
    if (!statbuf)
	return -1;
    return filesize(statbuf);
}

int SunFileType::UniqueId()
{
    if (!statbuf)
	return 0;
    return uniquefileid(statbuf);
}

bool SunFileType::IsExecutable()
{
    if (!statbuf)
	return FALSE;
    return (bool) (isexecutable(statbuf) > 0);
}
    
bool SunFileType::SystemFileType()
{
    if (!statbuf)
	return FALSE;
    if (isspecial(statbuf)) {
	typename= cDocTypeSpecial;
	return TRUE;
    } else if (isdirectory(statbuf)) {
	typename= cDocTypeDirectory;
	return TRUE;
    }
    return FALSE;
}

