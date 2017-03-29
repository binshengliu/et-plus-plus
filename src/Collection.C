//$Collection,DeletedObject,CollFilterIter$
#include "Error.h"
#include "Collection.h"
#include "OrdColl.h"
#include "ObjArray.h"
#include "Set.h"
#include "Bag.h"
#include "ObjList.h"
#include "SortedOList.h"
#include "String.h"
    
Collection *pCurrentCollection;
char *cNullPointerWarning= "called with 0";

AbstractMetaImpl(Collection, (T(size), T(nDeleted), T(iterCount), 0));

Collection::Collection()
{
    size= 0;
    iterCount= 0;
    nDeleted= 0;
}

Collection::~Collection()
{
    if (iterCount && gDebug) 
	Warning("~Collection", "%s deleted while there is an active iterator",
								ClassName());
}

bool Collection::assertclass(Class *cl)
{
    register ObjPtr op;
    Iter next(this);
    bool error= FALSE;

    if (cl == 0) {
	Error("assertclass", "class == 0");
	return TRUE;
    }
    for (int i= 0; op= next(); i++)
	if (! op->IsA()->isKindOf(cl)) {
	    Error("assertclass", "element %d is not instance of class %s (%s)", i,
						cl->Name(), op->ClassName());
	    error= TRUE;
	}
    return error;
}

void Collection::InspectorId(char *b, int sz)
{
    if (Size() && !IsKindOf(Set) && At(0) && sz > 10) {
	char *firstClass= At(0)->ClassName();
	if (firstClass && strlen(firstClass) + 20 < sz) 
	    sprintf(b, "<%s> [%d]", firstClass, Size());
	else
	    sprintf(b, "[%d]", Size()); 
    }
    else
	sprintf(b, "[%d]", Size());   
}

ObjPtr Collection::Add(ObjPtr) 
{
    AbstractMethod("Add");
    return 0;
}

void Collection::AddVector(ObjPtr va_(op1), ...) 
{
    va_list ap;    
    va_start(ap,va_(op1));
    register Object *op;
    
    Add(va_(op1));
    while (op= va_arg(ap, Object*))
	Add(op);
    va_end(ap);
}

void Collection::AddVector(va_list ap) 
{
    register Object *op;
    
    while (op= va_arg(ap, Object*))
	Add(op);
}

ObjPtr Collection::Remove(ObjPtr)
{
    AbstractMethod("Remove");
    return 0;
}

ObjPtr Collection::RemovePtr(ObjPtr)
{
    AbstractMethod("RemovePtr");
    return 0;
}

void Collection::RemoveDeleted()
{
}

ObjPtr Collection::Clone()
{
    Collection *newcoll= (Collection*) New();

    newcoll->AddAll(this);
    return newcoll;
}

void Collection::AddAll(CollPtr Coll)
{
    Iter next(Coll);
    register ObjPtr op;

    while (op = next())
	Add (op);
}

void Collection::RemoveAll(CollPtr Coll)
{
    Iter next(Coll);
    register ObjPtr op;

    while (op = next())
	Remove(op);
}

void Collection::FreeAll()
{
    Iter next(this);
    register ObjPtr op;

    while (op= next())
	if (op= Remove(op)) {
	    op->FreeAll();
	    delete op;
	}
    int size= 0;
}

void Collection::Empty(int)
{
    Iter next(this);
    register ObjPtr op;

    while (op= next())
	Remove(op);
}

bool Collection::Contains(ObjPtr anOp)
{
    return Find(anOp) != 0;
}

bool Collection::ContainsPtr(ObjPtr anOp)
{
    return FindPtr(anOp) != 0;
}

ObjPtr Collection::Find(ObjPtr anOp)
{
    register ObjPtr op;

    if (anOp == 0)
	return 0;
    Iter next(this);

    while (op = next()) 
	if (op->IsEqual (anOp))
	    return op;
    return 0;   
}

ObjPtr Collection::FindPtr(ObjPtr anOp)
{
    register ObjPtr op;

    if (anOp == 0)
	return 0;
    Iter next(this);

    while (op = next()) 
	if (op == anOp)
	    return op;
    return 0;   
}

ObjPtr Collection::At(int pos)
{
    Iter next(this);
    register ObjPtr op;

    for (int i= 0; op= next(); i++) 
	if (i == pos)
	    break;
    if (op == 0) {
	Error ("At", cOutOfBoundsError);
	return 0;
    }
    return op;   
}

Iterator *Collection::MakeIterator()
{
    AbstractMethod("Iterator");
    return 0;
}

bool Collection::IsEqual(ObjPtr col)
{
    if (IsA() != col->IsA())
	return FALSE;

    Iter next1(this), next2((CollPtr)col);
    register ObjPtr op1, op2;

    for (;;) {
	op1 = next1();
	op2 = next2();
	if (op1 == 0 && op2 == 0)
	    return TRUE;
	if (op1 == 0 || op2 == 0) 
	    break;
	if (! op1->IsEqual(op2))
	    break;
    }
    return FALSE;
}

