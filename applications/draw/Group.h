#ifndef Group_First
#define Group_First

#include "Shape.h"

//---- Group -------------------------------------------------------------------

class Group : public Shape {
    class ObjList *group;
public:
    MetaDef(Group);
    
    Group(class ObjList *g= 0);
    ~Group();
    void SetContainer(VObject *vop);
    ObjList *Ungroup();
    void Draw(Rectangle);
    void Highlight(HighlightState h);
    ShapeStretcher *NewStretcher(class DrawView*, int);
    ostream& PrintOn(ostream&);
    istream& ReadFrom(istream&);
};

#endif Group_First
