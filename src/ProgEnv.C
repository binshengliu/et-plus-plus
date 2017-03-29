//$ProgEnv,AbstractViewPolicy$

#include "ProgEnv.h"

ProgEnv *gProgEnv;

//---- ProgEnv -----------------------------------------------------------------

MetaImpl0(ProgEnv);

ProgEnv::ProgEnv()
{
}
    
void ProgEnv::Start()
{
    cerr << "ET++PE not linked in" NL;
}

void ProgEnv::EditSourceOf(class Class*, bool, int)
{
}

void ProgEnv::ShowInHierarchy(class Class*)
{
}

void ProgEnv::ShowInheritancePath(class Class*)
{
}
    
void ProgEnv::InspectObject(class Object*, bool)
{
}

void ProgEnv::ShowInObjStruBrowser(class Object*)
{
}

char *ProgEnv::HasAbstractView(Class *, void *, Object *)
{
    return 0;    
}

void ProgEnv::ShowAbstractView(Class *, void *, Object *, Object *)
{
}

void ProgEnv::AddAbstractViewPolicy(AbstractViewPolicy *avh)
{
    SafeDelete(avh);
}

char *AbstractViewPolicy::ViewName(Class *, void*, Object *)
{
    return 0;    
}

bool AbstractViewPolicy::ShowView(Class *, void*, Object *, Object *)
{
    return FALSE;
}
