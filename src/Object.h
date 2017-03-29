#ifndef Object_First
#ifdef __GNUG__
#pragma once
#endif
#define Object_First

#include "Root.h"
#include "Types.h"
#include "CmdNo.h"

#define Ptr(x) x
extern istream &LoadPtr(istream &s, class Object *&op, class Class*);

typedef class Object *ObjPtr;
typedef bool (*BoolFun)(ObjPtr,ObjPtr,void*);
typedef ObjPtr (*ObjPtrFun)(ObjPtr,ObjPtr,void*);
typedef void (Object::*ObjMemberFunc)(...);     // obsolete
typedef void (Object::*VoidObjMemberFunc)(...);
typedef unsigned char (Object::*BoolObjMemberFunc)(...);

enum ObjFlags {
    eObjIsDeleted   = BIT(1),
    eObjDefault     = 0,
    eObjLast        = 1
};

extern bool gInPrintOn;

const int cFlagMask= 0x00ffffff;

//---- private flags, clients can only test but not set them ------------

const int cObjNonDeleted    =       0x01000000,
	  cObjDelayChanges  =       0x02000000,
	  cObjVisited       =       0x04000000,
	  cObjOnHeap        =       0x08000000,
	  cObjIsProto       =       0x10000000,
	  cObjIsObserved    =       0x20000000;

//---- class Object -----------------------------------------------------

class Object : public Root {
friend class Class;
    u_int flags;
protected:
    //---- overridden if observers are stored in the object itself
    virtual class Collection *MakeObserverColl();
    virtual class Collection *GetObservers();
    virtual void DestroyObserverColl();
    virtual void SetObserverColl(Collection *);
    void CleanupObservers(Collection *);
public:
    //---- automatically added by macro metaDef --------------------------------
    static class Class *isa;
    friend istream &operator>> (istream &s, Object *&op)
	{ return ::LoadPtr(s, op, Object::isa); }
    friend class Class *_Type(Object*)
	{ return Object::isa; }
    Object(class _dummy*);
    virtual class Class *IsA();
    virtual void Members();

    //---- ordinary methods ----------------------------------------------------
    Object(int f= eObjDefault);
    virtual ~Object();

    virtual void FreeAll();

    char *ClassName();
    ObjPtr New();
    virtual void InitNew();

    //----- flag manipulation --------------------------------------------------
    void SetFlag(int f, bool b);
    void SetFlag(int f)
	{ flags|= f & cFlagMask; }
    void ResetFlag(int f)
	{ flags&= ~(f & cFlagMask); }
    bool TestFlag(int f)
	{ return (bool) ((flags & f) != 0); }
    void InvertFlag(int f)
	{ flags^= f & cFlagMask; }

    void MarkAsDeleted()
	{ SetFlag(eObjIsDeleted); }
    bool IsDeleted()
	{ return TestFlag(eObjIsDeleted); }
    void SetVisited()
	{ flags|= cObjVisited; }
    void ClearVisited()
	{ flags&= ~cObjVisited; }
    bool IsObserved()
	{ return TestFlag(cObjIsObserved); }
    
    //---- misc ----------------------------------------------------------------
    virtual void Inspect(bool block= FALSE);
    virtual void EditSource(bool declaration);
    virtual void InspectorId(char *buf, int bufSize);
    virtual void Parts(class Collection*);
    
    virtual void DoError(int level, char *location, char *fmt, va_list va);
    void Error(char *location, char *msgfmt, ...);
    void SysError(char *location, char *msgfmt, ...);
    void Warning(char *location, char *msgfmt, ...);
    void Fatal(char *location, char *msgfmt, ...);
    virtual Object *Clone();
    virtual Object *DeepClone();
    
    //---- direct (unchecked) access to instance variables, returns address of varialbe
    virtual void* GetAddrOf(const char *varname, Class *&cl);
    
    //---- fire walls ----------------------------------------------------------
    void AbstractMethod(char*);
    void MayNotUse(char*);
    Object *guard(Class*);

    //---- comparing -----------------------------------------------------------
    virtual u_long Hash();
    virtual bool IsEqual (Object*);
    virtual int Compare (Object*);

    //---- converting ----------------------------------------------------------
    virtual char* AsString();

    //---- change propagation --------------------------------------------------
    virtual void Send(int id= -1, int part= cPartAnyChange, void *vp= 0);
    virtual void DoObserve(int id, int part, void *vp, Object *op);
    
    void Changed()
	{ Send(); }
    void AddObserver(Object*);
    Object* RemoveObserver(Object*);
    class Iterator *GetObserverIter();
    void DelayChanges();
    void FlushChanges();
    virtual bool PrintOnWhenObserved(Object *);

    //---- activation passivation ----------------------------------------------
    virtual ostream& PrintOn (ostream&s);
    virtual istream& ReadFrom(istream &);
    virtual ostream& DisplayOn (ostream&s);

    friend ostream &operator<< (ostream &o, Object &p)
	{ return p.PrintOn(o); }

    friend istream &operator>> (istream &o, Object &p)
	{ return p.ReadFrom(o); }

    friend ostream &operator<< (ostream &s, Object *p);
};

inline void swap(class Object **a, class Object **b)
{
    class Object *tmp;
    tmp= *a; *a= *b; *b= tmp;
}

//---- DoDelayChanges ----------------------------------------------------------

// to delay the calls to Changed and ChangedWhat an instance of the class
// DelayChanges can be declared
// eg: Text::Cut() { DelayChanges dc(this); ... }

class DoDelayChanges {
    Object *op;
public:
    DoDelayChanges(Object *o)
	{ op= o; op->DelayChanges(); }
    ~DoDelayChanges()
	{ op->FlushChanges(); }
};

//------------------------------------------------------------------------------

#include "Class.h"

#define IsKindOf(name) IsA()->isKindOf(Meta(name))
#define Guard(p,name) ((name*)(p)->guard(Meta(name)))

extern class ObjectTable *gObjectTable;

#endif Object_First

