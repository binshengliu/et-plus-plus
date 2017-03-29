#include "SharedDocObjects.h"
#include "IdDictionary.h"
#include "OrdColl.h"
#include "Document.h"
#include "Error.h"
#include "System.h"

static IdDictionary *map= 0;

void SharedDocObjects::AddRef(Object *op, Document *doc)
{
    if (map == 0)
	map= new IdDictionary();

    Collection *refDocs= (Collection*) map->AtKey(op);
    if (refDocs == 0) {
	refDocs= new OrdCollection();
	map->AtKeyPut(op, refDocs);
    }
    refDocs->Add(doc);
}

void SharedDocObjects::RemoveRef(Object *op, Document *doc)
{
    if (map == 0)
	map= new IdDictionary();
    Collection *refDocs= (Collection*) map->AtKey(op);
    if (refDocs == 0) {
	Error("SharedDocObjects::RemoveRef", "object not found");
	return;
    }
    if (refDocs->RemovePtr(doc) == 0) {
	Error("SharedDocObjects::RemoveRef", "Document not found");
	return;
    }
    if (refDocs->IsEmpty()) {
	Assoc *ap= (Assoc*)map->RemoveKey(op);
	SafeDelete(ap);
	SafeDelete(refDocs);
    }
}

int SharedDocObjects::RefCount(Object *op)
{
    if (map == 0)
	map= new IdDictionary();
    Collection *refDocs= (Collection*) map->AtKey(op);
    if (refDocs) 
	return refDocs->Size();
    return 0;
}  

void SharedDocObjects::Delete(Object *op, Document *doc)
{
    RemoveRef(op, doc);
    if (RefCount(op) == 0)
	SafeDelete(op);
}

void SharedDocObjects::DeleteDelayed(Object *op, Document *doc)
{
    RemoveRef(op, doc);
    if (RefCount(op) == 0)
	gSystem->AddCleanupObject(op);
}

int SharedDocObjects::RefId(Object *op, Document *dp)
{
    SeqCollection *refDocs= (SeqCollection*) map->AtKey(op);
    if (refDocs)
	return refDocs->IndexOfPtr(dp);
    Error("SharedDocObjects::MakeDocumentIter", "object not found");
    return 0;
}  
    
Iterator *SharedDocObjects::MakeDocumentIter(Object *op)
{
    SeqCollection *refDocs= (SeqCollection*) map->AtKey(op);
    if (refDocs)
	return refDocs->MakeIterator();
    Error("SharedDocObjects::MakeDocumentIter", "object not found");
    return 0;
}
