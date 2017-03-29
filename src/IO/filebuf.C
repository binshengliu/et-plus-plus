#include <stdio.h>
#include <osfcn.h>

#include "filebuf.h"


filebuf::filebuf()
{
    opened= FALSE;
}

filebuf::filebuf(int nfd)
{
    fd= nfd;
    opened= TRUE;
}

filebuf::filebuf(int nfd, char* p, int l) : streambuf(p, l)
{
    fd= nfd;
    opened= TRUE;
}

filebuf::filebuf(char* name, open_mode om)
{
    opened= FALSE;
    open(name, om);
}

filebuf::~filebuf()
{
    close();
}

filebuf* filebuf::open (char *name, open_mode om)
{
    switch (om) {
    case input:     
	fd= ::open(name, 0);
	break;
    case output:    
	fd= ::creat(name, 0664);
	break;
    case append:    
	fd= ::open(name, 1);
	if (fd < 0)
	    fd= ::creat(name, 0664);
	if (fd >= 0)
	    ::lseek(fd, 0, 2);
	break;
    }

    if (fd < 0) {
	opened= FALSE;
	return NULL;
    }

    opened= TRUE;
    return this;
}

bool filebuf::close()
{
    if (opened) {
	::close(fd);
	opened= FALSE;
	return TRUE;
    }
    return FALSE;
}

int filebuf::overflow(int c)
{
    if (!opened || allocate() == EOF)
	return EOF;

    if (base == eptr) {        // unbuffered IO
	if (c != EOF) {
	    *pptr= c;
	    if (::write(fd, pptr, 1) != 1)
		return EOF;
	}
    } else {                          // buffered IO
	if (pptr > base)
	    if (::write(fd, base, pptr-base) != pptr-base)
		return EOF;
	pptr= gptr= base;
	if (c != EOF)
	    *pptr++ = c;
    }
    return zapeof(c);
}

int filebuf::underflow()
{
    int count;

    if (!opened || allocate() == EOF)
	 return EOF;

    if ((count= ::read(fd, base+1, eptr-base-1)) < 1)
	return EOF;
    gptr= base+1;                   // leave room for putback
    pptr= gptr+count;
    return zapeof(*gptr);
}

int filebuf::seek(long pos, bool inout)
{
    if (inout)
	pptr= base+pos;
    else {
	if (opened) {
	    lseek(fd, pos, 0);
	    gptr= pptr= 0;
	}
    }
    return 0;
}

long filebuf::tell(bool)
{
    if (opened)
	return lseek(fd, 0, 1) - (long) (pptr - gptr);
    return 0;
}
