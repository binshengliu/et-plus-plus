//$DefaultItem,FihView,FihClassTable$
  
#include "OrdColl.h"
#include "ObjList.h"
#include "ClassManager.h"
#include "String.h"

#include "EtPeCmdNo.h"
#include "EtPeDoc.h"
#include "FlatInhView.h"
#include "EtProgEnv.h"

//---- default Item for FihView ----------------------------------------------

class DefaultItem: public VObject {
public:
    MetaDef(DefaultItem);
    DefaultItem()
	{ Disable(FALSE); }
    void Draw(Rectangle )
	{ GrPaintRect(contentRect, ePatGrey25); }
};

MetaImpl0(DefaultItem);

//---- FihClassTable ----------------------------------------------------------

MetaImpl0(FihClassTable);

FihClassTable::FihClassTable()
{
    // find out the depth of the inheritance path
    Class *clp;
    int level; 
    nClasses= maxlevel= 0;
    
    Iter next(gClassManager->Iterator());
    while (clp= (Class*)next()) {
	nClasses++; 
	level= 1;      
	while (clp->Super() != 0) {
	    level++;
	    clp= clp->Super();
	}
	maxlevel= max(level, maxlevel);
    }
    // fill in table with inheritance path
    next.Reset(0);
    
    int i,j;
    table= new ObjArray((maxlevel+1)*(nClasses));
    for (i= 0; clp= (Class*)next(); i++) {
	AtPut(0, i, MakeCell(clp, TRUE));
	j= 1;
	while (clp->Super() != 0) {
	    clp= clp->Super();
	    AtPut(j++, i, MakeCell(clp));
	}
	while (j < maxlevel)
	    AtPut(j++, i, new DefaultItem());
    }
}

FihClassTable::~FihClassTable()
{   
    SafeDelete(table);
}

VObject *FihClassTable::MakeCell(Class *clp, bool bold) 
{ 
    GrFace fc= clp->IsAbstract() ? eFaceItalic : eFacePlain;
    
    if (bold)
	fc= (GrFace) (fc | eFaceBold);
    return new TextItem(clp->Name(), gFixedFont->WithFace(fc));
} 

//---- FihView ---------------------------------------------------------------

MetaImpl(FihView, (TP(ft), 0));

FihView::FihView(FihClassTable* f, EvtHandler *e)
      : CollectionView(e, f->Table(), eCVDontStuckToBorder, f->Rows(), f->Cols())
{ 
    ft= f; 
}

void FihView::DoSelect(Rectangle r, int)
{
    VObject *gop= (VObject*) ft->At(r.origin.x, r.origin.y);
    Class *clp= gClassManager->Find(gop->AsString());
    if (clp)
	Control(cIdBrowser, -1, clp);
}

void FihView::ShowClass(Class *cl)
{   
    char *name= cl->Name();
    for (int i= 0; i < ft->Rows(); i++) {
	VObject *vop= (VObject*)ft->At(0,i);
	if (vop && (strcmp(vop->AsString(), name) == 0)) {
	    RevealRect(vop->ContentRect().Expand(100), Point(0,100));
	    SetSelection(Rectangle(0, i, ft->Cols(), 1));      
	}
    }
}

Menu *FihView::GetMenu()
{
    return 0;
}

