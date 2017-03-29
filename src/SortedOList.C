//$SortedObjList$
#include "SortedOList.h"

MetaImpl0(SortedObjList);

SortedObjList::SortedObjList(bool as)
{ 
    if (as)
	SetFlag(SObListAscending);
}

ObjPtr SortedObjList::Add(ObjPtr op)
{
    register ObjLink *l;
    register int comp;
    
    if (CheckNotNull("Add", op))
	return 0;
    CheckActiveIter("Sort");

    if (first == 0) {
	Insert(op);
	return 0;
    }
    
    for (l= first; l; l= l->Next()) {
	comp= CompareObjects(op, l->Op());
	if ((Ascending() && comp < 0) || (!Ascending() && comp > 0)) {
	    if (l == first)
		Insert(op);
	    else {
		new ObjLink(op, l);
		size++;
		Changed();
	    }                
	    return 0;
	}
    }
    
    if (l == 0)
	ObjList::Add(op); 
    return 0;
}

int SortedObjList::CompareObjects(Object *op1, Object *op2)
{
    return op1->Compare(op2);
}

void SortedObjList::AddFirst(ObjPtr op)
{
    Add(op);
}

void SortedObjList::AddLast(ObjPtr op)
{
    Add(op);
}

void SortedObjList::InsertBefore(ObjPtr, ObjPtr op)
{
    Add(op);
}

void SortedObjList::InsertAfter(ObjPtr, ObjPtr op)
{
    Add(op);
}

void SortedObjList::InsertBeforePtr(ObjPtr, ObjPtr op)
{
    Add(op);
}

void SortedObjList::InsertAfterPtr(ObjPtr, ObjPtr op)
{
    Add(op);
}

