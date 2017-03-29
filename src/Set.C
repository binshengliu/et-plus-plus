//$Set,SetIter,SetDeletedObj$

#include "Set.h"
#include "Error.h"
#include "Math.h"

//---- place holder for deleted objects in a set ------------------------------

class SetDeletedObj: public DeletedObject {
    unsigned long hash;
public:
    SetDeletedObj(unsigned long h)
	{ hash= h; }
    unsigned long Hash () // do not break collision chain
	{ return hash; }
};

//---- set ------------------------------------------------------------

const cSetExpandFactor = 2,
      cSetShrinkFactor = 2;

MetaImpl(Set, (T(initSize), TP(cont), 0));

Set::Set(int s)
{
    Init(s);
}

Set::~Set()
{
    SafeDelete(cont);
}

void Set::InitNew()
{
    Init(cCollectionInitCap);
}

void Set::Init(int s)
{
    size= 0;
    initSize= SetNextPrime(max(s, cCollectionInitCap));
    cont= new ObjArray(initSize);
}

ObjPtr Set::Add(ObjPtr op)
{
    int slot;
    Object *ret= 0;

    if (CheckNotNull("Add", op))
	return 0;
    CheckActiveIter("Add");
    slot= FindElement(op);

    if (cont->At(slot) == 0) {
	size++;
	cont->AtPut(slot,op);
    }
    else
	ret= op;
    if (HighWaterMark())
	Expand(cSetExpandFactor * cont->Size());
    return ret;
}

void Set::Filter(ObjPtr op)
{
    int slot;

    if (CheckNotNull("Filter", op))
	return;
    CheckActiveIter("Filter");
    slot= FindElement(op);

    if (cont->At(slot) == 0) {
	size++;
	cont->AtPut (slot,op);
    } else 
	delete op;

    if (HighWaterMark())
	Expand(cSetExpandFactor * cont->Size()); 
}

ObjPtr Set::Remove(ObjPtr op)
{
    ObjPtr removedOp;
    int i;

    if (op == 0)
	return 0;
    i= FindElement(op);

    if ((removedOp= cont->At(i)) == 0) 
	return 0;

    if (InIterator()) {
	cont->AtPut (i, new SetDeletedObj(HashObject(cont->At(i))));
	AnnounceRemove();
	return removedOp;
    }
    else {
	cont->AtPut(i, 0);
	FixCollisions(i);
	size--;
	if (LowWaterMark())
	    Expand (cont->Size() / cSetShrinkFactor); 
    }
    return removedOp;
}

ObjPtr Set::RemovePtr(ObjPtr op)
{
    return Remove(op);
}

void Set::Empty (int s)                                         
{
    CheckActiveIter("Empty");
    SafeDelete(cont);
    s= max(s, cCollectionInitCap);
    cont= new ObjArray(s); 
    size= 0;
    Changed();
}

bool Set::Contains (ObjPtr op)                                         
{
    if (op == 0)
	return FALSE;
    if (cont->At(FindElement(op)) != 0)
	return TRUE;
    return FALSE;
}

bool Set::ContainsPtr (ObjPtr op)                                         
{
    return Contains(op);
}

//---- find the slot of op or return the index of the next empty slot
//     the starting point of the search op hash is invoked. In case
//     of a collision the next empty slot (linerar hashing) will be tried

int Set::FindElement (ObjPtr op)                                         
{
    register u_long slot, n;
    ObjPtr slotOp;
    int sz;

    if (op == 0)
	return FALSE;
    sz= cont->Size();
    slot= HashObject(op) % sz;
    for (n= 0; n < sz; n++) {
	if ((slotOp= cont->At((int)slot)) == 0)
	    return slot;
	if (!slotOp->IsDeleted() && ObjectsEqual(op, slotOp))
	    return slot;
	if (++slot == sz)
	    slot= 0;
    } 
    Error("FindElement", "set is full");
    return 0;
}

u_long Set::HashObject(Object *op)
{
    return (unsigned) op->Hash();
}

