//$ClassManager,CumSortedObjList$
#include "ClassManager.h"
#include "Error.h"
#include "Storage.h"
#include "String.h"
#include "ObjArray.h" 
#include "OrdColl.h"
#include "SortedOList.h"
#include "Set.h"
#include "ObjectTable.h"

ClassManager *gClassManager;

static bool sorted;

//---- ClassManager ------------------------------------------------------------

MetaImpl(ClassManager, (TP(classTable), 0));

ClassManager::ClassManager()
{
    classTable= 0;
    nClasses= 0;
}

ClassManager::~ClassManager()
{
    if (this == gClassManager)
	gClassManager= 0;
    SafeDelete(classTable);
}

void ClassManager::Add(Class *cl)
{
    if (classTable == 0) {
	classTable= new ObjArray(170);
	ObjectTable::AddRoot(classTable);
    }
    classTable->Add(cl);
    nClasses++;
    sorted= FALSE;
    // Changed();
}

void ClassManager::Remove(Class*)
{
    nClasses--;
}

Class *ClassManager::Find(char *name)
{
    Class lookFor(name);
    
    if (! sorted) {
	classTable->Sort();
	sorted= TRUE;
    }
    int at= classTable->BinarySearch(&lookFor, nClasses);
    if (at != -1)
	return (Class*) classTable->At(at);
    return 0;
}

ostream& ClassManager::SavePtr(ostream& o, ObjPtr p)
{
    if (p == 0)
	return o << "0 ";
    p->IsA()->SavePtr(o, p);
    return o;
}

void ClassManager::InvalidatePtr(ObjPtr p)
{
    if (p == 0)
	return ;
    p->IsA()->InvalidatePtr(p);
}

void ClassManager::SetDynLoadHook(Object *op, OObjMemberFunc of)
{
    dynLoadPrivData= op;
    DynLoadHook= of;
}

/* Read in new object, put a pointer to it in *op.
*  Make sure object read in is cl or a descendant of cl.
*/
istream& ClassManager::LoadPtr(istream& is, Object *&op, const Class *cl)
{
    char c, name[100];
    register Class *clp;    

    op= NULL;
    is >> c;
    if (c == '0')
	return is;
    if (c != '{') {
	Error("LoadPtr", "missing \'{\', got: \'%c\'", c);
	SetStatus(4);
	return is;
    }
    is >> name;
    while ((clp= Find(name)) == 0) {
	if ((DynLoadHook != 0) && dynLoadPrivData)
	    if ((dynLoadPrivData->*DynLoadHook)(name))
		continue;       // try again

	Error("LoadPtr", "can't find class %s", name);
	SetStatus(6);
	return is;
    }
    if (!clp->isKindOf((Class*)cl))
	Error("LoadPtr", "trying to read a %s into a %s", clp->Name(), cl->Name());
    op= clp->LoadPtr(is);
    return is;
}
 
int ClassManager::Reset()
{
    ObjArrayIter next(classTable);
    register Class *clp;
    int oldstatus= status; 

    while (clp= (Class*) next())
	clp->Reset();
    status= 0;
    return oldstatus;
}

ostream &ClassManager::DisplayOn(ostream &os)
{
    return classTable->DisplayOn(os);
}

void ClassManager::SetupSubclasses()
{
    ObjArrayIter next(classTable);
    register Class *clp, *super;
    Set classes;

    if (! sorted) {
	classTable->Sort();
	sorted= TRUE;
    }
    while (clp= (Class*) next()) 
	if (clp->subclasses) 
	    clp->subclasses->Empty();

    next.Reset(classTable);
    while (clp= (Class*) next()) 
	if (super= clp->Super())
	    super->AddSubclass(clp);
}

//---- sort list according number of instances of a class --------------------

class CumSortedObjList : public SortedObjList {

public:
    CumSortedObjList()
	{}
    int CompareObjects(Object *, Object *);
};
    
int CumSortedObjList::CompareObjects(Object *op1, Object *op2)
{
    Class *cp1= Guard(op1, Class), *cp2= Guard(op2, Class);
    return cp2->GetInstanceCount() - cp1->GetInstanceCount();
}

void ClassManager::InstanceStatistics(bool cumsorted)
{
    class Iterator *iter= 0;
    Class *clp;
    int cum= 0;
    CumSortedObjList csl;
    
    ObjectTable::UpdateInstCount();
    if (cumsorted) {
	csl.AddAll(classTable);
	iter= csl.MakeIterator();
    }
    else {
	iter= classTable->MakeIterator();
	if (! sorted) {
	    classTable->Sort();
	    sorted= TRUE;
	}
    }
    Iter next(iter);
    cout << "Number of Instances per Class " NL;
    cout << "=============================" NL;
    while (clp= (Class*) next()) {
	if (clp->className && clp->instanceCount > 0) {
	    cout << form("%-20s:\t%4d\n", clp->className, clp->GetInstanceCount());
	    cum+= clp->instanceCount;
	}
    }
    cout << "-----------------------------" NL;
    cout << "Total " << cum NL;
    cout << "=============================" NL;
    cout.flush();
}
 
Iterator *ClassManager::Iterator(bool)
{
    if (! sorted) {
	classTable->Sort();
	sorted= TRUE;
    }
    return classTable->MakeIterator();    
}

