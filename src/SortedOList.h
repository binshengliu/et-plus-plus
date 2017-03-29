#ifndef SortedOList_First
#ifdef __GNUG__
#pragma once
#endif
#define SortedOList_First

#include "ObjList.h"

enum SObListFlags {
    SObListAscending    = BIT(eObjLast+2),
    SObListLast         = eObjLast+3
};


class SortedObjList : public ObjList {

protected:
    virtual int CompareObjects(Object *, Object *);

public:
    MetaDef(SortedObjList);

    SortedObjList(bool as= TRUE);
    bool Ascending()
	{ return TestFlag(SObListAscending); };            

    ObjPtr Add(ObjPtr a);

    //---- overridden to preserve sorting order
    void AddFirst(ObjPtr);                      
    void AddLast(ObjPtr);
			  
    void InsertBefore(ObjPtr before, ObjPtr a); 
    void InsertAfter(ObjPtr after, ObjPtr a);   
    void InsertBeforePtr(ObjPtr before, ObjPtr a); 
    void InsertAfterPtr(ObjPtr after, ObjPtr a);   
};

#endif SortedOList_First
