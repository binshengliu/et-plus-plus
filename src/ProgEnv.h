#ifndef ProgEnv_First
#ifdef __GNUG__
#pragma once
#endif
#define ProgEnv_First

#include "Types.h"
#include "Object.h"

//---- ProgEnv ------------------------------------------------------------
// entry points for the ET++ programming environment

class ProgEnv : public Object {
public:
    MetaDef(ProgEnv);
    ProgEnv();
    
    virtual void Start();
    
    //---- source code browser
    virtual void EditSourceOf(class Class *cp, bool def, int at= -1);
    virtual void ShowInHierarchy(class Class *cp);
    virtual void ShowInheritancePath(class Class *cp);
    
    //---- inspector
    virtual void ShowInObjStruBrowser(class Object *op);
    virtual void InspectObject(class Object *op, bool block= FALSE);
    
    //---- abstract views
    virtual void AddAbstractViewPolicy(class AbstractViewPolicy*);
    virtual char *HasAbstractView(Class *cl, void *addr, Object *op);
    virtual void ShowAbstractView(Class *cl, void *addr, Object *op, Object *insp);
};               

class AbstractViewPolicy: public Object {
public:
    virtual char *ViewName(Class *cl, void *addr, Object *op);
    virtual bool ShowView(Class *cl, void *addr, Object *op, Object *);
};

extern ProgEnv *gProgEnv;

#endif ProgEnv_First
