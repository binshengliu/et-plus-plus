//$Object,ChangeMessage,FindMember$
#include "Object.h"
#include "OrdColl.h"
#include "IdDictionary.h"
#include "ObjInt.h"
#include "Error.h"
#include "String.h"
#include "Storage.h"
#include "ProgEnv.h"
#include "System.h"
#include "ObjectTable.h"
#include "ClassManager.h"
#include "AccessMem.h"
#include "IO/membuf.h"

bool gInPrintOn;
bool gAddToInstTable= TRUE;

extern void InitProgenv();
  
//---- observing ---------------------------------------------------------

static IdDictionary *Observer; 
// dictionary associating a list of observers with an Object

static IdDictionary *DelayedChanges; 
// dictionary to store the delayed Change messages, the key is the object pointer
// (IdDictionary) the value is an OrderedCollection of ChangeMessages (see below).
// The first entry of the OrderedCollection is a reference count of the number of
// DelayChanges requests. FlushChanges will be executed when the count drops to 0.
// Whether change messages are delayed is set with the cObjDelayChanges flag


struct ChangeMessage: public Object {
    void *what;
    int part, id;

    ChangeMessage(void *w, int p, int i)
	{ what= w; part= p; id= i; }
};

//---- automatically added by macro metaImpl -----------------------------------

static Class ObjectClassMetaImpl0("Object",
				  sizeof (Object),
				  new Object((_dummy*)0),
				  __FILE__,
				  "./Object.h",
				  __LINE__,
				  1,
				  TRUE);

Object::Object(class _dummy*)
{
    flags= cObjNonDeleted | cObjIsProto;
    isa= &ObjectClassMetaImpl0;
    ObjectClassMetaImpl0.SetSuper();
}

Class *Object::IsA()
{
    return &ObjectClassMetaImpl0;
}

void Object::Members()
{
    TheThis= this;
    D_F(TX(flags), 0);    
}

//---- ordinary methods ----------------------------------------------------

Object::Object(int f)
{
    flags= (f & cFlagMask) | cObjNonDeleted;
    ObjectTable::Add(this);
}

Object::~Object()
{
    if (! TestFlag(cObjNonDeleted)) {
	if (gDebug)
	    Fatal("~Object", "object deleted twice");
	return;
    }
  
    ObjectTable::Remove(this);
    flags&= ~cObjNonDeleted;
    if (gInPrintOn) 
	gClassManager->InvalidatePtr(this);
	
    if (IsObserved()) {
	Collection *observers= GetObservers(); // GetObservers is statically bound!!!
	flags &= ~cObjIsObserved;
	if (observers)
	    CleanupObservers(observers);
    }
}

void Object::FreeAll()
{
}

char *Object::ClassName()
{
    return IsA()->Name();
} 

ObjPtr Object::New()
{
    return IsA()->New();
}

void Object::InitNew()
{
}

void Object::Inspect(bool block)
{
    InitProgenv();
    gProgEnv->InspectObject(this, block);
}

void Object::EditSource(bool definition)
{
    InitProgenv();
    gProgEnv->EditSourceOf(IsA(), definition);
}

void Object::InspectorId(char *buf, int)
{
    buf= "";
}

void Object::Parts(class Collection *)
{
}

void Object::DoError(int level, char *location, char *fmt, va_list va)
{
    ::ErrorHandler(level, form("%s::%s", ClassName(), location), fmt, va);
}

void Object::Error(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    DoError(cError, location, va_(fmt), ap);
    va_end(ap);
}

void Object::SysError(char *location,  char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    DoError(cSysError, location, va_(fmt), ap);
    va_end(ap);
}

void Object::Warning(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    DoError(cWarning, location, va_(fmt), ap);
    va_end(ap);
}

void Object::Fatal(char *location, char *va_(fmt), ...)
{
    va_list ap;
    va_start(ap,va_(fmt));
    DoError(cFatal, location, va_(fmt), ap);
    va_end(ap);
}

Object *Object::Clone()
{
    Object *clone= New();

    if (clone)
	BCOPY((byte*) this, (byte*) clone, IsA()->Size());
    return clone;
}

