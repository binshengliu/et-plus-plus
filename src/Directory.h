#ifndef Directory_First
#ifdef __GNUG__
#pragma once
#endif
#define Directory_First

#include "Root.h"

//---- Directory ---------------------------------------------------------------

class Directory : public Root {
public:
    Directory(char*)
	{ }
    virtual ~Directory();
    virtual char *operator()();
};

#endif Directory_First
