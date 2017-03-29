#ifndef InspItem_First
#define InspItem_First

#include "VObject.h"

//---- InspectorItem0 ---------------------------------------------------------------

class InspectorItem0: public TextItem {
protected:
    char *name;
    bool died;
public:
    MetaDef(InspectorItem0);
    
    InspectorItem0(char *t, GrFace fc);
    ~InspectorItem0();
    virtual void Deref(bool expanded);
    virtual void Update();
    virtual void Died();
    void Draw(Rectangle);
};

//---- SuperItem0 ---------------------------------------------------------------

class SuperItem0: public InspectorItem0 {
public:
    MetaDef(SuperItem0);
    SuperItem0(char *name, GrFace fc= eFaceItalic);
    void Update();
    void Deref(bool expanded);
};

//---- ClassItem0 ---------------------------------------------------------------

class ClassItem0: public SuperItem0 {
    void      *addr;
public:
    MetaDef(ClassItem0);
    ClassItem0(void *addr, char *name, GrFace fc= eFaceBold);
    void Update();
};

//---- InspectorItem ---------------------------------------------------------------

class InspectorItem: public InspectorItem0 {
    class Ref *ref;
    
public:
    MetaDef(InspectorItem);
    
    InspectorItem(char *n, Ref *r);
    ~InspectorItem();
    void Deref(bool expanded);
    void Update();
};

#endif InspItem_First

