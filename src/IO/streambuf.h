#ifndef streambuf_First
#define streambuf_First

#include "../Types.h"

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif

#define zapeof(c) ((c) & 0377)

class streambuf {       // a buffer for streams
protected:
    char*   base;       // pointer to beginning of buffer
    char*   pptr;       // pointer to next free byte
    char*   gptr;       // pointer to next filled byte
    char*   eptr;       // pointer to first byte following buffer
    bool    alloc;      // true if buffer is allocated using "new"

public:
    streambuf();
    streambuf(char* p, int l, int count= 0);
    virtual ~streambuf();
    
    // supply an area for a buffer.
    // The "count" parameter allows the buffer to start in non-empty.
    streambuf *setbuf(char *p, int len, int count= 0, bool alloc= FALSE);
    
    // return size of buffer
    u_long size()
	{ return (u_long) (eptr-base); }

    // allocate some space for the buffer
    int doallocate();

    int allocate()
	{ return base == 0 ? doallocate() : 0; }

    // Empty a buffer. Return EOF on error; 0 on success
    virtual overflow(int c= EOF);

    // Fill a buffer. Return EOF on error or end of input; next character on success
    virtual int underflow();
    
    virtual int seek(long pos, bool inout);
    virtual long tell(bool inout);
   
    char *Base()
	{ return base; }
	
    const int sgetc()        // get the current character
	{ return (gptr >= pptr) ? underflow() : zapeof(*gptr); }

    int snextc()             // get the next character
	{ return (gptr >= (pptr-1)) ? underflow() : zapeof(*++gptr); }

    inline void stossc()            // advance to the next character
	{ if (gptr++ >= pptr) underflow(); }

    // Return a character to the buffer (ala lookahead 1).  Since
    // the user may be "playing games" the character might be 
    // different than the last one returned by sgetc or snextc.
    // been extracted, nothing will be put back.
    // Putting back an EOF is DANGEROUS.
    inline void sputbackc(char c)
	{ if (gptr > base) *--gptr= c; }

    // put a character into the buffer
    inline int sputc(int c= EOF)
	{ return (eptr <= pptr) ? overflow(zapeof(c)) : (*pptr++= zapeof(c)); }

    // put n characters into the buffer
    int sputn(const char *buf, int n);

    // get n characters from the buffer
    int sgetn(char *buf, int n);
};

#endif streambuf_First
