//$OrdCollection,OrdCollectionIter,RevOrdCollectionIter$
#include "OrdColl.h"
#include "Error.h"
#include "FixedStorage.h"

const int cOrdColShrinkFactor = 2,
	  cMinExpand = 8;

//---- ordered collection ------------------------------------------------------

MetaImpl(OrdCollection, (TP(cont), 0));

static void ObjNotFound(char *where, Object *which)
{
    Error(where, "object (0x%x) not found", (int) which);
}

OrdCollection::OrdCollection(int s)
{
    cont= new ObjArray(s);
    size= 0;
} 

OrdCollection::~OrdCollection()
{
    SafeDelete(cont);
}

void OrdCollection::InitNew()
{
    cont= new ObjArray(cCollectionInitCap);
    size= 0;
}

void OrdCollection::FreeAll()
{
    for (int i= 0; i < size; i++) {
	Object *op= cont->At(i);
	if (op) {
	    op->FreeAll();
	    SafeDelete(op);
	}
    }
    size= 0;
}

ObjPtr OrdCollection::At(int index)
{
    if (index < 0 || index >= size)
	Error ("At", "illegal index %d (size= %d)", index, size);
    if (!AnyDeleted())
	return cont->UncheckedAt(index);

    // take care of deleted objects
    return SeqCollection::At(index);
}

ObjPtr OrdCollection::After(ObjPtr op)
{
    if (op == 0) 
	return 0;
    int i= cont->IndexOf(op);
    if (i == -1) { 
	ObjNotFound("After", op);
	return 0;
    }
    if (i == size-1)
	return 0;    
    return cont->At(i+1);
}

ObjPtr OrdCollection::Before(ObjPtr op)
{
    if (op == 0) 
	return 0;
    int i= cont->IndexOf (op);
    if (i == -1) { 
	ObjNotFound("Before", op);
	return 0;
    }
    if (i == 0)
	return 0;    
    return cont->At(i-1);
}

ObjPtr OrdCollection::First()
{
    return At(0);
}

ObjPtr OrdCollection::Last()
{
    int i;
    Object *op;

    for (i= Size()-1; i >= 0; i--) {
	op= cont->At(i);
	if (!op->IsDeleted())
	    return op;
    }
    return 0;
}

int OrdCollection::Capacity()
{
    return cont->Size();
}

ObjPtr OrdCollection::Add(ObjPtr op)
{
    if (CheckNotNull("Add", op))
	return 0;
    AddAt(size, op);
    return 0;
}

void OrdCollection::InsertAtPos(ObjPtr at, ObjPtr op, bool byPtr, bool after, char *name) 
{
    if (CheckNotNull(name, op))
	return;
    if (cont->Size() <= 0)
	return;
    CheckActiveIter(name);
    int i;
    if (byPtr)
	i= cont->IndexOfPtr(at);
    else
	i= cont->IndexOf(at);
    if (i == -1) 
	ObjNotFound(name, op);
    if (after)
	i++;
    AddAt(i, op);
}

void OrdCollection::InsertAfter(ObjPtr after, ObjPtr op)
{
    InsertAtPos(after, op, FALSE, TRUE, "InsertAfter");
}

void OrdCollection::InsertAfterPtr(ObjPtr after, ObjPtr op)
{
    InsertAtPos(after, op, TRUE, TRUE, "InsertAfterPtr");
}

void OrdCollection::InsertBefore(ObjPtr before, ObjPtr op)
{
    InsertAtPos(before, op, FALSE, FALSE, "InsertBefore");
}

void OrdCollection::InsertBeforePtr(ObjPtr before, ObjPtr op)
{
    InsertAtPos(before, op, TRUE, FALSE, "InsertBeforePtr");
}

ObjPtr OrdCollection::Remove(ObjPtr e)
{
    register ObjPtr t= 0;

    if (e == 0) 
	return 0;
    for (int i= 0; i < size; i++) {
	Object* tmp; // g++ ??
	tmp= cont->At(i);
	if (e->IsEqual(tmp))
	    t= DoRemoveAt(i);
    }
    return t;
}

ObjPtr OrdCollection::RemovePtr(ObjPtr e)
{
    register ObjPtr t= 0;

    if (e == 0) 
	return 0;
    for (int i = 0; i < size; i++)
	if (e == cont->At(i))
	    t= DoRemoveAt(i);
    return t;
}

