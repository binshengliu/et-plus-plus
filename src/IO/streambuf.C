//$streambuf$
#include "streambuf.h"
#include "../String.h"

streambuf::streambuf()
{
    setbuf(0, 0, 0, FALSE);
}

streambuf::streambuf(char* p, int l, int count)
{
    if (p)
	setbuf(p, l, count, FALSE);
    else
	setbuf(new char[l], l, count, TRUE);
}

streambuf::~streambuf()
{
    if (alloc)
	SafeDelete(base);
}

streambuf *streambuf::setbuf(char *p, int len, int count, bool all)
{
    base= gptr= p;
    pptr= p + count;
    eptr= base + len;
    alloc= all;
    return this;
}

int streambuf::doallocate()
{
    if (base= new char[BUFSIZE]) {
	setbuf(base, BUFSIZE, 0, TRUE);
	return 0;
    }
    return EOF;
}

int streambuf::overflow(int c)
{
    if (allocate() == EOF)
	return EOF;
    if (c != EOF)
	*pptr++ = c;
    return zapeof(c);
}

int streambuf::underflow()
{
    return EOF;
}

int streambuf::sputn(const char *s, int n)
{
    register int avail= eptr-pptr;
    register int req= n;
    
    while (avail < req) {
	memcpy(pptr, s, avail);
	s+= avail;
	pptr+= avail;
	req-= avail;
	if (overflow(zapeof(*s++)) == EOF)
	    return n-req ;
	--req ;
	avail= eptr-pptr;
    }
    memcpy(pptr, s, req);
    pptr+= req;
    return n;
}

int streambuf::sgetn(char *s, int n)
{
    register int avail= pptr-gptr;
    register char* p= s;
    register int req= n;
    
    while (avail < req) {
	memcpy(p, gptr, avail) ;
	p+= avail;
	req-= avail;
	gptr+= avail;
	if (underflow() == EOF)
	    return p-s;
	avail= pptr-gptr;
    }

    memcpy(p, gptr, req);
    gptr+= req;
    return n;
}

int streambuf::seek(long pos, bool inout)
{
    if (inout)
	pptr= base+pos;
    else
	gptr= base+pos;
    return 0;
}

long streambuf::tell(bool inout)
{
    if (inout)
	return (long) (pptr-base);
    return (long) (gptr-base);
}
