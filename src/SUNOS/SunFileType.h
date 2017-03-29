#ifndef SunFileType_First
#define SunFileType_First

#include "FileType.h"

//---- SunFileType -----------------------------------------------------------

class SunFileType : public FileType {
    struct stat *statbuf;
    
    bool SystemFileType();
public:
    SunFileType(char *pathName, bool shallow);
    ~SunFileType();
    long SizeHint(); 
    int UniqueId();
    bool IsExecutable();
};

#endif SunFileType_First

