//$PathLookup, PathIter$

#include "ByteArray.h"
#include "PathLookup.h"
#include "OrdColl.h"
#include "System.h"
#include "String.h"

//---- PathLookup ----------------------------------------------------------

MetaImpl(PathLookup, (TP(path), TP(paths), 0));

PathLookup::PathLookup(char *p)
{
    path= strsave(p);
    paths= 0;
    Scan();
}

PathLookup::~PathLookup()
{
    SafeDelete(path);
    if (paths)
	paths->FreeAll();
    SafeDelete(paths);
}

bool PathLookup::Lookup(const char *name, char *buf)
{
    // try current working directory
    if (gSystem->AccessPathName((char*)name, 4) == 0) {
	strcpy(buf, name);
	return TRUE;
    }
    if (!paths)
	return FALSE;
    Iter next(paths);
    ByteArray *bp;
    while (bp= (ByteArray*)next()) {
	sprintf(buf, "%s/%s", bp->Str(), name);
	if (gSystem->AccessPathName((char*)buf, 4) == 0)
	    return TRUE;
    }
    return FALSE;    
}

void PathLookup::Scan()
{
    char *newpath= path;
    
    paths= new OrdCollection(4); 
    if (!newpath)
	return;
    char *p, *q, buf[200];
    for (p= newpath, q= buf;  ; ) {
	if (*p == ':' || *p == '\0') {
	    *q= '\0';
	    paths->Add(new ByteArray((byte*)buf, -1));
	    q= buf;
	    if (*p == '\0')
		break;
	    p++;
	} else
	    *q++= *p++;
    } 
}

void PathLookup::Add(char *p)
{
    ByteArray *b= new ByteArray((byte*)p, -1);
    if (!paths->Contains(b))
	paths->Add(b);
    else
	SafeDelete(b);
}

//---- class PathIter --------------------------------------------------------

PathIter::PathIter(PathLookup *p)
{
    ip= p->paths->MakeIterator();    
}

PathIter::~PathIter()
{
    SafeDelete(ip);
}

char *PathIter::operator()()
{
    ByteArray *bp= (ByteArray*)ip->operator()();
    if (bp)
	return (char*) bp->Str();
    return 0;
}
