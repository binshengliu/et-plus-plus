#ifndef AnnotatedItem_First
#define AnnotatedItem_First

#include "DialogItems.h"

class AnnotatedItem : public ImageButton {
    class VObjectText *msg;
    void Init(char *msg);
public:
    MetaDef(AnnotatedItem);
    
    AnnotatedItem(char *message);
    AnnotatedItem();
    ~AnnotatedItem();
    
    void DoOnItem(int, VObject*, Point);
    void SetMessage(VObjectText *m);
    VObjectText *GetMessage();
    GrCursor GetCursor(Point);
    ostream& PrintOn (ostream&s);
    istream& ReadFrom(istream &s);  
};


#endif AnnotatedItem_First
