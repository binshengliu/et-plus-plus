#include "Dictionary.h"
#include "Bag.h"
#include "Error.h"

//---- a dictionary is implemented as a set of key,value associations 

MetaImpl(Assoc, (TP(key), TP(value), 0));

void Assoc::FreeAll()
{
    if (key) {
	key->FreeAll();
	delete key;
    }
    if (value) {
	value->FreeAll();
	delete value;
    }
}

unsigned long Assoc::Hash()
{
    return key->Hash();
}

void Assoc::operator= (ObjPtr aValue)
{
    if (value) {
	value->FreeAll();
	delete value;
    }
    value = aValue;
}

bool Assoc::IsEqual(ObjPtr anOp)
{
    return anOp->IsKindOf(Assoc) && key->IsEqual(((Assoc*) anOp)->key);
}

int Assoc::Compare(ObjPtr anOp)
{
    return key->Compare(((Assoc*) anOp)->key);
}

ostream& Assoc::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    return s << key SP << value SP;
}
  
istream& Assoc::ReadFrom(istream& s)
{
    Object::ReadFrom(s);
    return s >> key >> value;
}

//---- Dictionary ------------------------------------------------------------

MetaImpl0(Dictionary);

Dictionary::Dictionary(int s) : Set(s)
{
}

Dictionary::~Dictionary()
{
    DictIter next(this);
    ObjPtr op;

    while(op = next())
	SafeDelete(op);
}

void Dictionary::FreeAll()
{
    DictIter next(this);
    ObjPtr op;

    while(op= next())
	if (op)
	    op->FreeAll();
}

void Dictionary::FreeValues()
{
    DictIter next(this);
    Assoc *ap;
    ObjPtr op;

    while(ap= (Assoc*)next()) {
	if (ap) {
	    op= ap->Value();
	    SafeDelete(op);
	}
    }
    Set::Empty(0);
}

void Dictionary::Empty(int s)
{
    DictIter next(this);
    ObjPtr op;

    while(op= next())
	if (op)
	    SafeDelete(op);
    Set::Empty(s);
}

bool Dictionary::AssertAssoc(char *where, ObjPtr op)
{
    if (!op->IsKindOf(Assoc)) {
	Error(where, "only Assoc's are allowed (%s)", op->ClassName());
	return FALSE;
    }  
    return TRUE;
}

ObjPtr Dictionary::Add(ObjPtr op)
{
    if (CheckNotNull("Add", op))
	return 0;
    if (!AssertAssoc("Add", op))
	return 0;  
    Assoc *tmp= (Assoc*) op;
    Assoc *dp= (Assoc*) Find(tmp);
    
    if (dp == 0)
	return Set::Add(tmp);
    return dp->SetValue(tmp->Value());  
}

ObjPtr Dictionary::Remove(ObjPtr op)
{
    if (op == 0)
	return 0;
    if (!AssertAssoc("Remove", op))
	return 0;  
    return Set::Remove(op);    
}

void Dictionary::AtKeyPut(ObjPtr key,ObjPtr value)
{
    Assoc *dp;
	
    if (key == 0)
	return;
    if (value == 0) {
	Error("AtKeyPut", "association with 0 not allowed");
	return;
    }
    dp = AssociationAtKey(key);
    
    if (dp == 0)
	Set::Add (new Assoc (key,value));
    else
	dp->SetValue(value);
}

ObjPtr Dictionary::AtKey(ObjPtr key)
{
    Assoc *dp = AssociationAtKey(key);
    return (dp == 0) ? 0 : dp->Value();
}

Assoc* Dictionary::AssociationAtKey(ObjPtr key)
{
    Assoc tmp(key, 0);
	
    return (key == 0) ? 0 : (Assoc*) Find(&tmp);
}

ObjPtr Dictionary::RemoveKey(ObjPtr key)
{
    return Remove(AssociationAtKey(key));
}

bool Dictionary::Contains(ObjPtr op)                                         
{
    return Collection::Contains(op);
}

bool Dictionary::ContainsAssoc (ObjPtr op)
{
    if (!AssertAssoc("ContainsAssoc", op))
	return FALSE;  
    if (op == 0)
	return (FALSE);        

    return (Assoc*)Find(op) != 0;
}

bool Dictionary::ContainsKey (ObjPtr op) 
{
    return AssociationAtKey(op) != 0;
}

int Dictionary::OccurrencesOfKey (ObjPtr op)
{
    return (AssociationAtKey(op) != 0) ? 1 : 0;
}

SetPtr Dictionary::Keys()
{
    register ObjPtr op;
    register SetPtr sp = new Set;    
    DictIterKeys next(this);
    
    while (op = next())
	sp->Add(op);
    return sp;
}

BagPtr Dictionary::Values()
{
    register ObjPtr op;
    register BagPtr bp = new Bag;    
    DictIterValues next(this);
    
    while (op = next())
	bp->Add(op);
    return (bp);
}

//---- DictIterValues ----------------------------------------------------------

DictIterValues::DictIterValues(Collection *s) : SetIter(s)
{
}

ObjPtr DictIterValues::operator()()
{        
    Assoc *ap= (Assoc*) SetIter::operator()();
    
    return (ap == 0) ? 0 : ap->Value();
}

//---- DictIterKeys ------------------------------------------------------------

DictIterKeys::DictIterKeys(Collection *s) : SetIter(s)
{
}

ObjPtr DictIterKeys::operator()()
{  
    Assoc *ap= (Assoc*)SetIter::operator()();
    return (ap == 0) ? 0 : ap->Key();
}

