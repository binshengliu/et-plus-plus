#ifndef Stream_First
#define Stream_First

#include "../Types.h"

#ifndef EOF
#define EOF (-1)
#endif EOF

#define SP << " "
#define NL << "\n"
#define TAB << "\t"

enum state_value { _good= 0, _eof= 1, _fail= 2, _bad= 4 };

//---- stream -----------------------------------------------------------------

class stream {
protected:
    bool delete_streambuf;
    class streambuf* bp;
    short state;

    stream(streambuf* s, bool dodelete= FALSE);
		
public:
    ~stream();
    
    operator void*()
	{ return _eof < state ? 0 : this; }
    int operator!()
	{ return _eof < state; }
    int eof()
	{ return state & _eof; }
    int fail()
	{ return _eof < state; }
    int bad()
	{ return _fail < state; }
    int good()
	{ return state == _good; }
    void setstate(state_value st)
	{ state= st; }
    void clear()
	{ state= _good; }
    int rdstate()
	{ return state; }
    char* bufptr();
    streambuf* getsbuf()
	{ return bp; }
    long tell();
};

//---- ostream -----------------------------------------------------------------

class istream;

class ostream : public stream {
public:
    ostream(streambuf* s, bool dodelete= FALSE);
    ostream(int fd);
    ostream(int fd, char *p, int size);
    ostream(int size, char *p);
    ostream(char *path);
    ~ostream();
	    
    ostream& flush();
    ostream& form(const char* va_alist, ...);
    ostream& put(char);                     // put('a') writes a
    ostream& seek(long pos);
    ostream& rewind()
	{ return seek(0); }
    long tell();
    int write(const u_char *s, int n);

    ostream& operator<< (const char*);      // write
    ostream& operator<< (int);
    ostream& operator<< (unsigned int); 
    ostream& operator<< (long);             // beware: << 'a' writes 97
    ostream& operator<< (unsigned long);
    ostream& operator<< (double);
    ostream& operator<< (streambuf&);
};

//---- istream -----------------------------------------------------------------

/*
  The >> operator reads after skipping initial whitespace
  get() reads but does not skip whitespace

  if >> fails     (1) the state of the stream turns non-zero
		  (2) the value of the argument does not change
		  (3) non-whitespace characters are put back onto the stream

  >> get() fails if the state of the stream is non-zero
*/

class istream : public stream {
    ostream*        tied_to;

public:
    istream(streambuf *s, ostream *t= 0, bool dodelete= FALSE); // bind to buffer
    istream(int size, char *p);                             // bind to string
    istream(int fd, ostream *t= 0);                         // bind to file
    istream(char *path);
   
    void flush()
	{ if (tied_to) tied_to->flush(); }

    operator void*()
	{ return stream::operator void*(); }
    void eatwhite();
    istream& putback(char c);
    istream& seek(long pos);
    istream& rewind()
	{ return seek(0); }
    long tell();
    ostream* tie(ostream* s);
    int read(u_char *s, int n);

    // raw input: get's do not skip whitespace
    istream& get(char*, int, char= '\n');           // string
    istream& get(streambuf &sb, char= '\n');
    istream& get(char &c);                          // single character
    
    // formatted input: >> skip whitespace
    istream& operator>> (char*);                    // string
    istream& operator>> (char&);                    // single character
    istream& operator>> (short&);
    istream& operator>> (int&);
    istream& operator>> (long&);
    istream& operator>> (float&);
    istream& operator>> (double&);
    istream& operator>> (streambuf&);

    istream& get(u_char &b);                        // single unsigned character
    istream& operator>> (u_char&);                  // single unsigned character
};

extern istream cin;     // standard input predefined
extern ostream cout;    // standard output
extern ostream cerr;    // error output

#endif Stream_First
