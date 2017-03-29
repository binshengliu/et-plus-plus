#include "System.h"
#include "SUNOS/dynlink.h"

main(int argc, char *argv[], char *env)
{
    DynLinkInit(argv[0], gEtDir);
    DynLoad(argv[1]);
    DynCall("main")(argc-1, &argv[1], env);
    return 0;
}

