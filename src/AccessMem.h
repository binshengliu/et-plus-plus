#ifndef AccessMembers_First
#ifdef __GNUG__
#pragma once
#endif
#define AccessMembers_First

#include "Types.h"
class Class;
class Object;

//---- access the instance variables of an Object ---------------------------

class AccessMembers {
protected:
    AccessMembers();
    //--- overridden to get access to instance variables
public:
    bool IsObjPtr(int type, Class *cl);
    bool IsObjPtrVec(int type, Class*); 
    Object *GetObject(Object *inObject, short offset, int type, Class *);
    Object **GetObjPtrVec(Object *inObject, short offset, int type, Class *);
    int GetLength(Object *inObject, short lenOrOffset, int type);

    virtual void Member(char *name, short offset, short lenOrOffset, int type, Class*);
    virtual void ClassName(char *name); 
};

//---- access Object* of an Object -------------------------------------------

class AccessObjPtrs: public AccessMembers {
protected:
    Object *inObject;
    virtual void FoundPointer(Object *op, char*, int= -1);
public:
    AccessObjPtrs(Object *op= 0);
    void ForObject(Object *op);
    void Member(char *name, short offset, short offsetOrLen, int type, Class *cl);
};    

#endif AccessMembers
