//$EtProgEnv, AbstractView$

#include "EtProgEnv.h"
#include "OrdColl.h"
#include "Application.h"
     
#include "EtPeDoc.h"

int gTrace= 0;
extern void AbstractViewOnEntry();

//---- global entry point ----------------------------------------------------

ProgEnv *NewEtProgEnv()
{
    gProgEnv= new EtProgEnv();
    AbstractViewOnEntry();
    return gProgEnv;
}

//---- EtProgEnv ------------------------------------------------------------

MetaImpl(EtProgEnv, (TP(abstViewPolicies), TP(pe),0));

EtProgEnv::EtProgEnv()
{
    abstViewPolicies= new OrdCollection();
}
    
EtProgEnv::~EtProgEnv()
{
    abstViewPolicies->FreeAll();
    SafeDelete(abstViewPolicies);
}

void EtProgEnv::Start()
{
    MakeEtPeDoc();
}

void EtProgEnv::EditSourceOf(Class *cp, bool decl, int at)
{
    MakeEtPeDoc();
    gEtPeDoc->SrcBrowserShow(cp, decl, at);
}

void EtProgEnv::ShowInHierarchy(Class *cp)
{
    MakeEtPeDoc();
    gEtPeDoc->HierarchyViewerShow(cp);
}

void EtProgEnv::ShowInheritancePath(Class *cp)
{
    MakeEtPeDoc();
    gEtPeDoc->FlatInheritanceShow(cp);
}

   
void EtProgEnv::InspectObject(Object *op, bool block)
{
    MakeEtPeDoc();
    gEtPeDoc->InspectorShow(op, block);
} 

void EtProgEnv::AddAbstractViewPolicy(AbstractViewPolicy *avh)
{
    abstViewPolicies->Add(avh);   
}

char *EtProgEnv::HasAbstractView(Class *cl, void *addr, Object *op)
{
    RevIter prev(abstViewPolicies);
    AbstractViewPolicy *avh;
    char *name;
    while (avh= (AbstractViewPolicy *)prev())
	if (name= avh->ViewName(cl, addr, op))
	    return name;
    return 0; 
}

void EtProgEnv::ShowAbstractView(Class *cl, void *addr, Object *op, Object *insp)
{
    RevIter prev(abstViewPolicies);
    AbstractViewPolicy *avh;
    while (avh= (AbstractViewPolicy*)prev())
	if (avh->ShowView(cl, addr, op, insp))
	    break;
}

void EtProgEnv::ShowInObjStruBrowser(Object *op)
{
    MakeEtPeDoc();
    gEtPeDoc->ObjectBrowserShow(op);
}

void EtProgEnv::MakeEtPeDoc()
{
    if (gEtPeDoc == 0) {
	pe= gEtPeDoc= new EtPeDoc;
	gApplication->AddDocument(gEtPeDoc);
	gEtPeDoc->OpenWindows();
    }
}
