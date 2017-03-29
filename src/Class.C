//$Class, _boolClass, _flagsClass,$

#include <stdio.h>

#include "Class.h"
#include "Error.h"
#include "Storage.h"
#include "String.h"
#include "ObjArray.h"
#include "OrdColl.h"
#include "ObjectTable.h"
#include "ClassManager.h"
#include "AccessMem.h"

//---- Class -------------------------------------------------------------------

static AccessMembers *accessMembers;

MetaImpl(Class, (TP(super), TP(className), T(myId), T(size),
    TP(instanceTable), TP(proto), TP(declFileName), 
    T(declFileLine), TP(implFileName),
    T(implFileLine), TP(subclasses), T(instanceCount), TP(methods), 0));

Class::Class(char *name, int sz, Object *pro, char *implfn, char *declfn,
					int il, int dl, bool abstract, int)
{
    size= sz;
    className= name;
    declFileName= declfn;
    implFileName= implfn;
    declFileLine= dl;
    implFileLine= il;
    proto= pro;
    subclasses= 0;
    instanceTable= 0;
    instanceCount= 0;
    methods= 0;
    if (abstract)
	SetFlag(eClassAbstract);
    if (size > 0) {
	if (gClassManager == 0)
	    gClassManager= new ClassManager;
	gClassManager->Add(this);
    }
    if (proto)
	SetFlag(eClassObject);
    classptr= 0;
}

Class::~Class()
{
#   ifndef __GNUG__
    if (proto)
        delete (void*)proto;        // avoid call of destructor for prototype
#   endif __GNUG__
    proto= 0;
    SafeDelete(instanceTable);
    if (methods) {
	methods->FreeAll();
	SafeDelete(methods);
    }
    SafeDelete(subclasses);
    if (size > 0 && gClassManager)
	gClassManager->Remove(this);
}

Class *Class::SetSuper()
{
    super= classptr;
    classptr= this;
    return this;
}

u_long Class::Hash()
{
    register u_long hash;
    register char *p= className;

    for (hash= 0; *p; p++)
	hash= (hash << 1) ^ *p;
    return hash;
}

bool Class::IsEqual(Object *op)
{
    return strcmp(className, ((Class*)op)->className) == 0;
}

int Class::Compare(Object *op)
{
    return strcmp(className, ((Class*)op)->className);
}

char* Class::AsString()
{
    return className;
}

void Class::EnumerateMyMembers(AccessMembers *ac)
{
    accessMembers= ac;
    if (className && accessMembers)
	accessMembers->ClassName(className);
    if (proto)
	proto->Members();
}

void Class::EnumerateMembers(AccessMembers *ac)
{
    EnumerateMyMembers(ac);
    if (super)
	super->EnumerateMembers(ac);
}

bool Class::isKindOf(register Class *c)
{
    register Class *cc;

    for (cc= this; cc; cc= cc->super)
	if (cc == c)
	    return TRUE;
    return FALSE;
}

Object *Class::New()
{
    if (size <= 0) {
	Error("New", "size <= 0 (%s)", className);
	return 0;
    }
    Object *op= (Object*) new byte[size];
    ObjectTable::Add(op);
    
    // give the new object the correct vtable in a portable? way
    BCOPY((byte*) proto, (byte*) op, size);

    op->flags |= cObjNonDeleted;
    op->flags &= ~cObjIsProto;  
    op->InitNew();
    return op;
}

ostream& Class::DisplayOn(ostream &s)
{
    s << form("%4d\t%-20s\t%-20s", size, className,
					    super ? super->className : "nil");
    return s;
}

void Class::Reset()
{
    if (instanceTable)
	for (int i= 0; i<instanceTable->Size(); i++)
	    (*instanceTable)[i]= 0;
}

void Class::AddSubclass(Class *cl)
{
    if (subclasses == 0)
	subclasses= new OrdCollection;
    subclasses->Add(cl);
}

Iterator *Class::SubclassIterator()
{
    if (subclasses == 0)
	subclasses= new OrdCollection;
    return subclasses->MakeIterator();
}

void Class::SavePtr(ostream& o, ObjPtr p)
{
    bool newinstance;

    o.put('\n');
    o.put('{');
    o << className;
    o.put(' ');
    o.put('#');
    o << MakeIndex(p, &newinstance);
    if (newinstance) {
	o.put(' ');
	o << *p;
    }
    o.put('}');
}

