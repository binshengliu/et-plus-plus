#ifndef CollTblView_First
#define CollTblView_First

#include "CollView.h"

//---- CollTableView ------------------------------------------------------------

class CollTableView: public CollectionView {
protected:
    class ObjectItem *AsObjectItem(Object *op);
public:
    CollTableView(int cols= 1);
    void DoSelect(Rectangle r, int clicks);
    virtual void ShowCollection(class Collection*);
};

class DictionaryView: public CollTableView {
public:
    DictionaryView();
    void ShowCollection(class Collection*);
};


#endif CollTblView_First

