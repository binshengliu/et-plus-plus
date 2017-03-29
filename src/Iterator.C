//$Iterator$

#include "Iterator.h"
#include "Collection.h"
#include "Error.h"

Iterator::Iterator()
{
    started= terminated= FALSE;    
}
  
Iterator::~Iterator()
{
}

void Iterator::Reset(class Collection*)
{
    IteratorEnd();
    terminated= started= FALSE;        
}

Object *Iterator::operator()()
{
    return 0;
}

Collection *Iterator::Coll()
{
    AbstractMethod("Iterator::Coll");
    return 0;
}

void Iterator::IteratorStart()
{ 
    if (!started) {
	Coll()->EnterIter();
	started= TRUE; 
    }
}

void Iterator::IteratorEnd()
{
    if (started && !terminated) {
	terminated= TRUE;
	Coll()->ExitIter(); 
    } 
}


