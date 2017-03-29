//$Bag,BagIter,BagOccIter,BagItem$
#include "Bag.h"
#include "Error.h"

const cInitCap = 5;

//---- Bag Item ----------------------------------------------------------------
//---- a bag is implemented as a set of bag items, that register the object
//     and the number of occurrences

MetaImpl(BagItem, (TP(op), T(occurrences), 0));

void BagItem::FreeAll()
{
    if (op) {
	op->FreeAll();
	delete op;
    }
}

unsigned long BagItem::Hash()
{
    return op->Hash();
}

bool BagItem::IsEqual(ObjPtr anOp)
{
    return op->IsEqual(((BagItem*)anOp)->op);
}

int BagItem::Compare(ObjPtr anOp)
{
    return op->Compare(Guard(anOp, BagItem)->op);
}

ostream& BagItem::PrintOn (ostream &s)
{
    return op->PrintOn(s);
}

istream& BagItem::ReadFrom (istream &s)
{
    return op->ReadFrom(s);
}        

//---- Bag ----------------------------------------------------------------------

MetaImpl(Bag, (TP(cont), 0));

Bag::Bag(int s)
{   
    size= max(s, cInitCap);
    cont= new Set(size);
}

Bag::~Bag()
{
    SafeDelete(cont);
}

void Bag::FreeAll()
{
    if (cont)
	cont->FreeAll();
}

ObjPtr Bag::Add(ObjPtr op)
{
    BagItem tmp(op, 1), *bip;

    if (CheckNotNull("Add", op))
	return 0;
    if (size == 0 || cont == 0)
	cont = new Set(size= cInitCap);
    bip = (BagItem*)cont->Find(&tmp);

    if (bip == 0)
	cont->Add (new BagItem (op,1));
    else
	bip->Inc();  
    size++;
    return 0;
}

void Bag::AddWithOccurrences(ObjPtr op,int count)
{
    while (count--)
	Add(op);
}

ObjPtr Bag::Remove(ObjPtr op)
{
    BagItem tmp(op, 1), *bip;
    ObjPtr removedOp;

    if (op == 0)
	return 0;

    bip = (BagItem*)cont->Find(&tmp);

    if (bip == 0)
	return 0;
    else {
	removedOp = bip->Op();
	bip->Dec();
	if (bip->Occurrences() == 0) {
	    if (cont->Remove(bip) == 0)
		Error("Remove", "bag item not found in contents");  
	    delete bip;  
	}        
    } 
    size--; 
    return removedOp;
}

ObjPtr Bag::At(int)
{
    MayNotUse ("At");
    return 0;
}

void Bag::Empty (int s)                                         
{
    if (cont) {
	delete cont;
	if (s < cInitCap)
	    s = cInitCap;    
	cont = new Set(s); 
	size = 0; 
    }
}

bool Bag::Contains(ObjPtr op)                                         
{
    BagItem tmp(op,1), *bip;

    if (op == 0)
	return FALSE;        

    bip = (BagItem*)cont->Find(&tmp);
    return (bool) (bip != 0);
}

int Bag::OccurrencesOf(ObjPtr op)
{
    BagItem tmp(op,1), *bip;

    if (op == 0)
	return FALSE;        

    bip = (BagItem*)cont->Find(&tmp);
    if (bip == 0)
	return 0;
    else
	return bip->Occurrences();
}

ObjPtr Bag::Find(ObjPtr op)
{ 
    BagItem tmp(op,1), *bip;

    if (op == 0)
	return 0;
    bip = (BagItem*)cont->Find(&tmp);
    if (bip == 0)
	return 0;
    return bip->Op();
}

Iterator *Bag::MakeIterator() 
{ 
    return new BagIter(this); 
}

//---- Bag Iterator -------------------------------------------------------------

ObjPtr BagIter::operator()()
{
    BagItem *bip;

    if (cs->Size() == 0)
	return 0;

    if (occurrences == 0) { // fetch next item from set  
	bip = (BagItem *) next();
	if (bip == 0)
	    return 0;
	occurrences = bip->Occurrences();
	op = bip->Op();
    }
    occurrences--;
    return op;    
}

void BagIter::Reset(Collection *s)
{ 
    occurrences = 0; 
    cs = (Bag *)s; 
    next.Reset(((Bag*)s)->cont); 
}

//---- Bag OccIterator ---------------------------------------------------------

ObjPtr BagOccIter::Occurrence(int *occurrences)
{
    BagItem *bip;

    if (cs->Size() == 0)
	return 0;

    bip = (BagItem *) next();
    if (bip == 0) 
	return 0;
    *occurrences = bip->Occurrences();
    return(bip->Op());
}

void BagOccIter::Reset(Collection *s)
{ 
    cs = (Bag *)s; 
    next.Reset(((Bag*)s)->cont); 
}
