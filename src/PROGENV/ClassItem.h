#ifndef ClassItem_First
#define ClassItem_First

#include "VObject.h"

//---- ClassItem ---------------------------------------------------------------

enum ClassItemFlags {
    eClItemCollapsed =   BIT(eVObjLast+1)
};

class ClassItem: public TextItem {
public:
    class Class *cl;
    
public:
    MetaDef(ClassItem);    
    ClassItem(int id, Class *cl, char *label= 0);
    Class *GetClass()
	{ return cl; }
    char *ClassName()
	{ return cl->Name(); }
    int Compare(ObjPtr);
    bool IsEqual(ObjPtr);
    
    Metric GetMinSize();
    void Draw(Rectangle r);
};

//---- ObjectItem --------------------------------------------------------------

const int cObjItemId= 1001;

class ObjectItem: public TextItem {
    Object *op;
public:
    MetaDef(ObjectItem);    
    ObjectItem(int id, Object *op);
    ObjectItem(int id, char *, Object *op);
    ObjectItem(char *, Object *op);
    Object *GetObject()
	{ return op; }
};


#endif ClassItem

