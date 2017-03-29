#include "AssocArray.h"

extern void ETInit();

main()
{
    ETInit();
    
    AssocArray a;
    
    a["test"]= "ttt";
    a["german"]["blue"]= "blau";
    a["english"]["blue"]= "blau";
    a["french"]["blue"]= "bleu";
    a["1"]["2"]["3"]["4"]["5"]["6"]["7"]["8"]= "wow!";
    
    cerr << a["test"] NL;
    cerr << a["german"]["blue"] NL;
    cerr << a["english"]["blue"] NL;
    cerr << a["french"]["blue"] NL;
    cerr << a["1"]["2"]["3"]["4"]["5"]["6"]["7"]["8"] NL;
}

