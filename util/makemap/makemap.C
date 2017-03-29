/*
 * Produces a "map file" for the c++ source file given as argument.
 * The map file for file xxx.C is stored as ./.MAP/xxx.C.map.
 * The map file will only be created if a directory with the name ".MAP"
 * exists.
 * Arguments: makemap filename ...
 * Note: inline member functions in the header file are ignored
 */


#include <stdio.h>
#include <ctype.h>

typedef unsigned int bool;
#define FALSE 0
#define TRUE 1

extern "C" { 
    char* index(char *,int);
    char* rindex(char *,int);
}

bool inDefine;
long lastComment;
int prevCh, c; 
int braceStack, inString, escape, inClass, line;
long canBeClass, canBeFunction;

inline int At(FILE *fp, long pos)
{ 
    fseek(fp, pos, 0); 
    return getc(fp); 
}

void FoundComment(FILE *fp, int &line)
{
    int c, prevCh= 0, l= line;
   
    while ((c= getc(fp)) != EOF) {
	if (c == '\n')
	    line++;
	if (c == '/' && prevCh == '*') 
	    break;
	prevCh= c;
    }
}

void FoundFunctionOrMethod(FILE *fp, FILE *map, long at, long lastComment)
{
    int c, len= 0;
    long pos= at;
    long oldpos= ftell(fp);
    while (--pos >= lastComment) {
	c= At(fp, pos);
	if (!isspace(c) && !index("[]*+-=%&><|:^%()~/",c)) // overloaded operators
	    break;
    }
    while (pos >= lastComment) {
	c= At(fp, pos);
	if (!(isalnum(c) || c == ':' || c == '~' ))
	    break;
	if (pos-1 < 0)
	    break;
	pos--;
	len++;
    }
    
    if (len) {
	char buf[500];
	pos++;
	fseek(fp, pos, 0); 
	fread(buf, sizeof(char), len, fp);
	buf[len]= '\0';
	char *p= index(buf, ':');
	if (p) {
	    *p= '\0';
	    char *pp= p+2;
	    fprintf(map, "%s _%s_%s %d 2\n", buf, buf, pp, line);
	}
    } 
    fseek(fp, oldpos, 0); 
}

void Analyze(FILE *fp, FILE *map)
{
    inDefine= escape= inClass= FALSE;
    lastComment= prevCh= braceStack= inString= line= 0;
    c= '\n'; 
    canBeClass= canBeFunction= -1;
    
    while ((c= getc(fp)) != EOF) {
	if (escape)
	    escape=FALSE;
	else
	    switch (c) {
	    case '#':
		if (prevCh == '\n')
		    inDefine= TRUE;
		break;
	    case '\\':
		escape= TRUE;
		break;
	    case '\n':
		inDefine= FALSE;
		line++;
		break;
	    case '*':
		if (prevCh == '/' && inString == 0) {
		    if (canBeFunction == -1 && canBeClass == -1)
			lastComment= ftell(fp); 
		    FoundComment(fp, line);
		    prevCh= 0;
		    continue;
		}
		break;
	    case '/':
		if (prevCh == '/' && inString == 0) {
		    if (canBeFunction == -1 && canBeClass == -1)
			lastComment= ftell(fp); 
		    prevCh= 0;
		    continue;
		}
		break;
	    case ';':
		canBeFunction= canBeClass= -1;
		break;
	    case '{':
		if (canBeFunction != -1) {
		    FoundFunctionOrMethod(fp, map, canBeFunction, lastComment);
		    canBeFunction= -1;
		}
		if (canBeClass != -1) {
		    canBeClass= -1;
		}
		if (inString == 0)
		    braceStack++;
		break;
	    case '}':
		if (inString == 0)
		    braceStack--;
		break;
	    case '(':
		if (!inDefine && inString == 0) {
		    if (braceStack == 0 && canBeFunction == -1)
			canBeFunction= ftell(fp)-1;
		    braceStack++;
		}
		break;
	    case ')':
		if (!inDefine && inString == 0)
		    braceStack--;
		break;
	    case '\'':
	    case '\"':
		if (inString == 0) {
		    if (c == '\"')
			inString= '\"';
		    else
			inString= '\'';
		} else {
		    if ((inString == '\"' && c == '\"') || (inString == '\'' 
							 && c == '\''))
			inString= 0;
		}
		break;
	    default:
		break;
	}
	prevCh= c;
    }
    
}

FILE *MapFileName(char *path)
{  
    char buf[400], *fname;
    char *base= rindex(path,'/');
    if (base) 
	fname= base+1;
    else
	fname= path; 
    sprintf(buf, ".MAP/%s.map", fname);
    return fopen(buf, "w");
}

main(int argc, char **argv)
{
    FILE *fp, *map;
    for (int i= 1; i < argc; i++) {
	if (argv[i][0] != '-') {
	    if (!(map= MapFileName(argv[i]))) 
		return 0;
	    if ((fp= fopen(argv[i], "r"))) {
		Analyze(fp, map);
		fclose(fp);
		fclose(map);
	    }
	}
    }
    return 0;
}