Object *Object::DeepClone()
{
    int status;
    membuf mb;
    ostream to(&mb);
    istream from(&mb);
    ObjPtr op;

    gClassManager->Reset();
    gInPrintOn= TRUE;
    to << this NL;
    to.flush();
    gInPrintOn= FALSE;
    
    gClassManager->Reset();
    from >> op;
    if (status= gClassManager->Reset()) {
	Error("DeepClone", "status %d", status);
	return 0;
    }
    return op;
}

void Object::SetFlag(int f, bool b)
{
    if (b)
	SetFlag(f);
    else
	ResetFlag(f);
}

//---- fire walls ----------------------------------------------------------

void Object::AbstractMethod(char *method)
{
    Warning(method, "abstract method called"); 
}

void Object::MayNotUse(char *method)
{
    Warning(method, "not allowed to use this method");
}

Object *Object::guard(Class *should)
{
    if (this != 0 && IsA()->isKindOf(should))
	return this;
    Error("Guard", "object is not a %s but a %s", should->Name(), ClassName());
    return 0;
}

//---- comparing -----------------------------------------------------------

u_long Object::Hash () 
{
    return (u_long) this;
}

bool Object::IsEqual (Object *anOp)
{
     return anOp == this;
}

int Object::Compare (Object*)
{
    AbstractMethod ("Compare");
    return 0;
}

//---- Object Observing --------------------------------------------------------

void Object::AddObserver (ObjPtr op)
{
    Collection *observer;
    
    if (op) {
	if (!IsObserved() || ((observer= GetObservers()) == 0)) {
	    flags |= cObjIsObserved; 
	    observer= MakeObserverColl();
	} else {
	    if (observer->FindPtr(op)) {
		Warning("AddObserver", "duplicate in observers");
		return;
	    }
	}
	observer->Add (op);
    }
}

ObjPtr Object::RemoveObserver (ObjPtr op)
{
    if (this && IsObserved()) {
	Collection *observers= GetObservers();
	if (observers) {
	    if (op) 
		op= observers->RemovePtr(op);
	    if (observers->IsEmpty()) {
		flags &= ~cObjIsObserved;
		DestroyObserverColl();
	    }
	    return op;
	}
    }
    return 0;
}

void Object::Send(int id, int part, void *vp)
{
    if (!IsObserved())
	return;
    if (TestFlag(cObjDelayChanges)) {
	if (DelayedChanges == 0)
	    ObjectTable::AddRoot(DelayedChanges= new IdDictionary);
	Collection *col= (Collection*) DelayedChanges->AtKey(this);
	if (!col)
	    Error("Send", "No collection for change messages, %s", ClassName());
	col->Add(new ChangeMessage(vp, part, id));
	DelayedChanges->AtKeyPut(this,col);
    } else {
	Collection *observers= GetObservers();
	if (observers)
	    observers->ForEach(Object,DoObserve)(id, part, vp, this);
    }
}

void Object::DoObserve(int id, int part, void *vp, Object *op)
{
}

void Object::DelayChanges()
{
    flags |= cObjDelayChanges;
    if (DelayedChanges == 0)
	ObjectTable::AddRoot(DelayedChanges= new IdDictionary);
    Collection *col= (Collection*) DelayedChanges->AtKey(this);
    if (!col) {
	col= new OrdCollection;
	col->Add(new ObjInt(0));
	DelayedChanges->AtKeyPut(this,col);
    } else { // increment reference count
	ObjInt *ref= (ObjInt*)col->At(0);
	(*ref)++;
    }
}

void Object::FlushChanges()
{
    if (DelayedChanges == 0)
	ObjectTable::AddRoot(DelayedChanges= new IdDictionary);
    Collection *col= (Collection*) DelayedChanges->AtKey(this);
    if (col) {
	ObjInt *ref= (ObjInt*)col->At(0);
	if (ref->GetValue() == 0) {
	    flags &= ~cObjDelayChanges;
	    ChangeMessage *cm;
	    Iter next(col);
	    next(); // overread reference count
	    while (cm= (ChangeMessage*) next()) 
		Send(cm->id, cm->part, cm->what);
	    col->FreeAll();
	    Object *tmp= DelayedChanges->RemoveKey(this);
	    delete tmp;
	} else 
	    (*ref)--;
    }   
}

