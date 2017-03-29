#ifndef FileType_First
#ifdef __GNUG__
#pragma once
#endif
#define FileType_First

#include "Root.h"
#include "Types.h"

//---- file types --------------------------------------------------------------

extern const char *cMagic,             // magic cookie of ET++ generated files
		  *cDocTypeUndef,      // "UNDEF" document type
		  *cDocTypePict,       // ET++ "PICT" document type
		  *cDocTypeBitmap,     // ET++ Bitmap document type
		  *cDocTypeAscii,      // document type for ordinary ascii-files
		  *cDocTypePostScript, // PostScript document type
		  *cDocTypeDirectory,  // System directory
		  *cDocTypeSpecial,    // System special file
		  *cDocSunRasterFile,  // Sun raster file format
		  *cDocSunRasterFileAscii, // Sun c raster file format
		  *cDocTypeBitmap,     // ET++ Bitmap format
		  *cDocTypeDitroff,    // ditroff file format
		  *cDocTypeET;         // ET++ file format

extern char       *cDocCreatorUndef;   // Creator of document unknown

//---- abstract class FileType -------------------------------------------------

class FileType : public Root {
protected:
    char *pathname, *creator;
    const char *typename;
    bool isSystem, isAscii, isETFormat, isShallow, isDeep;

    
    void WhatType();
    virtual bool SystemFileType();
    virtual bool NotAsciiFileType();
public:
    FileType(char *pathName, bool shallow= FALSE); 
		// shallow indicates that the contents if the file is not
		// considered for determining a file type
				
    virtual ~FileType();
    virtual const char *Type();
    virtual char *Creator();  // returns 0 if not an ET++ created file
    virtual long SizeHint();  // -1 indicates no size hint
    virtual char *FileName();
    virtual int  UniqueId();
    virtual bool IsCCode();
    virtual bool IsAscii();
    virtual bool IsSystem();
    virtual bool IsExecutable();
    virtual bool IsETFormat();
    virtual void DeepenShallowType(); 
    char *PathName();
};

//---- class FType -----------------------------------------------------------
// some syntactic sugar -> uses destructors to free a dynamically allocated
// instance of FileType

class FType {
    class FileType *ft;
public:
    FType(char *pathName);
    ~FType()
	{ SafeDelete(ft); }
    const char *Type()
	{ return ft->Type(); }
    FileType *FileType()
	{ return ft; }
    char *Creator()
	{ return ft->Creator(); }
    long SizeHint()
	{ return ft->SizeHint(); }
    char *FileName()
	{ return ft->FileName(); }
    int UniqueId()
	{ return ft->UniqueId(); }
    bool IsCCode()
	{ return ft->IsCCode(); }
    bool IsAscii()
	{ return ft->IsAscii(); }
    bool IsSystem()
	{ return ft->IsSystem(); }
    bool IsExecutable()
	{ return ft->IsExecutable(); }
    bool IsETFormat()
	{ return ft->IsETFormat(); }
    char *PathName()
	{ return ft->PathName(); }
};

#endif FileType_First

