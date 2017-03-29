#ifndef SharedDocObjects_First
#define SharedDocObjects_First

#include "Object.h"

//---- SharedDocObjects ------------------------------------------------------------

class Document; 
class Iterator;

// SharedDocObject manages an IdDictionary. Key= Object Value= Collection of
// Documents displaying the SharedDocObjects

class SharedDocObjects {
public:
    static void AddRef(Object *op, Document *);
    static void RemoveRef(Object *op, Document *);
    static void Delete(Object *op, Document *);
    static void DeleteDelayed(Object *op, Document *);
    static int RefCount(Object *op);   
    static int RefId(Object *op, Document*);
    static Iterator *MakeDocumentIter(Object *op);
};

#endif SharedDocObjects_First

