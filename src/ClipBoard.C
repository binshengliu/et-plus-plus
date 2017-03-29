//$ClipBoard$
#include "ClipBoard.h"
#include "View.h"
#include "IO/membuf.h"
#include "ClassManager.h"
#include "FileType.h"

//---- ClipBoard ---------------------------------------------------------------

ClipBoard::ClipBoard()
{
    type= (char*) cDocTypeUndef;
}

ClipBoard::~ClipBoard()
{
    SafeDelete(mb);
}

bool ClipBoard::CanPaste(class View *v)
{
    char *t= GetType();
    if (v && t)
	return v->CanPaste(t);
    return FALSE;
}

void ClipBoard::SetType(char *t)
{
    type= t;
}

char *ClipBoard::GetType()
{
    if (! owner)
	type= DevGetType();
    return type;
}

char *ClipBoard::DevGetType()
{
    return (char*) cDocTypeUndef;
}

void ClipBoard::SelectionToClipboard(class View *v)
{
    extern bool gInPrintOn;
    
    if (v == 0)
	return;
    
    if (mb)
	delete mb;
    mb= new membuf;
    ostream os(mb);
    owner= TRUE;
    gClassManager->Reset();
    gInPrintOn= TRUE;
    char *t= GetType();
    v->SelectionToClipboard(t, os);
    os.flush();
    gInPrintOn= FALSE;
    gClassManager->Reset();
    ScratchChanged(t ? t : cDocTypeUndef);
}

Command *ClipBoard::PasteClipboard(class View *v)
{   
    Command *cmd= gNoChanges;
    if (v) {
	if (! owner) {
	    // get clipboard from server
	    mb= MakeBuf(type);
	    owner= TRUE;
	}
	if (mb) {
	    istream is(mb);
	    gClassManager->Reset();
	    cmd= v->PasteData(GetType(), is);
	    is.rewind();
	    gClassManager->Reset();
	}
    }
    return cmd;
}

membuf *ClipBoard::MakeBuf(char*)
{
    cerr << "no clipboard server\n";
    return 0;
}

void ClipBoard::NotOwner(char *t)
{
    if (t)
	SetType(t);
    owner= FALSE;
    SafeDelete(mb);
}

char *ClipBoard::GetBuf(u_long *size)
{
    if (mb) {
	*size= mb->tell(TRUE);
	return mb->Base();
    }
    *size= 0;
    return 0;
}

void ClipBoard::ScratchChanged(char*)
{
    static bool first= TRUE;
    if (first) {
	cerr << "no clipboard server\n";
	first= FALSE;
    }
}