class Iterator *Object::GetObserverIter()        
{
    if (!IsObserved())
	return 0;
    Collection *observers= GetObservers(); 
    if (observers)
	return observers->MakeIterator();
    return 0;
}

bool Object::PrintOnWhenObserved(Object*)
{
    return TRUE;    
}

Collection *Object::MakeObserverColl()
{
    if (Observer == 0)
	ObjectTable::AddRoot(Observer= new IdDictionary(70));
    Collection *cp= new OrdCollection;
    Observer->AtKeyPut(this, cp);
    return cp;
}

Collection *Object::GetObservers()
{
    if (Observer == 0) {
	Warning("GetObservers", "without SetObserver");
	return 0;
    }      
    Collection *cp= (Collection*) Observer->AtKey(this);
    if (cp == 0) 
	Warning("GetObservers", "observers = 0 in GetObservers for %s", ClassName());
    return cp;
}

void Object::DestroyObserverColl()
{
    Assoc *op= (Assoc*)Observer->RemoveKey(this);
    Object *observers= op->Value();
    if (observers == 0)
	Warning ("DestroyObserverColl", "0 in DestroyObserverColl() for %s", ClassName());
    SafeDelete(op);
    SafeDelete(observers);
}

void Object::SetObserverColl(Collection *cp)
{
    Object *op= Observer->RemoveKey(this);
    SafeDelete(op);
    Observer->AtKeyPut(this, cp);        
}

void Object::CleanupObservers(Collection *observers)
{
    observers->ForEach(Object,DoObserve)(-1, cPartSenderDied, 0, this);
    DestroyObserverColl();
}

IdDictionary *ObjectGetObservers()
{
    return Observer;
}

//---- converting --------------------------------------------------------------

char* Object::AsString()
{
    return "";
}

//---- activation/passivation --------------------------------------------------

static bool printOnWhenObserved(ObjPtr, ObjPtr op, Object *from)
{
    return op->PrintOnWhenObserved(from);
}

ostream &Object::PrintOn(ostream &os)
{
    Collection *selectedDeps= 0;
	    
    IsA()->MakeIndex(this);
    if (IsObserved()) {
	Collection *observers= GetObservers();
	if (observers->IsEmpty()) { // impossible ??
	    flags &= ~cObjIsObserved;
	    DestroyObserverColl();
	} else {
	    selectedDeps= observers->Select((BoolFun)printOnWhenObserved, this);
	    if (selectedDeps->IsEmpty())
		SafeDelete(selectedDeps);
	}
    }
    os << (flags & cFlagMask) SP << selectedDeps SP;
    SafeDelete(selectedDeps);
    return os;
}

ostream &operator<< (ostream& s, ObjPtr p)
{
    return gClassManager->SavePtr(s, p);
}

istream &LoadPtr(istream &s, Object *&op, Class *cl)
{
    return gClassManager->LoadPtr(s, op, cl);
}

istream &Object::ReadFrom(istream &is)
{
    int f;
    Collection *observers;
    
    IsA()->MakeIndex(this);
    is >> f >> observers;
    SetFlag(f);
    if (observers) {
	flags |= cObjIsObserved;
	SetObserverColl(observers);
    }
    return is;
}

ostream &Object::DisplayOn(ostream &o)
{
    return o NL;
}

//---- FindMember --------------------------------------------------------------

class FindMember: public AccessMembers {
public:
    const char *lookfor;
    short offset;
    int len;
    int type;
    Object *op;
    Class *classp;
    
    FindMember(Object *in, const char *name)
	{ offset= -1; op= in; lookfor= name; type= 0; len= -1;}
    void Member(char *name, short offset, short lenOrOffset, int type, Class*);
};

void FindMember::Member(char *name, short o, short l, int t, Class *c)
{
    if (strcmp(name, lookfor) == 0) {
	offset= o;
	type= t;
	classp= c;
	if (l == -1)
	    len= GetLength(op, l, t);
    }
}

//---- interface for accessing instance variables by name will be improved
//     in a next release

void *Object::GetAddrOf(const char *name, Class *&cl)
{
    FindMember m(this, name);
    IsA()->EnumerateMembers(&m);
    if (m.offset != -1) {
	cl= m.classp;
	return (void*) ((u_long) this + (u_long) m.offset);
    }
    else
	return 0;
}

