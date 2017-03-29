//$MarkObjects$
 
#include "Object.h"
#include "OrdColl.h"
#include "Set.h"
#include "ObjectTable.h"
#include "System.h"
#include "Error.h"
#include "ClassManager.h"
#include "AccessMem.h"
#include "String.h"

//---- Object Table ----------------------------------------------------------
    
static OrdCollection *delayed; // insertions while an iterator is active are delayed

static bool died= FALSE;
static bool exists= FALSE;

ObjectTable     *gObjectTable= 0;
static ObjPtr   *gRootTable= 0;
static int      gRootSlot= 0;

ObjectTable::ObjectTable()
{
    convoi= seed= 0;
    cumconvoi= 0;
    
    if (exists)
	Error("ObjectTable::ObjectTable", "only one instance of ObjectTable");
    exists= TRUE;
    cursor= 0;
    fromClass= 0;
    inIterator= FALSE;
    size= SetNextPrime(10000);
    table= new ObjPtr[size]; 
}

ObjectTable::~ObjectTable()
{
    SafeDelete(table);
    died= TRUE;
}

void ObjectTable::Start(Class *clp, bool mem)
{
    if (inIterator)
	Error("ObjectTable::Start", "iterator is already active");
    if (delayed && delayed->Size() != 0)
	delayed->Empty();
    inIterator= TRUE;
    fromClass= clp;
    members= mem;
    cursor= 0;
}

void ObjectTable::End()
{
    if (!inIterator)
	Error("ObjectTable::End", "End() called without Start()");
    inIterator= FALSE;
    if (delayed == 0)
	delayed= new OrdCollection;
    Iter next(delayed);
    Object *op;
    while (op= next())
	Add(op);
    if (delayed->Size())
	delayed->Empty();
    fromClass= 0;
    cursor= 0;
}

Object *ObjectTable::operator()()
{  
    Object *op;
    
    for ( ; cursor < size ; cursor++) {
	op= table[cursor];
	if (op != 0) {
	    if (fromClass == 0)
		break;
	    if (members && fromClass == op->IsA())
		break;
	    if (!members && op->IsA() && op->IsA()->isKindOf(fromClass))
		break;
	}
    }              
    if (cursor < size) 
	return (table[cursor++]);
    else 
	return (0);
}    

void ObjectTable::AddObject(ObjPtr op)
{
    int slot;
    if (died)
	return;
    if (op == 0)
	Error("ObjectTable::AddObject", "op is 0");
    if (inIterator) {
	if (delayed == 0)
	    delayed= new OrdCollection;
	delayed->Add(op);
	return;
    }
    slot= FindElement (op);
    table[slot]= op;
    tally++;
    if (HighWaterMark())
	Expand(2 * size);
}

bool ObjectTable::PtrIsValidOfObject(Object *op)
{
    return table[FindElement(op)] != 0;
}

int ObjectTable::FindElement (ObjPtr op)                                         
{
    register slot,n;
    ObjPtr slotOp;

    seed++;
    slot= ((unsigned int) op >> 2) % size;
    for (n= 0; n < size; n++) {
	convoi= max(convoi, n);
	cumconvoi+= n;
	if ((slotOp = table[slot]) == 0) 
	    return slot;
	if (op == slotOp) 
	    return slot;
	if (++slot == size) 
	    slot = 0;
    } 
    return 0;
}

void ObjectTable::RemoveObject(ObjPtr op)
{
    int i;

    if (died)
	return;
    if (op == 0)
	Error("ObjectTable::Remove", "remove 0 from ObjectTable");
    if (inIterator)
	Error("ObjectTable::Remove", "remove while iterator is active");
    i= FindElement(op);
    if (table[i] == 0 && !op->TestFlag(cObjIsProto)) { 
	Error("ObjectTable::Remove", "%d not found at %d (%s)", (int)op, i,
							    op->ClassName());
	for (int j= 0; j < size; j++) {
	    if (table[j] == op) {
		Error("ObjectTable::Remove", "%d found at %d !!!", (int)op, j);
		i= j;
	    }
	}
    }
    table[i]= 0;
    FixCollisions(i);
    tally--;
}

void ObjectTable::FixCollisions (int index)                                         
{
    int oldIndex, nextIndex;
    Object *nextObject;
    
    oldIndex= index+1;
    for (;;oldIndex++) {
	if (oldIndex >= size)
	    oldIndex= 0;
	nextObject= table[oldIndex];
	if (nextObject == 0)
	    break;
	nextIndex= FindElement(nextObject);
	if (nextIndex != oldIndex) {
	    table[nextIndex]= nextObject;
	    table[oldIndex]= 0;
	}
    }
}

void ObjectTable::Expand(int newSize)
{
    ObjPtr *oldTable= table, op;

    int oldsize= size;
    newSize= SetNextPrime(newSize);
    table = new ObjPtr[newSize];
    size = newSize;
    tally= 0;
    for (int i= 0; i < oldsize; i++) {
	op= oldTable[i];
	if (op)
	    AddObject(op);
    }
    delete oldTable; 
}

void ObjectTable::HashStatistics()
{
    double mean= 0.0;
    if (seed)
	mean= (float)cumconvoi/(float)seed;
    cerr << "Hashing statistics of ObjectTable\n";
    cerr << "Capacity= " << size SP << "Tally= " << tally SP;
    cerr << "MaxConvoi= " << convoi SP << "Mean Convoi= " << mean NL;
}

