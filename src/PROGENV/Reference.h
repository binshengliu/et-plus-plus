#ifndef Reference_First
#define Reference_First

#include "Object.h"

enum RefFlags {
    eRefDeleted     =   BIT(eObjLast + 1),
    eRefLast        =   eObjLast + 1
};

//---- reference --------------------------------------------------------------

class Ref : public Object {
    int type, offset, len;
    class Class *cla;
    void *base;
    void Init(void *base, int type, int offset, int len, Class *c);
public:
    MetaDef(Ref);

    Ref();
    Ref(void *base, int type, int offset, int len, Class *c);
    Ref(Object *&op);
    Ref(Object &op);
    Ref(Ref &ref);
    
    bool IsEqual(Object *op);
    void operator=(Ref r);
    void *Addr()
	{ return (void*)((u_long)base + (u_long)offset); }
    int *AddrOfLen()
	{ return (int*)((u_long)base + (u_long)len); }
    
    Class *GetClass()
	{ return cla; }
    int GetOffset()
	{ return offset; }
    int GetType()
	{ return type; }
    void SetType(int t)
	{ type= t; }
    int GetLen()
	{ return len; }
    void SetLen(int l)
	{ len= l; }
    Object *GetObject();
    bool IsObject();
    int Length(bool &ok);
    bool CanDeref()
	{ return (bool)(IsObject() || (type & (T_PTR | T_VEC | T_ARR | T_PTR2))); }
    Ref *Deref();
    void *GetBase()
	{ return base; }
    char *TypeName();
    char *PreName();
    char *PostName();
    char *Value();
    
    void Dump();
};

#endif Reference_First

