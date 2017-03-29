//$VObjectText,VObjectTextIter,VObjectMark$
#include "VObjectText.h"
#include "TextView.h"
#include "Mark.h" 

static TextChanges changeRec;

//------ VObjectMark -----------------------------------------------------------

MetaImpl(VObjectMark, (TP(gop), 0));

VObjectMark::VObjectMark(int p, int l, VObject *go, eMarkState s) 
						    : Mark(p, l, s, eMarkFixedSize)
{   
    gop= go; 
}

VObjectMark::~VObjectMark() 
{
    SafeDelete(gop);
}

ObjPtr VObjectMark::DeepClone()
{
    return new VObjectMark(pos,len,(VObject*)gop->DeepClone()); 
}

void VObjectMark::FreeAll()
{
    gop->FreeAll();
}

ostream &VObjectMark::PrintOn(ostream &s) 
{
    Mark::PrintOn(s);
    return s << gop SP << gop->contentRect SP;
}

istream &VObjectMark::ReadFrom(istream &s) 
{
    Rectangle r;
      
    Mark::ReadFrom(s);
    s >> gop >> r;
    gop->SetContentRect(r, FALSE);
    return s;
}

void VObjectMark::Parts(Collection *col)
{
    col->Add(gop);
}

//----- class VObjectText ------------------------------------------------------

MetaImpl(VObjectText, (TP(marks), TP(tv)));

VObjectText::VObjectText()
{
    Init();
}

VObjectText::VObjectText(int size, FontPtr fd) :StyledText(size, fd)
{
    Init();
}

VObjectText::VObjectText(byte *buf, int len, bool ic, FontPtr fd)
						       :StyledText(buf,len,ic,fd)
{
    Init();
}
    
VObjectText::VObjectText(class TextRunArray *st, byte *buf, int len, bool ic )
						      :StyledText(st,buf,len,ic)
{
    Init();
}

void VObjectText::Init()
{
    marks= new MarkList(TRUE);
    SetEscapeChar(cVObjectChar);
}

VObjectText::~VObjectText()
{
    if (marks) {
	marks->FreeAll();
	SafeDelete(marks);
    }
}

void VObjectText::InitNew()
{
    StyledText::InitNew();
    Init();
}

TextView *VObjectText::GetTextView()
{ 
    return tv; 
}

void VObjectText::ReplaceWithStr(byte *str,int len)
{
    if (marks)
	marks->FreeAll();
    StyledText::ReplaceWithStr(str, len);    
}

void VObjectText::Cut(int from,int to)
{
    DoDelayChanges dc(this);
    marks->Cut(from,to-from);
    StyledText::Cut(from,to);
}

void VObjectText::Paste(Text* t, int from, int to)
{
    DoDelayChanges dc(this);
    if (from != to)
	marks->Cut(from,to-from);
    marks->Paste(from,t->Size());
    if (t->IsKindOf(VObjectText)) { // paste the gobjects and their marks
	VObjectText *gt= (VObjectText *)t;
	register VObjectMark *mp, *nmp;
	
	Iter next(gt->marks);
	while (mp= (VObjectMark*)next()) {
	    nmp= (VObjectMark*)mp->DeepClone();
	    register VObject *gop= nmp->GetVObject();
	    gop->Open();
	    if (tv)
		gop->SetContainer(tv);
	    nmp->pos +=from;
	    marks->Add(nmp);
	}    
    }    
    StyledText::Paste(t,from,to);
}

void VObjectText::Copy(Text* save, int from, int to)
{
    if (!CheckRange(Size(),from,to) || save == 0)
	return;
    StyledText::Copy(save,from,to);

    if (!save->IsKindOf(VObjectText)) 
	return;

    VObjectText *gt= (VObjectText*) save; 
    Iter next(marks);
    register VObjectMark *mp, *nmp;
    
    gt->marks->FreeAll();
    while (mp= (VObjectMark*)next()) {
	if (mp->pos >= from && mp->pos+mp->len <= to) {
	    nmp= (VObjectMark*) mp->DeepClone();
	    nmp->pos= mp->pos - from;
	    gt->marks->Add(nmp);        
	}
    }    
}

TextPtr VObjectText::Save(int from, int to)
{
    if (!CheckRange(Size(),from,to))
	return 0;
	    
    VObjectText* t= new VObjectText(to-from);
    Copy(t, from, to);
    return t;
}

void VObjectText::Insert(byte c, int from,int to)
{
    DoDelayChanges dc(this);
    if (from != to)
	marks->Cut(from,to-from);
    marks->Paste(from,1);
    StyledText::Insert(c,from,to);
}

