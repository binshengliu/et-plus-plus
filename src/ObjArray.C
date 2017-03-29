//$ObjArray,ObjArrayIter$
#include "ObjArray.h"
#include "Error.h"
#include "Storage.h"
#include "FixedStorage.h"
#include "String.h"

typedef int (*COMPARE)(const void*, const void*);

#ifndef __GNUG__
    extern "C" void qsort(void*, int, int, COMPARE);
#endif

char *cOutOfBoundsError= "out of bounds %d size %d in 0x%x";
char *cMethodName= "operator[]";

//---- class ObjArray ---------------------------------------------------------

MetaImpl(ObjArray, (T(lb), TVP(cont,size), 0));

ObjArray::ObjArray(int s, int lowerBound)                
{ 
    if (s <= 0)
	s= 10;
    cont= new ObjPtr[size= s];
    lb= lowerBound;
}

ObjArray::~ObjArray()
{
    SafeDelete(cont);
}

void ObjArray::InitNew()
{
    cont= new ObjPtr[cCollectionInitCap];
    lb= 0;
}

void ObjArray::FreeAll()
{
    register int i;

    for (i= 0; i < size; i++) {
	if (cont[i]) {
	    cont[i]->FreeAll();
	    SafeDelete(cont[i]);
	}
    }
    size= 0;
}

ObjPtr ObjArray::RemoveAt(int i)
{ 
    ObjPtr t;

    if (!BoundsOk("RemoveAt", i))
	return 0;
    t= cont[i-lb];
    cont[i-lb]= 0; 
    Changed();
    return t;
}

ObjPtr ObjArray::Remove(ObjPtr a)
{   
    register int i;
    register ObjPtr op;

    for (i = 0; i < size; i++) {
	op= cont[i];
	if (op && op->IsEqual(a)) {
	    cont[i]= 0;
	    Changed();
	    return op;
	}
    }
    return 0;
}

ObjPtr ObjArray::RemovePtr(ObjPtr a)
{   
    register int i;
    register ObjPtr op;

    for (i = 0; i < size; i++) {
	op= cont[i];
	if (op && op == a) {
	    cont[i]= 0;
	    Changed();
	    return op;
	}
    }
    return 0;
}

void ObjArray::Expand(int newSize)
{
    if (newSize < 0) {
	Error ("Expand", "newSize < 0");
	return;
    }
    if (newSize == size)
	return;
    if (newSize <= size) {
	// if the array is shrinked check whether there are nonempty entries
	for (int j = newSize; j < size; j++)
	    if (cont[j] != 0) {
		Error ("Expand", "expand would cut off nonempty entries at %d", j);
		return;
	    }
    }            
    cont= (ObjPtr*) Realloc(cont, newSize * sizeof(ObjPtr));
    size= newSize;
}

Iterator *ObjArray::MakeIterator()
{
    return new ObjArrayIter(this);
}

ObjPtr ObjArray::AtPut(int i, ObjPtr ob)
{ 
    if (!BoundsOk ("AtPut", i))
	return 0;
    ObjPtr t= cont[i-lb];
    cont[i-lb]= ob;
    Changed();
    return t;
}

void ObjArray::AtPutAndExpand(int i,ObjPtr op)
{
    if (i < lb) {
	Error ("AtPutAndExpand", "out of bounds at %d in %x", i, this);
	return;
    }
    if (i-lb >= size) 
	Expand (max(i, GrowBy(size)));
    cont[i-lb] = op; 
    Changed();
}

ObjPtr ObjArray::Add(ObjPtr op)
{
    register int i;

    if (cont == 0)
	cont= new ObjPtr[size= 10];
    for (i= 0; i < size; i++)
	if (cont[i] == 0)
	    break;
    if (i >= size)
	Expand(GrowBy(size));
    cont[i]= op; 
    Changed();
    return 0;
}

int ObjArray::IndexOf (ObjPtr ob)
{
    register int i;

    for (i = 0; i < size; i++)
	if (ob && ob->IsEqual(cont[i])) 
	    return i+lb;
    return -1;
}

int ObjArray::IndexOfPtr (ObjPtr ob)
{
    register int i;

    for (i= 0; i < size; i++)
	if (ob == cont[i]) 
	    return i+lb;
    return -1;
}

ObjPtr ObjArray::At(int i)
{
    if (!BoundsOk("At", i))
	return 0;
    return cont[i-lb];
}

int ObjArray::Compare (ObjPtr)
{
    MayNotUse ("Compare");
    return -2;
}

bool ObjArray::IsEqual (ObjPtr ob)
{
    if (!ob->IsKindOf(ObjArray))
	return FALSE;
    ObjArray *t = (ObjArray *) ob; 
    if (t->size != size || t->lb != lb)
	return FALSE;
    for (int i= 0; i < Size(); i++) {
	if (cont[i] == 0 && t->cont[i] == 0)
	    continue;
	if (cont[i] == 0 || t->cont[i] == 0)
	    break;
	if (!cont[i]->IsEqual(t->cont[i]))
	    break;
    }
    return i == size;
}

ostream& ObjArray::PrintOn(ostream &s)
{
    Object::PrintOn(s); // supress Collection::PrintOn
    s << Size() SP;
    for (int i= 0; i < Size(); i++) 
	s << cont[i] SP;
    return s NL;
}

istream& ObjArray::ReadFrom(istream &s)
{
    Object::ReadFrom(s);
    delete cont;
    s >> size;
    cont= new ObjPtr[size];
    for (int i= 0; i < size; i++)
	s >> cont[i];
    return s;
}

unsigned long ObjArray::Hash ()
{
    register u_long i, hash;

    for (i= hash= size; i < size; i++)
	if (cont[i])
	    hash ^= cont[i]->Hash();
    return hash;
}

static int CompareFun(ObjPtr* a, ObjPtr* b) 
{ 
    if (*a == 0 || *b == 0)
	return 0;
    return (*a)->Compare(*b);
}

void ObjArray::Sort(int upto)
{
    qsort(cont, min(size, upto-lb), sizeof(ObjPtr), (COMPARE)CompareFun);
}

int ObjArray::BinarySearch(ObjPtr op, int upto)
{
    register int base, position, last, result;
    register Object *op2;
    
    if (op == 0)
	return -1;
    
    base= 0;
    last= min(size, upto-lb) - 1;
    
    while (last >= base) {
	position= (base+last) / 2;
	op2= cont[position];
	if ((op2 == 0) || (result= op->Compare(op2)) == 0)
	    return position + lb;
	if (result < 0)
	    last= position-1;
	else
	    base= position+1;
    }
    return -1;
}

//---- class ObjArray -------------------------------------------------------

ObjArrayIter::ObjArrayIter(Collection *s)
{
    cs= (ObjArray*)s; ce= 0; 
}

ObjArrayIter::~ObjArrayIter()
{
}

void *ObjArrayIter::operator new(size_t sz)
{
    return MemPools::Alloc(sz);
}

void ObjArrayIter::operator delete(void *vp)
{
    MemPools::Free(vp, sizeof(ObjArrayIter)); 
}

void ObjArrayIter::Reset(Collection *s)
{
    if (s == 0)
	s= cs;
    cs= (ObjArray*)s;
    ce= 0;
}

ObjPtr ObjArrayIter::operator()()
{
    for (;ce < cs->Size() && cs->cont[ce] == 0; ce++)
	;
    if (ce < cs->Size())
	return cs->cont[ce++];
    return 0;
}

bool ObjArrayIter::Filter(ObjPtr)
{
    return TRUE;
}
