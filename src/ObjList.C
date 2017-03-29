//$ObjLink,ObjList,ObjListIter,RevObjListIter,ObjLink$
#include "ObjList.h"
#include "FixedStorage.h"
#include "System.h"
#include "Storage.h"

//---- ObjLink -----------------------------------------------------------------

MetaImpl(ObjLink, (TP(next), TP(previous), TP(op), 0));

ObjLink::ObjLink(ObjPtr a, ObjLink* n, ObjLink* p)
{ 
    op= a; 
    next= n; 
    previous= p; 
}

ObjLink::ObjLink(ObjPtr a, ObjLink *l)
{
    op= a;
    next= l;
    previous= l->previous;
    l->previous->next= this;
    l->previous= this;
}

ObjLink::~ObjLink()
{
}

void ObjLink::FreeAll()
{
    if (op) {
	op->FreeAll();
	SafeDelete(op);
    }
}

void *ObjLink::operator new(size_t sz)
{
    return MemPools::Alloc(sz);
}

void ObjLink::operator delete(void *vp)
{
    MemPools::Free(vp, sizeof(ObjLink)); 
}

//---- ObjList -----------------------------------------------------------------

MetaImpl(ObjList, (TP(first), TP(last), 0));

ObjList::ObjList()
{
    first= last= 0;
}

ObjList::~ObjList()
{
    register ObjLink *l, *ll;

    for(l= first; l; l= ll) {
	ll= l->next;
	SafeDelete(l);
    }
    first= last= 0;
}

void ObjList::FreeAll()
{
    register ObjLink *l, *ll;

    for (l= first; l; l= ll) {
	ll= l->next;
	l->FreeAll();
	SafeDelete(l);
    }
    first= last= 0;
    size= 0;
}

void ObjList::RemoveDeleted()
{
    register ObjLink *l, *p;

    l= first;
    while (l) {
	p= l;
	l= l->next;
	if (p->op->IsDeleted()) {
	    SafeDelete(p->op);
	    RemoveLink(p);
	}
    }
}

void ObjList::Insert(ObjPtr a)
{
    if (CheckNotNull("Insert", a))
	return;
    CheckActiveIter("Insert");
    first= new ObjLink(a, first, 0);
    if (last == 0)
	last= first;
    else 
	first->next->previous= first;
    size++;
    Changed();
}

void ObjList::InsertObjLink(ObjPtr op, ObjLink *l)
{
    new ObjLink(op, l);
    size++;
    Changed();
}

ObjLink *ObjList::FindObjLink(ObjPtr op, bool byPtr)
{
    register ObjLink *l;
    for(l= first; l; l= l->next) {
	if (l->op->IsDeleted())
	    continue;
	if ((byPtr && l->op == op) || (!byPtr && l->op->IsEqual(op))) 
	    return l;
    }
    return 0;
}

void ObjList::PrivInsertBefore(ObjPtr before, ObjPtr op, bool byPtr, char *name)
{
    register ObjLink *l;

    if (CheckNotNull(name, op))
	return;
    CheckActiveIter(name);
    l= FindObjLink(before, byPtr);
    if (l != 0) 
       if (l == first)
	   Insert(op);
       else 
	   InsertObjLink(op, l);
    else
	Error(name, "object not found");
}

void ObjList::InsertBefore(ObjPtr before, ObjPtr op)
{
    PrivInsertBefore(before, op, FALSE, "InsertBefore");
}

void ObjList::InsertBeforePtr(ObjPtr before, ObjPtr op)
{
    PrivInsertBefore(before, op, TRUE, "InsertBeforePtr");
}

void ObjList::PrivInsertAfter(ObjPtr after, ObjPtr op, bool byPtr, char *name)
{
    register ObjLink *l;

    if (CheckNotNull(name, op))
	return;
    CheckActiveIter(name);
    l= FindObjLink(after, byPtr);
    if (l != 0) {
	l= l->next;
	if (l == 0)
	    Add(op); 
	else 
	    InsertObjLink(op, l);
    }
    else
	Error (name, "object not found");
}

void ObjList::InsertAfter(ObjPtr after, ObjPtr op)
{
    PrivInsertAfter(after, op, FALSE, "InsertAfter");
}

void ObjList::InsertAfterPtr(ObjPtr after, ObjPtr op)
{
    PrivInsertAfter(after, op, TRUE, "InsertAfterPtr");
}

ObjPtr ObjList::Add(ObjPtr a)
{
    if (CheckNotNull("Add", a))
	return 0;
    CheckActiveIter("Add");
    last= new ObjLink(a, 0, last);
    if (first == 0)
	first= last;
    else
	last->previous->next= last;
    size++;
    Changed();
    return 0;
}

