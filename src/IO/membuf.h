#ifndef membuf_First
#define membuf_First

#include "streambuf.h"

const int cSizeInc= 1024;

class membuf: public streambuf {
public:
    membuf(int size= cSizeInc);
    int overflow(int c= EOF);
};

#endif membuf_First


