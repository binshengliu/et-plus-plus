//$Ref$

#include "Reference.h"
#include "ObjectTable.h"
#include "System.h" 
#include "String.h"

//---- reference --------------------------------------------------------------

MetaImpl(Ref, (T(offset),T(offset), T(type), T(len), TP(cla), 0));

Ref::Ref()
{
    Init(0, 0, 0, -1, 0);
}

Ref::Ref(void *b, int t, int o, int l= -1, Class *c= 0)
{
    Init(b, t, o, l, c);
}

Ref::Ref(Object &op)
{
    Init(&op, 0, 0, -1, op.IsA());
}

Ref::Ref(Object *&op)
{
    Init(op, T_PTR, 0, -1, op->IsA());
}

Ref::Ref(Ref &ref)
{
    Init(ref.base, ref.type, ref.offset, ref.len, ref.cla);
}

void Ref::Init(void *b, int t, int o, int l, Class *c)
{
    base= b;
    type= t;
    offset= o;
    len= l;
    cla= c;
}

void Ref::operator=(Ref r)
{
    Init(r.base, r.type, r.offset, r.len, r.cla);
}

bool Ref::IsEqual(Object *op)
{
    if (op->IsKindOf(Ref)) {
	Ref *rp= (Ref*)op;
	return (rp->base == base) && (rp->offset == offset);
    }
    return FALSE;
}

bool Ref::IsObject()
{
    if (cla && cla->TestFlag(eClassObject) && (type == 0)) {
	if (ObjectTable::PtrIsValid((Object*)base))
	    return TRUE;
    }
    return FALSE;
}

Object *Ref::GetObject()
{
    return IsObject() ? (Object*) base : 0;
}

static void GetLength(void *p1, void *p2, void*, void*)
{
    *(int*)p1= *(int*)p2;
}

int Ref::Length(bool &ok)
{
    ok= TRUE;
    if (base) {
	if (type & T_VEC) {
	    int l= 0, *lp= AddrOfLen();
	    
	    if (gSystem->Call(GetLength, (void*) &l, (void*) lp, 0, 0)) {
		ok= FALSE;
		return 0;
	    }
	    return l;
	}
	if (type & T_ARR)
	    return len;
    }
    return -1;
}

char *Ref::TypeName()
{
    return cla ? cla->Name() : "?Type?";
}

char *Ref::PreName()
{
    if ((type & T_PTR) && (type & T_PTR2))
	return "**";
    if ((type & T_PTR) || (type & T_PTR2))
	return " *";
    return "  ";
}

char *Ref::PostName()
{
    bool ok= TRUE;
    int l= Length(ok);
    
    if (l >= 0) {
	if (ok)
	    return form("[%d]", l);
	return "[<ill addr>]";
    }
    return "";
}

static void GetStrValue(void *, void *vbp, void *addr, void *len)
{    
    int l= (int)len, ll;
    char *s, *bp= (char*)vbp;
    if (l == -1)
	l= strlen(*(char**) addr);
    s= *(char**) addr;
    ll= min(l, 50);
    *bp++= '\"';  
    for (int j=0; j<ll; j++)
	bp= strquotechar(*s++, bp);
    *bp++= '\"';
    if (l > ll) {
	*bp++= '.'; *bp++= '.'; *bp++= '.';
    }
    *bp= '\0';
}

static void GetValue(void *cl, void *bp, void *addr, void *)
{    
    ((Class*)cl)->Show((char*)bp, addr);
}

static void GetAddrValue(void *cl, void *vbp, void *addr, void*)
{    
    char *bp= (char*)vbp;
    if (ObjectTable::PtrIsValid((Object*)addr) &&
			((Object*)addr)->IsA() != (Class*)cl) 
	strcpy(bp, form("0x%08x <%s>", addr, ((Object*)addr)->ClassName()));
    else
	strcpy(bp, form("0x%08x", addr));
}

char *Ref::Value()
{
    static char buf[400];
    void *addr= Addr();
    void *val;
    bool ok;
    
    //---- prefetch contents of a string
    
    if ((type == T_PTR || type == T_PTR2) && strismember(cla->Name(), "char", "byte", 0)) {
	val= *((void**) addr);
	if (val == 0)
	    return "<nil>";
	if (gSystem->Call(GetStrValue, (void*) cla, (void*) buf, addr, (void*)-1))
	    return "<illegal address>";
	return buf;
    }
    if (type == T_VEC && strismember(cla->Name(), "char", "byte", 0)) {
	val= *((void**) addr);
	if (val == 0)
	    return "<nil>";
	if (gSystem->Call(GetStrValue, (void*) cla, (void*) buf, addr, (void*)Length(ok)))
	    return "<illegal address>";
	return buf;
    }
    
    //---- end prefetch of strings
    
    if (type & (T_PTR | T_VEC)) {
	val= *((void**) addr);
	if (val == 0)
	    return "<nil>";
	if (gSystem->Call(GetAddrValue, (void*) cla, (void*) buf, val, 0))
	    return form("0x%08x", val);
	return buf;
    }
    if (type & T_ARR) {
	return form("0x%08x", addr);
    }
    if (type & (T_PTR2)) {
	val= *((void**) addr);
	if (val == 0)
	    return "<nil>";
	if (gSystem->Call(GetAddrValue, (void*) cla, (void*) buf, val, 0))
	    return form("0x%08x", val);
	return buf;
    }
    if (type == 0) {
	if (gSystem->Call(GetValue, (void*) cla, (void*) buf, addr, 0))
	    return "<illegal address>";
	return buf;
    }
    return "?Value?";
}

static void GetRef(void *p1, void*, void*, void*)
{
    char *dummy= *(char**)p1;
}

Ref *Ref::Deref()
{
    bool ok;
    void *oo;
    char *name= "?Deref?";
    if (type & T_VEC) { // dynamic array with var length
	if (gSystem->Call(GetRef, Addr(), 0, 0, 0))
	    return 0;
	oo= *(void**)Addr();
	if (oo == 0)
	    return 0;
	return new Ref(oo, type & ~T_PTR, 0, Length(ok), cla);
    }
    if (type & T_ARR) { // inline array of simple
	if (gSystem->Call(GetRef, Addr(), 0, 0, 0))
	    return 0;
	oo= Addr();
	if (oo == 0)
	    return 0;
	return new Ref(oo, type, 0, Length(ok), cla);
    }
    if (type & T_PTR) {  // ptr to simple  
	if (gSystem->Call(GetRef, Addr(), 0, 0, 0))
	    return 0;
	oo= *((void**)Addr());
	if (oo == 0)
	    return 0;
	return new Ref(oo, type & ~T_PTR, 0, -1, cla);
    }
    if (type & T_PTR2) {  // 
	if (gSystem->Call(GetRef, Addr(), 0, 0, 0))
	    return 0;
	oo= *((void**)Addr());
	if (oo == 0)
	    return 0;
	return new Ref(oo, type & ~T_PTR2, 0, -1, cla);
    }
    else { // simple
	oo= Addr();
	return new Ref(oo, type, 0, -1, cla);
    }
}

void Ref::Dump()
{
    cerr.form("%s %sname%s %s 0x%x 0x%x\n", TypeName(), PreName(), 
		  PostName(), Value(), (unsigned)Addr(), (unsigned)GetBase());
}

