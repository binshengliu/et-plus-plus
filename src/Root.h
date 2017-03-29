#ifndef Root_First
#ifdef __GNUG__
#pragma once
#endif
#define Root_First

class Root {
protected:
    Root();
public:
    virtual class Class *IsA();
    
#ifdef __GNUG__
    char rdummy;
#endif
};

#endif Root_First
