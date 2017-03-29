#ifndef ErShapes_First
#define ErShapes_First

#include "VObject.h"

//--- ErShape --------------------------------------------------------------------

class ErShape: public CompositeVObject {
protected:
    class TextView *textview;

public:
    MetaDef(ErShape);
    
    ErShape(char *label);
    ~ErShape();
    
    void Init(View *vp);
    void SetLabel(class Text *t);
    virtual bool CanConnectWith(ErShape*)
	{ return FALSE; }
    
    //---- layout
    void SetOrigin(Point at);
    Metric GetMinSize();
    Point Center()
	{ return contentRect.Center(); }
    void DoObserve(int id, int part, void *vp, Object *op);
	
    //---- rendering
    void Draw(Rectangle);
    
    //---- event handling
    Command *Input(Point p, Token t, Clipper *vf);
    
    //---- input/output
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

//---- Entity ------------------------------------------------------------------

class Entity: public ErShape {
public:
    MetaDef(Entity);
    
    Entity(char *label= "Entity") : (label)
	{ }
    void Draw(Rectangle r);
    bool CanConnectWith(ErShape *s);
};

//---- Relation ----------------------------------------------------------------

class Relation: public ErShape {
public:
    MetaDef(Relation);
    
    Relation(char *label= "Relation") : (label)
	{ }
    void Draw(Rectangle r);
    bool CanConnectWith(ErShape *s);
};

//--- Connection ---------------------------------------------------------------

class Connection: public ErShape {
    ErShape *startShape, *endShape;

public:
    MetaDef(Connection);
    
    Connection(ErShape*, ErShape*);
    ~Connection();
    
    Metric GetMinSize();
    void SetOrigin(Point at);
    bool ContainsPoint(Point p);
    void UpdatePoints();
    void Draw(Rectangle);
    void DoObserve(int id, int part, void *vp, Object *op);
    
    ostream &PrintOn(ostream &s);
    istream &ReadFrom(istream &s);
};

#endif ErShapes_First