void ObjectTable::Verify() 
{
    for (int i= 0; i < size; i++) {
	//if (table[i] && table[i]->_vptr == 0) {
	//    cerr << "VERIFY: " << (int) table[i] SP << "vptr == 0" NL;
	//    break;
	//}
	if (table[i]) {
	    Object *op= table[i];
	    op->IsA();
	}
    }
}

//---- static members --------------------------------------------------------

void ObjectTable::UpdateInstCount()
{
    register Class *clp;
    Object *op;
    
    if (gObjectTable == 0)
	return;
	
    Iter sp(gClassManager->Iterator());
    
    while (clp= (Class*) sp())
	clp->ResetInstanceCount();
    
    gObjectTable->Start();    
    while (op= (*gObjectTable)()) {
	if (op->IsA())
	    op->IsA()->AddInstance(); 
    } 
    gObjectTable->End();
}

int ObjectTable::Instances()
{
    if (gObjectTable == 0)
	return 0;
    return gObjectTable->tally;
}

void ObjectTable::Add(Object *op)
{
#   ifdef ET_PROGENV

    extern bool gAddToInstTable;

    if (gAddToInstTable) {
	if (gObjectTable == 0 && !died)
	    gObjectTable= new ObjectTable;
	gObjectTable->AddObject(op);
    }
    
#   endif ET_PROGENV
}
    
void ObjectTable::Remove(Object *op)
{ 
#   ifdef ET_PROGENV
    if (gObjectTable && !op->TestFlag(cObjIsProto)) 
	gObjectTable->RemoveObject(op);
#   endif ET_PROGENV
}

bool ObjectTable::PtrIsValid(Object *op)
{ 
    if (gObjectTable)
	return gObjectTable->PtrIsValidOfObject(op);
    return TRUE;
}

Object *ObjectTable::SomeInstance(Class *cl)
{
    Object *op= 0;
    gObjectTable->Start();    
    while (op= (*gObjectTable)())
	if (op->IsA() && op->IsA()->isKindOf(cl))
	    break;
    gObjectTable->End();
    return op;
}

Object *ObjectTable::SomeMember(Class *cl)
{
    Object *op= 0;
    gObjectTable->Start();    
    while (op= (*gObjectTable)())
	if (op->IsA() && op->IsA() == cl)
	    break;
    gObjectTable->End();
    return op;
}

void ObjectTable::AddRoot(Object *op)
{
#   ifdef ET_PROGENV    
    if (gRootTable == 0)
	gRootTable= new ObjPtr[200];
    gRootTable[gRootSlot++]= op;
#   endif ET_PROGENV
}

void ObjectTable::AddRoots(Object *va_(first), ...)
{
    Object *op;
    va_list ap;
    va_start(ap,va_(first));
    AddRoot(va_(first)); 
    while (op= va_arg(ap, Object*))
	ObjectTable::AddRoot(op);  
    va_end(ap);
}

//---- MarkObjects --------------------------------------------------------------        
	
class MarkObjects: public AccessObjPtrs {
public:
    MarkObjects() : AccessObjPtrs(0)
	{}
    void FoundPointer(Object *op, char*, int);
}; 

void MarkObjects::FoundPointer(Object *op, char*, int)
{
    // Mark the Object
    Object *saved= inObject; 
    if (op && !op->TestFlag(cObjVisited) && ObjectTable::PtrIsValid(op)) { 
	inObject= op;
	inObject->SetVisited();
	op->IsA()->EnumerateMembers(this);
    }
    inObject= saved;
}

static void inspId(void *p1, void *p2, void *p3, void*)
{
    Object *op= (Object*)p1;
    char *buf= (char*)p2;
    int sz= (int)p3;
    op->InspectorId(buf, sz);
}

void ObjectTable::VisitObjects()
{
#   ifdef ET_PROGENV
    Object *op;
    char buf[200];
    
    if (!gObjectTable || !gRootTable)
	return;
    gObjectTable->Start();    
    while (op= (*gObjectTable)()) 
	op->ClearVisited();
    gObjectTable->End();
    
    MarkObjects m;
    cout << "Objects unreachable from:" NL;
    cout << "=========================" NL;
    for (int i= 0; i < gRootSlot; i++) {
	op= gRootTable[i];
	if (op && !op->TestFlag(cObjVisited) && ObjectTable::PtrIsValid(op)) { 
	    buf[0]= '\0';
	    gSystem->Call(inspId, op, buf, (void*)sizeof(buf), 0);
	    cout.form("    0x%x (%s): %s\n", (int)op, op->ClassName(), buf);
	    m.ForObject(op);
	    m.FoundPointer(op, 0, -1);
	}   
    } 
    cout << "-------------------------" NL;
    gObjectTable->Start();    
    while (op= (*gObjectTable)()) 
	if (!op->TestFlag(cObjVisited)) { 
	    buf[0]= '\0';
	    gSystem->Call(inspId, op, buf, (void*)sizeof(buf), 0);
	    cout.form("0x%x (%s): %s\n", (int)op, op->ClassName(), buf);
	}
    gObjectTable->End();
    cout << "=============================" NL;
    cout.flush();
#   endif ET_PROGENV
}

