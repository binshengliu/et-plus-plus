#include "AccessMem.h"
#include "System.h"

static void peekLength(void *p1, void *p2, void*, void*)
{
    *((int*) p1)= *((int*) p2);
}

//---- AccessMembers ---------------------------------------------------------

AccessMembers::AccessMembers()
{
}

bool AccessMembers::IsObjPtr(int type, Class *cl)
{ 
    return (type & T_PTR) && cl->TestFlag(eClassObject); 
}
 
int AccessMembers::GetLength(Object *inObject, short lenOrOffset, int type)
{
    if (type & T_VEC) { 
	int l= 0, *lp= (int*)((u_long)inObject + (u_long)lenOrOffset);
	if (gSystem->Call(peekLength, (void*) &l, (void*) lp, 0, 0))
	    return -1;
	return l;
    }
    if (type & T_ARR)
	return lenOrOffset;
    return -1;
}

Object *AccessMembers::GetObject(Object *inObject, short offset, int type, Class *cl)
{
    if (cl->TestFlag(eClassObject) && (type & T_PTR))
	return *(Object**)((unsigned long) inObject + (unsigned long) offset);
    return 0;
}

bool AccessMembers::IsObjPtrVec(int type, Class *cl)
{
    return cl->TestFlag(eClassObject) && (type&T_PTR2) && (type & (T_ARR|T_VEC));
}
 
Object **AccessMembers::GetObjPtrVec(Object *inObject, short offset, int type, Class *cl)
{
    if (IsObjPtrVec(type, cl))
	return *(Object***)((u_long) inObject + (u_long) offset);
    return 0;
}

void AccessMembers::ClassName(char*)
{
}

void AccessMembers::Member(char *name, short offset, short offsetOrLen, int type, Class *cl)
{
}

//---- AccessObjPtrs ----------------------------------------------------------

AccessObjPtrs::AccessObjPtrs(Object *op)
{ 
    inObject= op; 
}
    
void AccessObjPtrs::ForObject(Object *op)
{ 
    inObject= op; 
}

void AccessObjPtrs::Member(char *name, short offset, short offsetOrLen, int type, Class *cl)
{
    if (IsObjPtrVec(type, cl)) {
	int l= GetLength(inObject, offsetOrLen, type);
	Object **ov= GetObjPtrVec(inObject, offset, type, cl);
	for (int i= 0; ov && i < l; i++) 
	    if (ov[i]) 
		FoundPointer(ov[i], name, i);
    }
    else { 
	if (IsObjPtr(type, cl)) {
	    FoundPointer(GetObject(inObject, offset, type, cl), name, -1);
	} 
    }
}

void AccessObjPtrs::FoundPointer(Object *op, char*, int)
{
}
