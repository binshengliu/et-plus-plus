#include "filebuf.h"
#include "stream.h"
#include "../CType.h"
#include "../String.h"

istream cin((int)0, &cout);          // UNIX input stream 0 tied to cout

istream::istream(streambuf* s, ostream* t, bool dodelete) : stream(s, dodelete)        // bind to buffer
{
    tied_to= t;
}

istream::istream(int size, char* p) : stream(new streambuf(p, size, size), TRUE)    // bind to string
{
    tied_to= 0;
}

istream::istream(int fd, ostream* t) : stream(new filebuf(fd), TRUE)        // bind to file
{
    tied_to= t;
}

istream::istream(char *path) : stream(new filebuf(path, input), TRUE)        // bind to file
{
    tied_to= 0;
    if (! ((filebuf*)bp)->isopen())
	state |= _fail;
}

istream& istream::seek(long pos)
{
    bp->seek(pos, FALSE);
    return *this;
}

long istream::tell()
{
    return bp->tell(FALSE);
}

ostream* istream::tie(ostream* s)
{
    ostream* t= tied_to;
    tied_to= s;
    return t; 
}

void istream::eatwhite()
{
    flush();
    register streambuf *nbp= bp;
    register char c= nbp->sgetc();

    while (Isspace(zapeof(c)))
	c= nbp->snextc();
    if (c == EOF)
	state |= _eof;
}

int istream::read(u_char* s, int n)
{
    if (state || n <= 0) {
	state |= _fail;
	return 0;
    }
    int r= bp->sgetn((char*)s, n);
    if (r < n)
	setstate((state_value)(_fail|_eof));
    return r;
}

istream& istream::get(char& c)  // single character
{
    flush();
    
    if (state) {
	state |= _fail;
	return *this;
    }
    register tc= bp->sgetc();
    if (tc == EOF)
	state |= _fail|_eof;
    else {
	c= tc;
	bp->stossc();
    }
    return *this;
}

istream& istream::operator>> (register char& s)
{
    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    register c= bp->sgetc();
    if (c == EOF)
	state |= _fail|_eof;
    else {
	s= c;
	bp->stossc();
    }

    return *this;
}

istream& istream::operator>> (register char* s)
{
    register streambuf *nbp = bp;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    /* get string */
    register c= nbp->sgetc();
    if (c == EOF)
	state|= _fail;
    while (!Isspace(c) && c != EOF) {
	*s++= c;
	c= nbp->snextc();
    }
    *s= '\0';

    if (c == EOF)
	state|= _eof;

    return *this;
}

istream& istream::operator>> (long &i)
{
    register int c, ii= 0;
    register streambuf *nbp= bp;
    int neg= 0;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    switch (c= nbp->sgetc()) {
    case '-':
    case '+':
	neg= c;
	c= nbp->snextc();
	break;
    case EOF:
	state |= _fail;
    }

    if (Isdigit(c)) {
	do {
	    ii= ii*10+c-'0';
	} while (Isdigit(c=nbp->snextc()));
	i= (neg == '-') ? -ii : ii;
    } else
	state |= _fail;

    if (c == EOF)
	state |= _eof;
    return *this;
}

istream& istream::operator>> (int &i)
{
    long int l;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    if (*this >> l)
	i= (int) l;
    return *this;
}

istream& istream::operator>> (short &i)
{
    long l;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    if (*this >> l)
	i= (short) l;
    return *this;
}

istream& istream::operator>>(double &d)
{
    register int c= 0;
    char buf[256];
    register char* p= buf;
    register streambuf* nbp= bp;
    
    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    /* get the sign */
    switch (c= nbp->sgetc()) {
    case EOF:
	state= _eof|_fail;
	return *this;
    case '-':
    case '+':
	*p++= c;
	c= bp->snextc();
    }

    /* get integral part */
    while (Isdigit(c)) {
	*p++= c;
	c= bp->snextc();
    }

    /* get fraction */
    if (c == '.') {
	    do {
		    *p++ = c;
		    c = bp->snextc();
	    } while (Isdigit(c));
    }

    /* get exponent */
    if (c == 'e' || c == 'E') {
	*p++= c;
	switch (c= nbp->snextc()) {
	case EOF:
	    state= _eof|_fail;
	    return *this;
	case '-':
	case '+':
	    *p++ = c;
	    c = bp->snextc();
	}
	while (Isdigit(c)) {
	    *p++= c;
	    c= bp->snextc();
	}
    }

    *p= 0;
    d= atof(buf);

    if (c == EOF)
	state |= _eof;
    return *this;
}

istream& istream::operator>> (float &f)
{
    double d;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    if (*this >> d)
	f= d;
    return *this;
}

istream& istream::get(
	register char* s,       // character array to read into
	register int len,       // size of character array
	register char term      // character that terminates input
) {
    register int c;
    register streambuf *nbp= bp;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    if ((c= bp->sgetc()) == EOF) {
	state |= _fail | _eof;
	return *this;
    }

    while (c != term && c != EOF && len > 1) {
	*s++= c;
	c= nbp->snextc();
	len--;
    }
    *s= '\0';
    if (c == EOF)
	state |= _eof;
    return *this;
}

istream& istream::putback(register char c)
{
    bp->sputbackc(c);
    return *this;
}

istream& istream::get(
	register streambuf &s,  // streambuf to input to
	register char term      // termination character
){
    register c;
    register streambuf *nbp = bp;

    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    if ((c = bp->sgetc()) == EOF) {
	state|= _fail | _eof;
	return *this;
    }

    while (c != term && c != EOF) {
	if (s.sputc(c) == EOF)
	    break;
	c= nbp->snextc();
    }
    if (c == EOF)
	state|= _eof;
    return *this;
}

istream& istream::operator>> (register streambuf &s)
{
    register int c;
    register streambuf *nbp= bp;
    
    eatwhite();

    if (state) {
	state|= _fail;
	return *this;
    }

    if ((c= bp->sgetc()) == EOF) {
	state|= _fail | _eof;
	return *this;
    }

     while (c != EOF) {
	if (s.sputc(c) == EOF)
	    break;
	c= nbp->snextc();
    }
    if (c == EOF)
	state |= _eof;
    return *this;
}

istream& istream::get(u_char &b)
{
    flush();
    
    if (state) {
	state |= _fail;
	return *this;
    }
    register tc= bp->sgetc();
    if (tc == EOF)
	state |= _fail|_eof;
    else {
	b= (u_char) tc;
	bp->stossc();
    }
    return *this;
}

istream& istream::operator>> (u_char &s)
{
    eatwhite();

    if (state) {
	state |= _fail;
	return *this;
    }

    register c= bp->sgetc();
    if (c == EOF)
	state |= _fail|_eof;
    else {
	s= (u_char) c;
	bp->stossc();
    }

    return *this;
}