void Class::InvalidatePtr(ObjPtr p)
{
    int ix;

    if (instanceTable == 0)
	return;
    if ((ix= instanceTable->IndexOfPtr(p)) < 0) 
	return;
    (*instanceTable)[ix]= (ObjPtr) 0x01;    // hack !!
}

int Class::MakeIndex(ObjPtr p, bool *bp)
{
    int ix;
    bool isnew= FALSE;
    
    if (instanceTable == 0)
	instanceTable= new ObjArray(10);

    if ((ix= instanceTable->IndexOfPtr(p)) < 0) {
	isnew= TRUE;
	instanceTable->Add(p);
	ix= instanceTable->IndexOfPtr(p);
    }
    if (bp)
	*bp= isnew;
    return ix;
}

ObjPtr Class::LoadPtr(istream& is)
{
    ObjPtr p= 0;
    int ix;
    char c;

    is >> c >> ix;
    if (c != '#') {
	Error("LoadPtr", "missing \"#\" (%s)", className);
	gClassManager->SetStatus(1);
	return NULL;
    }
    if (ix < 0) {
	Error("LoadPtr", "instance number out of range (%d)", ix); 
	gClassManager->SetStatus(2);
	return NULL;
    }

    if (instanceTable == 0)
	instanceTable= new ObjArray(ix*2);
    else if (ix >= instanceTable->Size())     // new
	instanceTable->Expand (2*instanceTable->Size());
    else
	p= instanceTable->At(ix);

    if (p == 0) {
	if ((p= New()) == 0) {
	    Error("LoadPtr", "can't make clone; New returns 0");
	    return NULL;
	}
	is >> *p;
	instanceTable->AtPut(ix, p);
    }

    is >> c;
    if (c != '}') {
	Error("LoadPtr", "missing \'}\' (%s,%d,%c)", className, ix, c);
	gClassManager->SetStatus(3);
	return NULL;
    }
    return p;
}

void Class::InspectorId(char *buf, int bufSize)
{
    if (className)
	strn0cpy(buf, className, bufSize);
    else
	Object::InspectorId(buf, bufSize);
}

Object *Class::SomeInstance()
{
    return ObjectTable::SomeInstance(this);
}
 
Object *Class::SomeMember()
{
    return ObjectTable::SomeMember(this);
}

Collection *Class::GetMethods()
{
    return methods;
}

void Class::SetMethods(Collection *col)
{
    methods= col;
}

void Class::Show(char *buf, void *addr)
{
    Object* oo= (Object*) addr;
    if (oo && oo->IsKindOf(Object))
	sprintf(buf, "<%s> 0x%x", oo->ClassName(), oo);
    else
	sprintf(buf, "oops");
}

//---- access functions ---------------------------------------------------

void *TheThis= 0;

void D_F(int va_(firstTag), ...)
{
    void *dummy= 0;
    va_list ap;
    int t= 0, tag;
    char *name;
    Class *cl;
    u_long th= (u_long) TheThis, offset;
    
    va_start(ap, va_(firstTag));
    for (int i= 0; accessMembers; i++) {
	if (i == 0)
	    tag= va_(firstTag);
	else
	    tag= va_arg(ap, int);
	name= va_arg(ap, char*);
	cl= va_arg(ap, Class*);
	offset= va_arg(ap, u_long);
	if (name == 0 || cl == 0)
	    break;
	switch (tag) {
	case 'A':
	    accessMembers->Member(name, (short)(offset-th), -1, t+0, cl);
	    break;
	case 'P': 
	    accessMembers->Member(name, (short)(offset-th), -1, t+T_PTR, cl);
	    break;
	case 'p':
	    accessMembers->Member(name, (short)(offset-th), -1, t+T_PTR+T_PTR2, cl);
	    break;
	case 'B':
	    accessMembers->Member(name, (short)(offset-th),
				    (short)(va_arg(ap, u_long)-th), t+T_VEC, cl);
	    break;
	case 'b':
	    accessMembers->Member(name, (short)(offset-th),
				    (short)(va_arg(ap, u_long)-th), t+T_VEC+T_PTR2, cl);
	    break;
	case 'D':
	    accessMembers->Member(name, (short)(offset-th),
					    va_arg(ap, int), t+T_ARR, cl);
	    break;
	case 'd':
	    accessMembers->Member(name, (short)(offset-th),
					    va_arg(ap, int), t+T_ARR+T_PTR2, cl);
	    break;
	default:
	    goto out;
	}
    }
out:
    va_end(ap);
}
