//$SeqCollection$
#include "SeqColl.h"

AbstractMetaImpl0(SeqCollection);

SeqCollection::SeqCollection()
{
}

void SeqCollection::InsertBefore(ObjPtr, ObjPtr)
{
    AbstractMethod("InsertBefore");
}

void SeqCollection::InsertAfter(ObjPtr, ObjPtr)
{
    AbstractMethod("InsertAfter");
}

void SeqCollection::InsertBeforePtr(ObjPtr, ObjPtr)
{
    AbstractMethod("InsertBeforePt");
}

void SeqCollection::InsertAfterPtr(ObjPtr, ObjPtr)
{
    AbstractMethod("InsertAfterPtr");
}

ObjPtr SeqCollection::Before(ObjPtr)
{
    AbstractMethod("Before");
    return 0;
}

ObjPtr SeqCollection::After(ObjPtr)
{
    AbstractMethod("After");
    return 0;
}

ObjPtr SeqCollection::First()
{
    AbstractMethod("First");
    return 0;
}

ObjPtr SeqCollection::Last()
{
    AbstractMethod("Last");
    return 0;
}

void SeqCollection::AddFirst(ObjPtr op)
{
    ObjPtr first= First();
    if (first)
	InsertBefore(first, op);
    else
	Add(op);
}
    
void SeqCollection::AddLast(ObjPtr op)
{
    ObjPtr last = Last();
    if (last)
	InsertAfter(last,op);
    else
	Add(op);
}

ObjPtr SeqCollection::RemoveFirst()
{
    ObjPtr first = First();
    return first ? RemovePtr(first) : 0;
}
    
ObjPtr SeqCollection::RemoveLast()
{
    ObjPtr last = Last();
    return last ? RemovePtr(last) : 0;
}

int SeqCollection::IndexOf(ObjPtr anOp)
{
    Iter next(this);
    register ObjPtr op;
    register int s;
    
    if (anOp == 0)
	return -1;
    for (s = 0; op = next(); ) {
	if (!op->IsDeleted()) {
	    if (op->IsEqual(anOp))
		break;
	    s++;
	}
    }
    return op == 0 ? -1 : s;
}    

int SeqCollection::IndexOfPtr(ObjPtr anOp)
{
    Iter next(this);
    register ObjPtr op;
    register int s;
    
    if (anOp == 0)
	return -1;
    for (s = 0; op = next(); s) {
	if (!op->IsDeleted()) {
	    if (op == anOp)
		break;
	    s++;
	}
    }
    return op == 0 ? -1 : s;
} 

Iterator *SeqCollection::MakeReversedIterator()
{
    AbstractMethod("MakeReversedIterator");
    return 0;
}

