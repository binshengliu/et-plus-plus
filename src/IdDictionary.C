//$IdDictionary,IdSet$
#include "IdDictionary.h"

//---- IdDictionary ------------------------------------------------------------

MetaImpl0(IdDictionary);

IdDictionary::IdDictionary(int s) : Dictionary(s)
{
}

u_long IdDictionary::HashObject(Object *op)
{
    if (op->IsDeleted()) 
	return op->Hash();
    return (u_long) ((Assoc*)op)->Key();
}
    
bool IdDictionary::ObjectsEqual(Object *op1, Object *op2)
{
    return ((Assoc*)op1)->Key() == ((Assoc*)op2)->Key();    
}

Set* IdDictionary::Keys()
{
    register ObjPtr op;
    register SetPtr sp= new IdSet;    
    DictIterKeys next(this);
    
    while (op= next())
	sp->Add(op);
    return sp;
}

//---- IdSet -------------------------------------------------------------------

MetaImpl0(IdSet);

IdSet::IdSet(int s) : Set(s)
{
}

u_long IdSet::HashObject(Object *op)
{
    return ((unsigned int) op);
}
    
bool IdSet::ObjectsEqual(Object *op1, Object *op2)
{
    return op1 == op2;    
}