unsigned long Collection::Hash()
{
    Iter next(this);
    register ObjPtr op;
    register unsigned long s= 0;

    while (op= next())
	s^= op->Hash();
    return s;   
}

int Collection::OccurrencesOf(ObjPtr anOp)
{
    register ObjPtr op;
    register int n = 0;

    if (anOp == 0)
	return 0;
    Iter next(this);
    while (op = next()) 
	if (op->IsEqual (anOp))
	    n++;
    return n;   
}

int Collection::OccurrencesOfPtr(ObjPtr anOp)
{
    register ObjPtr op;
    register int n = 0;

    if (anOp == 0)
	return FALSE;
    Iter next(this);
    while (op = next()) 
	if (op == anOp)
	    n++;
    return n;   
}

CollPtr Collection::Collect(ObjPtrFun fp,void* Arg)
{
    Iter next(this);
    register ObjPtr op;
    register CollPtr cp = (CollPtr) New();    

    while (op = next())
	cp->Add ((*fp)(this,op,Arg)); 
    return cp;
}

CollPtr Collection::Select(BoolFun fp,void *Arg)
{
    Iter next(this);
    register ObjPtr op;
    register CollPtr cp = (CollPtr) New();    

    while (op = next())
	if ((*fp)(this,op,Arg))
	    cp->Add (op); 
    return cp;
}

ObjPtr Collection::Detect(BoolFun fp,void *Arg)
{
    Iter next(this);
    register ObjPtr op, detectedOp = 0;

    while (op = next())
	if ((*fp)(this,op,Arg)) {
	    detectedOp = op;
	    break;
	}
    return detectedOp;
}

void Collection::EnterIter()
{
    iterCount++;
}

void Collection::ExitIter()
{
    iterCount--;
    if (iterCount == 0 && nDeleted) {
	RemoveDeleted();
	nDeleted= 0;
    }
    else if (iterCount < 0)
	iterCount= 0;
}

void Collection::CheckActiveIter(char *where)
{
    if (InIterator())
	Warning(where, "iterator is still active");    
}

int Collection::GrowBy(int desiredSize)
{
    int s= 0;
    if (Size() >= cMaxInt)
	Error("GrowBy", "cannot expand collection");
    else
	s= range(2, cMaxInt - desiredSize, desiredSize);
    return Size()+s;
}

class OrdCollection *Collection::AsOrderedCollection()
{
    OrdCollection *oc= new OrdCollection(Size());

    oc->AddAll(this);
    return oc;
}

class ObjArray *Collection::AsObjArray()
{
    register ObjArray *oa = new ObjArray(Size());
    Iter next(this);
    register ObjPtr op;

    for (int i= 0; op = next(); i++)
	oa->AtPut(i, op);
    return oa;
}

class Bag *Collection::AsBag()
{
    Bag *bp = new Bag(Size());

    bp->AddAll(this);
    return bp;
}

class Set *Collection::AsSet()
{
    Set *sp = new Set(Size());

    sp->AddAll(this);
    return sp;
}

class ObjList *Collection::AsObjList()
{
    ObjList *ol = new ObjList;

    ol->AddAll(this);
    return ol;
}

class SortedObjList *Collection::AsSortedObjList(bool ascending)
{
    SortedObjList *sl = new SortedObjList(ascending);

    sl->AddAll(this);
    return sl;
}

ostream& Collection::PrintOn (ostream& s)
{
    register ObjPtr op;
    Iter next(this);
    int sz;

    Object::PrintOn(s);
    s << Size() SP;
    for (sz= 0; op = next(); sz++)
	if (!op->IsDeleted())
	    s << op SP;
    return s NL;
}

ostream& Collection::DisplayOn(ostream &s)
{
    register ObjPtr op;
    Iter next(this);

    while (op= next()) {
	op->DisplayOn(s);
	s NL;
    }
    return s;
}

istream& Collection::ReadFrom (istream& s)
{
    int sz;
    ObjPtr op;

    Object::ReadFrom(s);
    s >> sz;
    for (int i= 0; i < sz; i++) {
	s >> op;
	Add(op);
    }
    return s;
}

bool Collection::NullPointerWarning(char *where)
{
    Warning(where, cNullPointerWarning); 
    return TRUE; 
}

//---- class CollFilterIter ----------------------------------------------------

CollFilterIter::~CollFilterIter()
{
    SafeDelete(next);
}

void CollFilterIter::Reset(Collection *s)
{
    next->Reset(s);
}

ObjPtr CollFilterIter::operator()()
{
    register ObjPtr op;

    while (op = (*next)()) {
	if (!filterFun)
	    break;
	if (filterFun((ObjPtr)this,op,filterArg))
	    break;
    }
    return op;               
}

//---- class DeletedObject -----------------------------------------------------

MetaImpl0(DeletedObject);

DeletedObject::DeletedObject() : Object(eObjIsDeleted)
{ 
}
