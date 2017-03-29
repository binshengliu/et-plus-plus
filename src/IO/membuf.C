#include "membuf.h"
#include "../Storage.h"

membuf::membuf(int size) : streambuf(0, size)
{
}

int membuf::overflow(int c)
{
    if (c != EOF) {
	size_t oldsize= (size_t) size();
	int oldpos= (int) tell(TRUE);
    
	if (oldpos >= size())
	    setbuf((char*)Realloc(base, oldsize*2), oldsize*2, oldpos, TRUE);
    
	*pptr++= c;
    }
    return zapeof(c);
}