bool Set::ObjectsEqual(Object *op1, Object *op2)
{
    return op1->IsEqual (op2);
}
    
//---- fix the hole in the collision chain

void Set::FixCollisions (int index)                                         
{
    int oldIndex, nextIndex;
    Object *nextObject;
    
    oldIndex= index+1;
    for (;;oldIndex++) {
	if (oldIndex >= cont->Size())
	    oldIndex= 0;
	nextObject= cont->At(oldIndex);
	if (nextObject == 0)
	    break;
	nextIndex= FindElement(nextObject);
	if (nextIndex != oldIndex) {
	    cont->AtPut(nextIndex, nextObject);
	    cont->AtPut(oldIndex, 0);
	}
    }
}

void Set::RemoveDeleted()
{
    Object *op;

    for (int j= 0; j < cont->Size();) {
	op= cont->At(j);
	if (op && op->IsDeleted()) {
	    SafeDelete(op);
	    cont->AtPut(j, 0);
	    FixCollisions(j);
	    size--;
	}    
	else
	    j++;
    }
}

ObjPtr Set::Find(ObjPtr op)
{ 
    if (op == 0)
	return 0;
    return cont->At(FindElement(op));
}

ObjPtr Set::FindPtr(ObjPtr op)
{ 
    return Find(op);
}

Iterator *Set::MakeIterator() 
{ 
    return new SetIter(this); 
}

SetPtr Set::Union (SetPtr sp)
{
    SetPtr aUnion = new Set;

    aUnion->AddAll(this);
    aUnion->AddAll(sp);
    return aUnion;
}

SetPtr Set::Difference (SetPtr sp)
{
    SetPtr diff = new Set;

    diff->AddAll (this);
    diff->RemoveAll (sp);
    return diff;
}    

SetPtr Set::Intersection (SetPtr sp)
{
    SetIter next(this);
    register SetPtr intSec= new Set;
    register ObjPtr op;

    while (op = next())
	if (sp->Contains(op))
	    intSec->Add(op);
    return intSec;
}

void Set::Expand(int newSize)
{
    // ensure that the newSize is a prime number
    newSize= SetNextPrime(newSize);
    ObjArray *oldCont= cont;
    cont= new ObjArray(newSize);
    size= 0;
    AddAll(oldCont);    
    delete oldCont;    
}

//---- class SetIter ----------------------------------------------------------

SetIter::SetIter(Collection *s)
{ 
    cs= (Set *)s; 
    ce= 0; 
}

Collection *SetIter::Coll()
{
    return cs;
}

void SetIter::Reset(Collection *s)
{
    if (s == 0)
	s= Coll();
    cs= (Set*) s;
    ce= 0;
    IteratorEnd();
    Iterator::Reset(s);
}

SetIter::~SetIter()
{
    IteratorEnd();
}

ObjPtr SetIter::operator()()
{
    if (cs->cont == 0)
	return 0;
    IteratorStart();
    int sz= cs->cont->Size();
    // find next non empty item
    for (;ce < sz; ce++) {
	  Object *op= cs->cont->UncheckedAt(ce);
	  if (op != 0 && !op->IsDeleted())
	    break;
    }    
    if (ce < sz) 
	return cs->cont->UncheckedAt(ce++);
    IteratorEnd();
    return 0;
}

//-----------------------------------------------------------------------------

int SetNextPrime(int x)
{
    int n, sqr;
    
    if (x <= 3) 
	return 3;
	
    if (x % 2 == 0) 
	x++;
/* Change by Bryan Boreham, Kewill, Thu Aug 10 15:16:35 1989.
   Added the cast to double to get g++ to use the inline version.    */
#ifdef __GNUG__
    sqr= (int) sqrt((double)x) + 1;
#else
    sqr= (int) sqrt(x) + 1;
#endif
    for (;;) {
	for (n = 3; (n <= sqr) && (x % n != 0); n+= 2) 
	    ;
	if (n > sqr) 
	    return x;
	x+= 2;
    }
}
