//$RegularExp,RegularExpIter$
#include "Object.h"
#include "RegularExp.h"
#include "String.h"
#include "ObjectTable.h"

//---- C++ interface to regex.c ---------------------------------------------

extern "C" {
    char *re_compile_pattern (char *pat, int,struct re_pattern_buffer *bufp);
    struct re_pattern_buffer *re_get_pattern_buffer(bool case_sensitive);
    void free_re_pattern_buffer(struct re_pattern_buffer *);
    void re_compile_fastmap (struct re_pattern_buffer *);
    int re_match (struct re_pattern_buffer *, char *, int, int, RegExRegs *regs);
    int re_match_2 (struct re_pattern_buffer *, char *, int, char *, int, int, 
					    RegExRegs *regs, int);
    int re_search (struct re_pattern_buffer *, char *str, int size, 
			       int start, int range, RegExRegs*,int *len);
    int re_search_2 (struct re_pattern_buffer *, char *str1, int size1,
		char *str2, int size2, int startpos, int range, RegExRegs *regs,
		int mstop, int *len); 
}

//-----class Regular Expression --------------------------------------------

MetaImpl(RegularExp, (TP(source), TP(result), TB(caseSensitive), TB(fastSearch), 0));

RegularExp::RegularExp(char *pat, bool cs, bool fs)
{
    if (pat == 0)
	pat= "";
    caseSensitive= cs;
    fastSearch= fs;
    pb= re_get_pattern_buffer(caseSensitive);
    source= strsave(pat);
    result= re_compile_pattern(source, strlen(source), pb);
    if (fastSearch)
	re_compile_fastmap(pb);
}

RegularExp::~RegularExp()
{
    if (pb) {
	free_re_pattern_buffer(pb);
	pb= 0;
    }
    SafeDelete(source);
}

void RegularExp::Reset(char *pat, bool cs, bool fs)
{
    if (strcmp(pat, source) == 0 && cs == caseSensitive && fs == fastSearch)
	return; // no recompilation necessary
    if (pb)
	free_re_pattern_buffer(pb);
    strreplace(&source, pat);
    caseSensitive = cs;
    fastSearch = fs;
    pb = re_get_pattern_buffer(caseSensitive);
    result = re_compile_pattern (pat, strlen(pat), pb);
    if (fastSearch)
	re_compile_fastmap (pb);
}

char *RegularExp::GetExprState()
{
    return result;
} 

int RegularExp::Match (char *pat, int pos, int len, RegExRegs *regs)
{
    if (len == -1) 
	len = strlen(pat);
    return re_match (pb, pat, len, pos, regs);    
} 

int RegularExp::Match2 (char *str1, char *str2, int len1,
		    int len2, int pos, int stopMatchAt, RegExRegs *regs)
{
    if (len1 == -1) 
	len1 = strlen(str1);
    if (len2 == -1)
	len2 = strlen(str2);
    stopMatchAt = range (0, len1+len2, stopMatchAt);
    return re_match_2 (pb, str1, len1, str2, len2, pos, regs, stopMatchAt);
} 

int RegularExp::SearchForward (char *string, int *nMatched,int start,  
					int len, int itsRange, RegExRegs *regs)
{
    if (len == -1) 
	len = strlen(string);
    start = range(0,len-1,start);
    itsRange = range (0,len - start,itsRange);
    return re_search (pb, string, len, start, itsRange, regs, nMatched);
}

int RegularExp::SearchBackward (char *string, int *nMatched,int start,  
					int len, int itsRange, RegExRegs *regs)
{
    if (len == -1) 
	len = strlen(string);
    start = range(0,len,start);
    itsRange = range (0,start,itsRange);
    return re_search (pb, string, len, start, -itsRange, regs, nMatched);
}

int RegularExp::SearchForward2 (char *str1, int len1, char *str2, int len2,
			int start, int itsRange, RegExRegs *regs, int *nMatched)
{
    if (len1 == -1) 
	len1 = strlen(str1);
    if (len2 == -1)
	len2 = strlen(str2);
    int clen = len1+len2;
    start = range (0, clen-1, start);
    itsRange = range (0,clen - start,itsRange);
    int mstop = start+itsRange;
    return re_search_2 (pb, str1, len1, str2, len2, start, itsRange, 
			regs, mstop, nMatched); 
}

int RegularExp::SearchBackward2 (char *str1,int len1, char *str2, int len2,
			int start, int itsRange, RegExRegs *regs, int *nMatched)
{
    if (len1 == -1) 
	len1 = strlen(str1);
    if (len2 == -1)
	len2 = strlen(str2);
    int clen = len1+len2;
    start = range (0, clen-1, start);
    itsRange = range (0,start+1,itsRange);
    int mstop = start-itsRange;
    return re_search_2 (pb, str1, len1, str2, len2, start, -itsRange, 
			regs, mstop, nMatched); 
}

const char *RegularExp::GetPattern()
{
    return source;
}

const char *RegularExp::MatchWordPattern()
{
    return "\\<%s\\>";
}

ostream& RegularExp::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    PrintString(s, (byte*)source);
    return s SP << caseSensitive SP << fastSearch SP;
}

istream& RegularExp::ReadFrom(istream& s)
{
    Object::ReadFrom(s);
    SafeDelete(source);
    ReadString(s, (byte**)&source);
    s >> Bool(caseSensitive) >> Bool(fastSearch);
    pb= re_get_pattern_buffer(caseSensitive);
    result= re_compile_pattern (source, strlen(source), pb);
    if (fastSearch)
	re_compile_fastmap (pb);
    return s;
}

void RegularExp::InspectorId(char *buf, int sz)
{
    strn0cpy(buf, source, sz);
}

//---- class Regular Expression Iterator --------------------------------------

RegularExpIter::RegularExpIter (RegularExp *r, char *s, int itsLen, int start,
								      int stp)
{
    str = s;
    re = r;
    int ll = itsLen;
    if (ll == -1)
	itsLen = strlen (s);
    len = itsLen;
    pos = range(0,len,start);
    stop = range (0, len, stp);
}

void RegularExpIter::Reset (RegularExp *r, char *s, int l, int start, int stp)
{
    str = s;
    re = r;
    if (l == -1)
	l = strlen (s);
    len = l;
    pos = range(0,len,start);
    stop = range (0,len,stp);
}

int RegularExpIter::operator()(int *l, RegExRegs *regs)                   
{
    int at = -1;
    if (pos != -1)
	pos = re->SearchForward (str, l, pos, len, stop - pos, regs);

    if (pos > stop)
	pos = at = -1;
    else {        
	at = pos;
	pos += *l;
    }
    return at;
}

