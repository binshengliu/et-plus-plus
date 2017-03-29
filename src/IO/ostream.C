#include "stream.h"
#include "filebuf.h"
#include "../String.h"

ostream cout(1);        // UNIX output stream 1
ostream cerr(2, 0, 0);  // UNIX output stream 2

//---- ostream -----------------------------------------------------------------

ostream::ostream(streambuf* s, bool dodelete) : stream(s, dodelete)
{
}

ostream::ostream(int fd) : stream(new filebuf(fd), TRUE)
{
}

ostream::ostream(int fd, char *bp, int size) : stream(new filebuf(fd, bp, size), TRUE)
{
}

ostream::ostream(int size, char* p) : stream(new streambuf(p, size), TRUE)
{
}

ostream::ostream(char* path) : stream(new filebuf(path, output), TRUE)
{
    if (! ((filebuf*)bp)->isopen())
	state |= _fail;
}

ostream::~ostream()
{
    flush();
}

ostream& ostream::seek(long pos)
{
    bp->seek(pos, TRUE);
    return *this;
}

long ostream::tell()
{
    return bp->tell(TRUE);
}

ostream& ostream::flush()
{
    bp->overflow();
    return *this;
}

ostream& ostream::put(char c)
{
    if (state == 0)
	if (bp->sputc(c) == EOF)
	    state |= _eof | _fail;
    return *this;
}

ostream& ostream::form(const char* va_(fmt), ...)
{
    if (state == 0) {
	va_list ap;
	va_start(ap,va_(fmt));
	const char *format= va_(fmt); 
	char buf[BUFSIZE];
    
	register int ll= (int) vsprintf(buf, format, ap);
	if (0 < ll && ll < BUFSIZE)                 // length
	    ;
	else if (buf < (char*)ll && (char*)ll < buf+BUFSIZE) // pointer to trailing 0
	    ll= (char*)ll - buf;
	else
	    ll= strlen(buf);
	write((u_char*)buf, ll);
	va_end(ap);
    }
    return *this;
}

ostream& ostream::operator<< (const char* s)
{
    if (state == 0 && s && *s)
	write((u_char*)s, strlen(s));
    return *this;
}

ostream& ostream::operator<< (unsigned int i)
{
    return form("%u", i);
}

ostream& ostream::operator<< (int i)
{
    return form("%d", i);
}

ostream& ostream::operator<< (unsigned long i)
{
    return form("%lu", i);
}

ostream& ostream::operator<< (long i)
{
    return form("%ld", i);
}

ostream& ostream::operator<< (double d)
{
    return form("%g", d);
}

ostream& ostream::operator<< (streambuf &b)
{
    register streambuf* nbp= bp;
    register int c;

    if (state)
	    return *this;

    c= b.sgetc();
    while (c != EOF) {
	    if (nbp->sputc(c) == EOF) {
		    setstate((state_value)(_eof|_fail));
		    break;
	    }
	    c= b.snextc();
    }
	    
    return *this;
}

int ostream::write(const u_char *s, int n)     
{
    if (state || n <= 0)
	return 0;
    int r= bp->sputn((char*)s, n);
    if (r != n)
	setstate((state_value)(_eof|_fail));
    return r;
}