void OrdCollection::AddAt(int at, ObjPtr op)
{
    register int j;

    if (CheckNotNull("AddAt", op))
	return;
    CheckActiveIter("AddAt");
    if (at > size)
	Error("AddAt", "out of range");
    if (size == cont->Size())
	cont->Expand (GrowBy(max(cont->Size(), cMinExpand)));
    for (j= size-1; j >= at; j--)
	(*cont)[j+1]= (*cont)[j];

    (*cont)[at]= op;
    size++;
    Changed();
}

ObjPtr OrdCollection::RemoveAt(int i)
{
    if (!AnyDeleted())
	return DoRemoveAt(i);
    return RemovePtr(At(i));
}

ObjPtr OrdCollection::DoRemoveAt(int i)
{
    ObjPtr t;
    register int j;

    t= cont->RemoveAt(i);
    if (InIterator()) {
	cont->AtPut (i, new DeletedObject);
	AnnounceRemove();
	Changed();
	return t;
    }
    for (j= i; j < size-1; j++)
	cont->AtPut(j, cont->At(j+1));
    cont->AtPut(size-1, 0);
    
    size--;
    if (LowWaterMark()) 
	cont->Expand (cont->Size() / cOrdColShrinkFactor);
	
    Changed();
    return t;
}

void OrdCollection::RemoveDeleted()
{
    Object *op;
    for (int j= 0; j < size;) {
	op= cont->At(j);
	if (op && op->IsDeleted()) {
	    DoRemoveAt(j);
	    SafeDelete(op);
	}
	else 
	    j++;
    }
}

void OrdCollection::Empty (int s)                                         
{
    CheckActiveIter("Empty");
    SafeDelete(cont);
    cont= new ObjArray(max(s,4)); 
    size= 0; 
}

Iterator *OrdCollection::MakeIterator() 
{ 
    return new OrdCollectionIter(this); 
}

Iterator *OrdCollection::MakeReversedIterator () 
{ 
    return new RevOrdCollectionIter(this); 
}

void OrdCollection::Sort()
{
    CheckActiveIter("Sort");
    cont->Sort(size);
}

int OrdCollection::BinarySearch(ObjPtr op)
{
    return cont->BinarySearch(op, size);
}

//---- ordered collection iterator ---------------------------------------------

OrdCollectionIter::OrdCollectionIter(Collection *s)
{ 
    cs= (OrdCollection *)s;
    ce= 0; 
}

OrdCollectionIter::~OrdCollectionIter()
{
    IteratorEnd();
}

void OrdCollectionIter::Reset(Collection *s)
{
    if (s == 0)
	s= Coll();
    cs= (OrdCollection *)s;
    ce= 0;
    Iterator::Reset(s);
}

Collection *OrdCollectionIter::Coll()
{
    return cs;
}

ObjPtr OrdCollectionIter::operator()()
{
    IteratorStart();
    // find next non deleted object
    for ( ;ce < cs->Size() && cs->cont->UncheckedAt(ce)->IsDeleted(); ce++)
	;
    if (ce < cs->Size()) 
	return cs->cont->UncheckedAt(ce++);
    IteratorEnd();
    return 0;
}

void *OrdCollectionIter::operator new(size_t sz)
{
    return MemPools::Alloc(sz);
}

void OrdCollectionIter::operator delete(void *vp)
{
    MemPools::Free(vp, sizeof(OrdCollectionIter)); 
}

//---- RevOrdCollectionIter ----------------------------------------------------

RevOrdCollectionIter::RevOrdCollectionIter(Collection *s) : OrdCollectionIter(s)
{
    ce= cs->Size()-1;
}

void RevOrdCollectionIter::Reset(Collection *s)
{
    OrdCollectionIter::Reset(s);
    ce= cs->Size()-1;
}

ObjPtr RevOrdCollectionIter::operator()()
{
    IteratorStart();
    // find next non deleted object
    for (; ce >= 0 && cs->cont->At(ce)->IsDeleted(); ce--);
    if (ce >= 0) 
	return cs->cont->UncheckedAt(ce--);
    IteratorEnd();
    return 0;
}
