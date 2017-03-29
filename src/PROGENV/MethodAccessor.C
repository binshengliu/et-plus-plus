//$PeMethodAccessor,PeMapMethodAccessor, PeScanMethodAccessor, MethodScanner$

#include "MethodAccessor.h"
#include "OrdColl.h"
#include "PathLookup.h"
#include "String.h"
#include "System.h"
#include "CheapText.h"
#include "CodeTextView.h"
 
#include "EtPeDoc.h"

const int cMaxName= 400;

//---- PeMethodAccessor ----------------------------------------------------

PeMethodAccessor::PeMethodAccessor()
{
}

class Collection *PeMethodAccessor::FindMethods(Class *, PathLookup *)
{
    AbstractMethod("FindMethods");
    return 0;
}

//---- PeMapMethodAccessor ----------------------------------------------------

static char *dtor= "_dtor",
	    *ctor= "_ctor";

PeMapMethodAccessor::PeMapMethodAccessor()
{
    mapSrcPath= 0;    
}

void PeMapMethodAccessor::CreateMapSearchPath(PathLookup *universe)
{
    char path[2000];
    char *p;

    strcpy(path, ".MAP");
    PathIter next(universe);
    while (p= next()) 
	strcat(path, form(":%s/.MAP:%s", p, p));
    mapSrcPath= new PathLookup(path);
}

char *PeMapMethodAccessor::GetMapFileName(Class *cl, PathLookup *universe)
{
    char mapname[cMaxName], fname[cMaxName], *bp;
    static char mappathname[2*cMaxName];
    
    const char *path, *base;
    if (mapSrcPath == 0) 
	CreateMapSearchPath(universe);
    
    fname[0]= mapname[0]= '\0';
    base= cl->GetImplFileName();
    path= ""; /* cl->GetCompDir(); */
    bp= rindex((char*)base, '/');
    if (bp == 0)
	bp= (char*)base;
    sprintf(mapname, "%s.map", bp);
    sprintf(mappathname, "%s/%s", path, mapname); 
    if(gSystem->AccessPathName(mappathname, 4) != 0) {
	sprintf(mappathname, "%s/.MAP/%s", path, mapname); 
	if(gSystem->AccessPathName(mappathname, 4) != 0)
	    if (!mapSrcPath->Lookup(mapname, mappathname)) 
		return 0;
    } 
    return mappathname; 
}

Collection *PeMapMethodAccessor::FindMethods(Class *cl, PathLookup *universe)
{
    char classname[cMaxName], methodname[cMaxName], clname[cMaxName], *name;
    int line, pub;
    OrdCollection *oc= new OrdCollection;
    char *mapfile= GetMapFileName(cl, universe);
    if (mapfile == 0)
	return (Collection*) oc;
    istream fp(mapfile);
    
    if (fp) {
	// assumption: method is translated to _class_method
	sprintf(clname, "_%s_", cl->Name());
	while (fp >> classname >> methodname >> line >> pub) {
	    if (strncmp(methodname, clname, strlen(clname)) == 0) {
		// change _ctor and & _dtor
		name= methodname+strlen(clname);
		if (strncmp(name, dtor, strlen(dtor)) == 0)
		    name= form("~%s", cl->Name());
		else if (strncmp(name, ctor, strlen(ctor)) == 0)
		    name= form("+%s(%s)", cl->Name(), name+strlen(ctor));
		oc->Add(new MethodReference(line, cl, name, pub == 2));
	    }
	}
    }
    return (Collection *)oc; // ??? cast to keep cfront 1.2 happy 
}

//---- MethodScanner -----------------------------------------------------------

class MethodScanner: public CodeAnalyzer {
    Collection *cp;
    char *cname;
    Class *clp;
protected:
    void Function(int line, int start, int end, char *name, char *classname);
public:
    MethodScanner(Text *t, Collection *cp, Class *clp);
};

MethodScanner::MethodScanner(Text *t, Collection *c, Class *classp)
							    : CodeAnalyzer(t)
{
    cp= c; 
    clp= classp;
    cname= clp->Name();
}

void MethodScanner::Function(int line, int, int, char *name, char *clname)
{
    if (clname && strcmp(clname, cname) == 0) 
	cp->Add(new MethodReference(line, clp, name, TRUE));
}

//---- PeScanMethodAccessor ----------------------------------------------------

PeScanMethodAccessor::PeScanMethodAccessor()
{
}

Collection *PeScanMethodAccessor::FindMethods(Class *cl, PathLookup *)
{
    OrdCollection *oc= new OrdCollection;
    char filename[1000];
    if (!gEtPeDoc->FileOfClass(cl, filename, FALSE))
	return 0;
    istream fp(filename);
    CheapText text(5000);
    text.ReadFromAsPureText(fp);  
    MethodScanner ms(&text, oc, cl);
    ms.Doit();
    return (Collection*)oc; // ??? cast to keep cfront 1.2 happy
}
