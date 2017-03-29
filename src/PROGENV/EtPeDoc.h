#ifndef EtPeDoc_First
#define EtPeDoc_First

#include "Document.h"
#include "ByteArray.h"

//---- EtPeDoc ------------------------------------------------------------

class EtPeDoc: public Document {
    class VObject *commands;
    
    int srcCount, hierCount, ostruCount, inspCount, fihCount;
    class HierarchyBrowser *hierBrowser;
    class ObjectBrowser *objBrowser;
    class Inspector *inspector;
    class SourceBrowser *browser;
    class FihView *fihBrowser;
    class Window *inspWin, *hierWin, *objWin, *fihWin;
    class PathLookup *universe;
    class PeMethodAccessor *methodAccessor;
    
    char *MakeTitle(int count, char *label);

public:
    MetaDef(EtPeDoc);
    EtPeDoc();
    ~EtPeDoc();

    Window *DoMakeWindows();
    bool Modified();
    class Icon *DoMakeIcon(char *name);
    
    void Control(int id, int part, void *val);
    void DoObserve(int, int what, void *, Object* op);

    void HierarchyViewerShow(Class *cp);
    void HierarchyViewerSpawn(Class *cp);
    void ObjectBrowserSpawn(Object *op);
    void ObjectBrowserShow(Object *op);
    void InspectorShow(Object *op, bool block);
    void InspectorSpawn(class Ref *r, bool block);
    void SrcBrowserShow(Class *cp, bool decl, int at= -1);
    void FlatInheritanceSpawn(Class *cl);
    void FlatInheritanceShow(Class *cl);

    //---- universe of source files
    class PathLookup *Universe();
    bool FileOfClass(Class *cl, char *fname, bool decl);
    
    //---- method access
    bool SameMethods(char *m1, char *m2);
    class Collection *LoadMethodsOfClass(Class *cl);
    bool IsDestructor(char *m1);

    void Parts(Collection* col);
};

//---- MethodReference ---------------------------------------------------------

class MethodReference: public ByteArray {
    Class *clp;
    int line;
    bool isPublic;
public:
    MetaDef(MethodReference);    
    MethodReference(int line, Class *clp, char *method, bool isPublic);
    bool IsEqual(Object *);
    int Line()
	{ return line; }
    Class *GetClass()
	{ return clp; }
    bool IsPublic()
	{ return isPublic; }
};

//---- entry points ------------------------------------------------------------

extern EtPeDoc *gEtPeDoc;
extern void MakeEtPeDoc();

#endif EtPeDoc_First
