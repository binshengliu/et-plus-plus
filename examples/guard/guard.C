//$A,B$
#include "Object.h"

extern void ETInit();

class A: public Object {
public:
    MetaDef(A);

    A()
      {}
    void bar()
      { cerr << "bar\n"; }
};

MetaImpl0(A);

class B: public A {
public:
    MetaDef(B);

    B()
      {}
    void foo()
      { cerr << "foo\n"; }
};

MetaImpl0(B);

main()
{
    ETInit();
    Object *o= new A;

    Guard(o, A)->bar();
    Guard(o, B)->foo();
}

