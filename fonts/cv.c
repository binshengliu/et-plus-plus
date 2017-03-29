#include <stdio.h>


struct map {
    char *n1, *n2;
} Map[]= {
    "helvetica",    "helvet",
    "courier",      "courie",
    "chicago",      "chicag",
    "avant_garde",  "avgard",
    "bookman",      "bookmn",
    "century_schlbk", "ceslbk",
    "n_helvetica",  "nhelve",
    "palatino",     "palati",
    "zapf_chancery", "zchanc",
    "zapf_dingbats", "zdingb",
    "los_angeles",  "losang",
    "new_york",     "newyrk",
    "san_francisco", "sanfra",
    0, 0
};

char *lookup(name)
char *name;
{
    int i;
    
    for (i= 0; Map[i].n1; i++)
	if (strcmp(Map[i].n1, name) == 0)
	    return Map[i].n2;
    return name;
}

main(argc, argv)
int argc;
char *argv[];
{
    char *cp, buf[100], name[20], face[10];
    int i, size;
    
    for (i= 1; i < argc; i++) {
	strcpy(buf, argv[i]);
	for (cp= buf; *cp; cp++)
	    if (*cp == '.')
		*cp= ' ';
	if (sscanf(buf, "%s %s %d", name, face, &size) == 3) {
	    sprintf(buf, "%s%02d%s.bdf", lookup(name), size, face);
	    fprintf(stderr, "<%s> -> <%s>\n", argv[i], buf);
	    rename(argv[i], buf);
	    
	}
    }
}

