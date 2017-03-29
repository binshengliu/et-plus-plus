#ifndef FlatInhView_First
#define FlatInhView_First

#include "CollView.h"
#include "ObjArray.h"

//---- Class table for Flat Inheritance View ----------------------------------

class FihClassTable: public Object {
    class ObjArray *table;
    int maxlevel, nClasses; 
public:
    MetaDef(FihClassTable);
    FihClassTable();
    ~FihClassTable();
    void AtPut(int row, int col, Object *op)
	{ table->AtPut(nClasses*row + col, op); } 
    Object *At(int row, int col)
	{ return table->At(nClasses*row + col); } 
    VObject *MakeCell(Class *clp, bool bold= FALSE); 
    int Rows()
	{ return nClasses; }
    int Cols()
	{ return maxlevel; }
    Collection *Table()
	{ return table; }
    
};

//---- FihView --------------------------------------------------------------

class FihView: public CollectionView {
    FihClassTable* ft;
public:
    MetaDef(FihView);
    FihView(FihClassTable* f, EvtHandler *next);
    void DoSelect(Rectangle r, int clicks);
    void ShowClass(class Class *);
    Menu *GetMenu();
};

#endif FlatInhView_First

