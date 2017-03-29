//$FileType$
#include "FileType.h"
#include "System.h"
#include "String.h"
#include "Error.h"

const char *cMagic              =   "@!";
const char *cDocTypeUndef       =   "UNDEF",  // ???
	   *cDocTypeAscii       =   "ASCII", 
	   *cDocTypePict        =   "PICT",
	   *cDocTypePostScript  =   "POSTSCRIPT",
	   *cDocTypeDirectory   =   "SYSDIRECTORY", 
	   *cDocTypeSpecial     =   "SYSSPECIAL",
	   *cDocSunRasterFile   =   "SUN_RASTERFILE";
const char *cDocSunRasterFileAscii= "SUN_RASTERFILE_ASCII",
	   *cDocTypeDitroff     =   "DITROFF",
	   *cDocTypeET          =   "ET++",
	   *cDocTypeBitmap      =   "ET++Bitmap";

char *cDocCreatorUndef    =   "__UNKNOWN__";

const int SUN_RAS_MAGIC   =   0x59a66a95;   // wuerg !!!

FileType::FileType(char *path, bool shallow)
{
    pathname= strsave(path);
    typename= creator= 0;
    isAscii= isETFormat= isSystem= isDeep= FALSE;
    isShallow= shallow;
}

FileType::~FileType()
{
    SafeDelete(pathname);
    SafeDelete(creator);
    if (isETFormat && typename) {
	delete (void*) typename;
	typename= 0;
    }
}

long FileType::SizeHint()
{
    return -1;
}

void FileType::WhatType()
{
    if (SystemFileType()) {
	isSystem= TRUE;
	return;
    }

    if (isShallow) {
	typename= cDocTypeUndef;
	return;
    }  
    DeepenShallowType();
}

void FileType::DeepenShallowType()
{
    char fbuf[200], *bp, ch;
    int l= strlen(cMagic);

    if (isDeep || isSystem)
	return;
    isDeep= TRUE;
    
    istream from(pathname);
    if (! from) {
	typename= cDocCreatorUndef;
	return;
    }
	
    for (bp= fbuf; bp < fbuf + sizeof(fbuf)-1; bp++) {
	if (!from.get(ch))
	    break;
	if (! Isascii(ch)) {
	    isAscii= FALSE;
	    if (NotAsciiFileType())
		return;
	    typename= cDocTypeUndef;
	    return;
	}
	if (ch == '\n')
	    ch= '\0';
	*bp= ch;
    } 
    *bp= '\0';       
    isAscii= TRUE;
    typename= cDocTypeAscii;

    if (strncmp(fbuf, cMagic, l) == 0) { // ET++ file
	char bcreator[100], btype[100];
	int d= sscanf(fbuf, "@!%s %s", btype, bcreator);   
	if (d == 1)
	    strreplace(&creator, cDocCreatorUndef);
	else
	    strreplace(&creator, bcreator);                 
	isETFormat= TRUE;
	typename= strsave(btype);
	return;
    }
    if (strncmp(fbuf, "%!", 2) == 0) {
	typename= cDocTypePostScript;
	return;
    }
    if (strncmp(fbuf, "x T ", 4) == 0) {
	typename= cDocTypeDitroff;
	return;
    }
    if (strncmp(fbuf, "/* Format_version=1", 19) == 0
					|| strncmp(fbuf, "#define", 7) == 0) {
	typename= cDocSunRasterFileAscii;
	return;
    }
}

bool FileType::NotAsciiFileType()
{
    istream fp(PathName());
    
    if (fp) {
	int magic;
	fp.read((u_char*)&magic, sizeof(int));
	if (SUN_RAS_MAGIC == magic) {
	    typename= cDocSunRasterFile;
	    return TRUE;
	}
    }   
    return FALSE;
}

char *FileType::Creator()
{
    return creator;
}

int FileType::UniqueId()
{
    AbstractMethod("FileType::UniqueId");
    return 0;
}

char *FileType::PathName()
{
    return pathname;
}

char *FileType::FileName()
{
    return PathName();
}

const char *FileType::Type()
{
    if (!typename)
	WhatType();
    return typename;
}

bool FileType::IsCCode()
{
    if (isDeep && !IsAscii())
	return FALSE;
	
    char *p;
    for (p= pathname+strlen(pathname)-1; p >= pathname && *p != '.'; p--)
	;
    p++;
    if (p < pathname || strlen(p) != 1)
	return FALSE;
    return (bool) (index("CcHh", *p) != 0);
}

bool FileType::IsAscii()
{
    if (!typename)
	WhatType();
    return isAscii;
}

bool FileType::IsSystem()
{
    if (!typename)
	WhatType();
    return isSystem;
}

bool FileType::IsExecutable()
{
    if (!typename)
	WhatType();
    return FALSE;
}

bool FileType::IsETFormat()
{
    if (!typename)
	WhatType();
    return isETFormat;    
}
    
bool FileType::SystemFileType()
{
    return FALSE;
}

//---- class FType -------------------------------------------------------

FType::FType(char *p)
{
    ft= gSystem->GetFileType(p);
}
