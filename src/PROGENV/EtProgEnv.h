#ifndef EtProgEnv_First
#define EtProgEnv_First

#include "ProgEnv.h"

//---- EtProgEnv ------------------------------------------------------------
// entry points for the ET++ programming environment

class EtProgEnv : public ProgEnv {
    class SeqCollection *abstViewPolicies;
    class EtPeDoc *pe;
    void MakeEtPeDoc();
public:
    MetaDef(EtProgEnv);
    EtProgEnv();
    ~EtProgEnv();
    
    void Start();
    //---- source code browser
    void EditSourceOf(class Class *cp, bool def, int at= -1);
    void ShowInHierarchy(class Class *cp);
    void ShowInheritancePath(class Class *cp);
	
    //---- inspector
    void InspectObject(class Object *op, bool block= FALSE);
    void ShowInObjStruBrowser(class Object *vop);

    //---- abstract views
    void AddAbstractViewPolicy(class AbstractViewPolicy*);
    char *HasAbstractView(Class *cl, void *addr, Object *op);
    void ShowAbstractView(Class *cl, void *addr, Object *op, Object *insp);
};

extern ProgEnv *NewEtProgEnv();

#endif EtProgEnv_First
