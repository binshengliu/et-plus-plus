#include "stream.h"
#include "streambuf.h"

//---- stream ---------------------------------------------------------------

stream::stream(streambuf* s, bool dodelete)
{
    bp= s;
    delete_streambuf= dodelete;
    state= _good;
}

stream::~stream()
{
    if (delete_streambuf)
	SafeDelete(bp);
}

char* stream::bufptr()
{
    return bp->Base();
}

long stream::tell()
{
    return 0;
}
