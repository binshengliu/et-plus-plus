#ifndef ClassManager_First
#ifdef __GNUG__
#pragma once
#endif
#define ClassManager_First

#include "Object.h"

typedef int (Object::*OObjMemberFunc)(char*);

//---- ClassManager ------------------------------------------------------------

class ClassManager: public Object {
    class ObjArray *classTable;
    int nClasses, status;
    OObjMemberFunc DynLoadHook;
    Object *dynLoadPrivData;
public:
    MetaDef(ClassManager);
    ClassManager();
    ~ClassManager();
    
    void Add(Class*);
    void Remove(Class*);
    void SetStatus(int st)
	{ status= st; }
    ostream& SavePtr(ostream&, Object*);
    istream& LoadPtr(istream&, Object *&op, const Class*);
    void InvalidatePtr(Object*);
    ostream &DisplayOn(ostream &os);
    int Reset();
    Class *Find(char *name);
    class Iterator *Iterator(bool sorted= TRUE);
    void SetupSubclasses();
    void SetDynLoadHook(Object *op, OObjMemberFunc of);
    void InstanceStatistics(bool cumSorted= FALSE);
};

extern ClassManager *gClassManager;


#endif ClassManager_First
