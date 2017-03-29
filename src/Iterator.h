#ifndef Iterator_First
#ifdef __GNUG__
#pragma once
#endif
#define Iterator_First

#include "Root.h"
#include "Types.h"

//---- class Iterator --------------------------------------------------

class Iterator : public Root {
protected:
    bool started, terminated; 
    void IteratorStart();
    void IteratorEnd();
public:  
    //---- creation, destruction
    Iterator();  
    virtual ~Iterator();
    virtual class Object *operator()();
    virtual class Collection *Coll();
    virtual void Reset(class Collection *c= 0);
};

#endif Iterator_First

