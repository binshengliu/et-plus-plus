#ifndef RegExp_First
#ifdef __GNUG__
#pragma once
#endif
#define RegExp_First

#include "Object.h"
const int cNumRegs = 10;

// interface to the GNU emacs regular expression compiler,
// refer to regex.doc for a description of its advanced features

/* Examples:
    float:  "\\(+\\|-\\)?[0-9]*\\(\\.[0-9]*\\)?"
    int:    "\\(+\\|-\\)?[0-9]*"
    double: "\\(+\\|-\\)?[0-9]*\\(\\.[0-9]*\\)?\\([eE]\\(+\\|-\\)?[0-9]*\\)?" 
    alpha:  "[A-Za-z]+"
    lowercase:  "[a-z]+"
    uppercase:  "[A-Z]+"
    alphanum:   "[0-9A-Za-z]+"
    identifier: "[A-Za-z_][A-Za-z0-9_]*"
*/

typedef struct { 
    int start[cNumRegs];
    int end[cNumRegs];
} RegExRegs;

//   Pass the address of such a structure as an argument to Match, etc.,
//   if you want this information back.
//
//   start[i] and end[i] record the string matched by \( ... \) grouping i,
//   for i from 1 to cNumReg - 1.
//   start[0] and end[0] record the entire string matched.
//   eg: Regex = "a\(b*\)c"
//       String = "abbbbc" 
//       start[1] = 1, end[1] = 5
//

//---- class RegularExp -----------------------------------------------------

class RegularExp: public Object {
    struct re_pattern_buffer *pb;
    char *source;
    char *result;
    bool caseSensitive;
    bool fastSearch;
public:
    MetaDef(RegularExp);

    RegularExp(char *pattern= 0, bool caseSensitive= TRUE, bool fastSearch= TRUE);
    ~RegularExp();

    void Reset(char *pattern, bool caseSensitive= TRUE, bool fastSearch= TRUE);
	// recompiles the pattern if necessary

    char *GetExprState(); 
	// 0 == pattern ok, otherwise reason of failure

    int Match (char *string, int pos = 0, int len = -1, RegExRegs *regs = 0); 
	// returns length of matched pattern, starting at pos (-1= no match)

    int SearchForward (char *string, int *nMatched, int start = 0, int len = -1, 
				       int range = cMaxInt, RegExRegs *regs = 0);
	// return the position of the next matching substring (-1 == no match),
	// per default the range is set up to the end of the string, nMatched
	// is set to the number of matched characters

    int SearchBackward (char *string, int *nMatched, int start = cMaxInt, 
			 int len = -1, int range = cMaxInt, RegExRegs *regs = 0);
	// find next matching substring starting at pos and trying the
	// positions pos-1, pos-2 etc. 

    int Match2 (char *string1, char *string2, int len1 = -1,int len2 = -1,
		      int pos = 0, int stopMatchAt = cMaxInt, RegExRegs *regs = 0);
	// matches the pattern as if string1 and string2 were concatented

    int SearchForward2(char *str1,int size1, char *str2, int size2, int start,
			int itsRange, RegExRegs *regs, int *nMatched);
	// like Match2

    int SearchBackward2(char *str1,int size1, char *str2, int size2, int start,
			int itsRange, RegExRegs *regs, int *nMatched);
	// like Match2

    const char* GetPattern(); 
	// return the pattern of this regular expression

    virtual const char *MatchWordPattern();
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
    void InspectorId(char *buf, int sz);
};

//---- regular expression iterator --------------------------------------------

class RegularExpIter {
    int pos;
    char *str;
    int len;
    RegularExp *re;
    int stop;
public:
    RegularExpIter (RegularExp *, char *string, int len = -1,int start = 0, 
							  int stop = cMaxInt);
    int operator()(int *len, RegExRegs *regs = 0);                    
      // returns position of match (-1 = no match), and in len 
      // the number of matched characters
    void Reset (RegularExp *, char *string, int len = -1 , int start = 0, 
							  int stop = cMaxInt);
};

#endif RegExp_First