ObjPtr ObjList::Replace(ObjPtr a, ObjPtr b)
{
    register ObjLink *l;
    ObjPtr t;

    if (CheckNotNull("Replace", b))
	return 0;
    l= FindObjLink(a, TRUE);
    if (l) {
	    t= l->op;
	    l->op= b;
	    Changed();
	    return t;
    }
    return 0;
}

ObjPtr ObjList::First()
{
    if (Size() == 0)
	return 0;
    return GetAt(0);
}

ObjPtr ObjList::Last()
{
    register ObjLink *l;

    if (Size() == 0)
	return 0;
    for(l= last; l; l= l->previous) 
	if (!l->op->IsDeleted()) 
	    return l->op;
    return 0;
}

ObjPtr ObjList::Remove(ObjPtr a)
{   
    register ObjLink *l;
    register ObjPtr op= 0;

    if (a == 0)
	return 0;
    for(l= first; l; l= l->next) {
	if (l->op->IsEqual(a)) {
	    op= l->op;
	    RemoveLink(l);
	    break;
	}
    }
    return op;
}

ObjPtr ObjList::RemovePtr(ObjPtr a)
{   
    register ObjLink *l;
    register ObjPtr op= 0;

    for(l= first; l; l= l->next) {
	if (l->op == a) {
	    op= l->op;
	    RemoveLink(l);
	    break;
	}
    }
    return op;
}

void ObjList::RemoveLink(ObjLink *l)
{
    if (InIterator()) {
	l->op= new DeletedObject;
	AnnounceRemove();
    }
    else {
	if (l->next)
	    l->next->previous= l->previous;
	else
	    last= l->previous;
	if (l->previous)
	    l->previous->next= l->next;
	else
	    first= l->next;
	delete l;
	size--;
    }
    Changed();
}

ObjPtr ObjList::GetAt(int index)
{
    register ObjLink *l;
    register int i= 0;

    for(i= 0, l= first; l; l= l->next) {
	if (!l->op->IsDeleted()) {
	    if (i == index)
		return l->op;
	    i++;
	}
    }
    return 0;
}

ObjPtr ObjList::Before(ObjPtr before)
{
    register ObjLink *l;

    for (l = first; l; l= l->next) {
	if (l->op->IsEqual(before)) {
	    l = l->previous;
	    if (l == 0)
		return 0;
	    return l->op;
	}
    }    
    if (l == 0)
	Error ("Before", "object not found");
    return 0;
}

ObjPtr ObjList::After(ObjPtr after)
{
    register ObjLink *l;

    for (l = first; l; l= l->next) {
	if (l->op->IsEqual(after)) {
	    l= l->next;
	    if (l == 0) 
		return 0;
	    return l->op;
	}
    }
    if (l == 0)
	Error ("After", "object not found");
    return 0;
}

void ObjList::Empty(int)
{
    CheckActiveIter("Empty");
    RemoveAll(this);
}

ObjPtr ObjList::At(int i)
{
    return GetAt(i);
}

Iterator *ObjList::MakeIterator() 
{ 
    return new ObjListIter(this); 
}

Iterator *ObjList::MakeReversedIterator () 
{ 
    return new RevObjListIter(this); 
}

//---- ObjListIter -------------------------------------------------------------

ObjListIter::ObjListIter(Collection *s)
{
    cs= (ObjList*)s; 
    ce= cs ? cs->first : 0; 
}

ObjListIter::~ObjListIter()
{
    IteratorEnd();
}

void ObjListIter::Reset(Collection *s)
{
    if (s == 0)
	s= Coll();
    cs= (ObjList*)s;
    ce= cs ? cs->first : 0;
    Iterator::Reset(s);
}

Collection *ObjListIter::Coll()
{
    return cs;
}

Object* ObjListIter::operator()()
{
    Object *op;

    if (cs == 0)
	return 0;
    IteratorStart();

    while (ce && ce->op->IsDeleted())
	ce= ce->next;

    if (ce) {
	op= ce->op;
	ce= ce->next;
	return op;
    }
    ce= cs->first;
    IteratorEnd();
    return 0;
}

void *ObjListIter::operator new(size_t sz)
{
    return MemPools::Alloc(sz);
}

void ObjListIter::operator delete(void *vp)
{
    MemPools::Free(vp, sizeof(ObjListIter)); 
}

//---- RevObjListIter ----------------------------------------------------------

RevObjListIter::RevObjListIter(ObjList *s) : ObjListIter(s)
{ 
    cs= (ObjList*)s; 
    ce= cs ? cs->last : 0;
}

void RevObjListIter::Reset(Collection *s)
{
    ObjListIter::Reset(s);
    ce= cs ? cs->last : 0;
}

Object* RevObjListIter::operator()()
{
    Object *op;

    if (cs == 0)
	return 0;
    IteratorStart();

    while (ce && ce->op->IsDeleted())
	ce= ce->previous;

    if (ce) {
	op= ce->op;
	ce= ce->previous;
	return op;
    }
    ce= cs->last;
    IteratorEnd();
    return 0;
}

