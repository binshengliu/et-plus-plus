#ifndef filebuf_First
#define filebuf_First

#include "streambuf.h"

enum open_mode { input= 0, output= 1, append= 2 };

//---- filebuf -----------------------------------------------------------------

class filebuf : public streambuf {     // a stream buffer for files
    int fd;         // file descriptor
    bool opened;    // non-zero if file has been opened

public:
    filebuf();
    filebuf(int nfd);
    filebuf(int nfd, char* p, int l);
    filebuf(char *path, open_mode m);

    ~filebuf();
    
    bool isopen()
	{ return opened; }

    // Open a file. return 0 if failure; return "this" if success
    filebuf* open(char *name, open_mode om);
    
    bool close();
    
    // Empty a buffer. Return EOF on error; 0 on success
    virtual int overflow(int c= EOF);
    
    // Fill a buffer. Return EOF on error or end of input; next character on success
    virtual int underflow();
    
    virtual int seek(long pos, bool inout);
    virtual long tell(bool inout);
};

#endif filebuf_First