Command *VObjectText::InsertVObject(VObject *gop) 
{
    Token t;
    int from,to;
    VObjectMark *gm;
    Command *cmd;
    
    gop->Open();
    gop->CalcExtent();
    gop->SetContainer(tv);
    gop->SendDown(0, cPartEnableLayoutCntl, 0);
    tv->GetSelection(&from,&to);
    gm= new VObjectMark(from, 1, gop);
    gm->Lock();
    marks->Add(gm);
    cmd= tv->DoKeyCommand(cVObjectChar, gPoint0, t);
    gm->Unlock();
    return cmd;
}

void VObjectText::SetView(TextView *vp)
{
    Iter next(marks);
    register VObjectMark *m;
    
    if (!vp->IsKindOf(TextView))
	Error("SetView", "only accepts TextViews");
    tv= vp;
    while (m= (VObjectMark*) next())
	m->GetVObject()->SetContainer(vp);
}

VObject *VObjectText::MarkAt(int charNo)
{
    register Mark *mp;
    VObject *gop= 0;    
    Iter next(marks);

    while (mp = (Mark*) next()) {
	if (mp->pos == charNo && mp->len == 1 && mp->IsKindOf(VObjectMark)) {
	    gop= ((VObjectMark*)mp)->GetVObject();
	    break;
	}
    }    
    return gop;
}

VObjectMark *VObjectText::GetMarkAt(int charNo)
{
    register VObjectMark *mp= 0;
    
    Iter next(marks);
    while (mp= (VObjectMark*) next()) 
	if (mp->pos == charNo && mp->len == 1 && mp->IsKindOf(VObjectMark)) 
	    return mp;
    return mp;
}

VObject *VObjectText::ContainsPoint(Point p)
{
    register Mark *mp;
    VObject *gop= 0;
    Iter next(marks);

    while (mp= (Mark*) next()) 
	if (mp->IsKindOf(VObjectMark)) {
	    gop= ((VObjectMark*)mp)->GetVObject();
	    if (gop->ContainsPoint(p))
		break;
	}
    if (mp == 0)
	return 0;
    return gop;
}
    
int VObjectText::FindPositionOfVObject(VObject *gop1)
{
    register Mark *mp;
    int pos= -1;
    VObject *gop= 0;
    Iter next(marks);

    while (mp= (Mark*)next()) {
	if (mp->IsKindOf(VObjectMark)) {
	    gop= ((VObjectMark*)mp)->GetVObject();
	    if (gop == gop1)
		return mp->Pos();
	}
    }
    return pos;
}

bool VObjectText::IsEscape (int charNo)
{
    return IsVObjectChar(charNo) && MarkAt(charNo);
}

int VObjectText::CalcEscape (int i, LineDesc* ld)
{
    VObject *gop= MarkAt(i);
    if (!gop)
	Error("CalcEscape", "no Escape");

    if (ld)
	ld->Max(gop->Base(), gop->Height());
    return gop->Width();    
}

void VObjectText::DrawEscape (int i,Rectangle clip)
{
    FontPtr oldFont;
    int line;
    Point p, bp, tp;

    VObjectMark *mp= GetMarkAt(i);
    if (!mp)
	Error("DrawEscape", "no Escape at %d (%c)", i, (*this)[i]);
    VObject *gop= mp->GetVObject();
	
    oldFont= GrGetFont();
    if (mp->IsInvalidated()) {  // reposition invalidated VObject
	tv->CharToPos(i, &line, &p, FALSE);
	bp= tv->LineToPoint(line, TRUE, FALSE); 
	p.y= bp.y - gop->Base();
	gop->SetOrigin(p);
	mp->Validate();
    }
    tp= GrGetTextPos();
    gop->DrawAll(clip, FALSE);
    tp.x+= gop->Width();
    GrTextMoveto(tp);
    GrSetFont(oldFont);
}

void VObjectText::VObjectChangedSize(VObject *vop)
{
    int at= FindPositionOfVObject(vop);
    if (at == -1)
	return; //Error("VObjectChangedSize", "vobject not found");
    Send(cIdNone, eTextReplaced, changeRec(at, at+1, 1));     
}

Iterator *VObjectText::VObjectIterator()
{
    return marks->MakeIterator();
}
    
int VObjectText::VObjectCount()
{
    return marks->Size();
}
    
ostream &VObjectText::PrintOn(ostream &s) 
{
    StyledText::PrintOn(s);
    return s << marks SP;
}

istream &VObjectText::ReadFrom(istream &s) 
{
    StyledText::ReadFrom(s);
    s >> marks;
    return s;
}

void VObjectText::Parts(Collection *col)
{
    StyledText::Parts(col);
    col->Add(marks);
}
