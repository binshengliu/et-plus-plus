//$RunArray,RunArrayIter$
#include "RunArray.h"
#include "Error.h"
#include "String.h"

const cRunArrExpandFactor = 2;   // increment of expansion during Shift

static char *cOutOfRangeMsg= "out of range from= %d to= %d length= %d";

inline bool OutOfRange(int from, int to, int len = cMaxInt)
{
    return  ( from < 0 || from > to || to > len );
}

inline bool HighWaterMark(int level, int size)
{
    return ( level > size );
}

inline bool LowWaterMark(int level, int size)
{
    return ( level < size/4 -1 );
}

//---- class RunArray ----------------------------------------------------------

MetaImpl(RunArray, (TVP(cont, count), TV(runlength, count), T(size),
    T(count), T(length), T(current), T(offset), TP(nullrun),
    T(nullpos), 0));

//---- public methods ----------------------------------------------------------         

RunArray::RunArray(int elements)
{
    cont= new ObjPtr[size = elements];
    runlength= new int[size];
    length= current= offset= count= nullpos = 0;
    nullrun= 0;
}

RunArray::~RunArray()
{
    SafeDelete(cont);
    SafeDelete(runlength);                                            
}

void RunArray::OutOfRangeError(char *where, int at)
{
    Error(where, "out of range at %d (length %d)", at, length);
}

ObjPtr RunArray::Add(ObjPtr op)
{
    int len= 1;
    if (op == 0){
	Error("Add", "op == 0");
	return 0;
    }
    InsertRuns(length,length, &op, &len, 1);
    length++;
    return 0;
}

ObjPtr RunArray::Remove(ObjPtr op)
{
    ObjPtr tmp= 0;
    MoveTo(0);
    while( !IsInRun(length) && !cont[current]->IsEqual(op) )
	NextRun();
    if (offset < length) {
	tmp= cont[current];
	Cut(offset, offset+1);
    }
    return tmp;
}

Iterator *RunArray::MakeIterator()
{
    return new RunArrayIter(this);
}

int RunArray::ContainsPtrAt (ObjPtr op)
{
    register int i, pos;
    for (i= pos= 0; i < count; i++, pos += runlength[i])
	if (op == cont[i])
	    return pos;
    return -1;
}

int RunArray::ContainsAt(ObjPtr op)
{
    register int i, pos;
    for (i= pos= 0; i < count; i++, pos += runlength[i])
	if (op->IsEqual(cont[i]))
	    return pos;
    return -1;
}

ObjPtr& RunArray::operator[](int i)
{
    if (OutOfRange(i, length-1))
	OutOfRangeError("operator[]", i);
    MoveTo(i);
    return cont[current];    
}

void RunArray::Insert(ObjPtr op, int from, int to, int len, bool free)
{
    if (OutOfRange(from, to, length)) {
	Error("Insert", cOutOfRangeMsg, from, to, length);
	return;
    }
    if (len < 0 || op == 0) {
	Error("Insert", "op == 0");
	return;
    }
    if (len == 0 ){
	// store the run
	Cut(from, to, free);
	nullrun= op;
	nullpos= from;
    } else { 
	InsertRuns(from, to, &op, &len, 1, free);
	length+= len - to + from;
    }
}

void RunArray::ChangeRunSize(int i, int shift, bool free)
{
    if (OutOfRange(i,length) || -shift > i) {
	OutOfRangeError("ChangeRunSize", i);
	return;
    }
    if (shift > 0 && i == nullpos && nullrun != 0) {
	// shift run with length 0
	InsertRuns(i, i, &nullrun, &shift, 1, free);
	length+= shift;
    } else {
	nullrun= NULL;
	MoveTo(i);
	if (i == offset && i > 0) 
	    // |xxxxx|yyyyy|     runs grow at left margin !!
	    //       v    
	    // |xxxxxv|yyyyy|
	    PrevRun();
	if (i + shift >= offset) {
	    runlength[current]+= shift;
	    length+= shift;
	} else
	    Cut(i + shift, i , free);
    }
}

void RunArray::Cut(int from, int to, bool free)
{
    ObjPtr op= 0;

    if (cont[0])
	op= cont[0]->DeepClone(); // preserve first item of runarray ???

    if (OutOfRange(from, to, length)) {
	Error("Cut", cOutOfRangeMsg, from, to, length);
	return;
    }

    InsertRuns(from, to, 0, 0, 0, free);
    length-= to - from;
    if (length == 0 && op) {
	nullrun= op;
	nullpos= 0;
    }
}

void RunArray::Paste(RunArray *paste, int from, int to, bool free)
{
    if (OutOfRange(from, to, length)) {
	Error("Paste", cOutOfRangeMsg, from, to, length);
	return;
    }
    InsertRuns(from, to, paste->cont, paste->runlength, paste->count, free);
    length+= from - to + paste->length;
}

void RunArray::Copy(RunArray *save, int from, int to)
{
    int left, leftlen;

    if (OutOfRange(from, to, length)) {
	Error("Copy", cOutOfRangeMsg, from, to, length);
	return;
    }
    if (save == 0) {
	Warning("Copy", "save == 0");
	return;
    } 

    MoveTo(from);
    left= current;
    leftlen= from - offset;
    MoveTo(max(0, to-1));
    save->InsertRuns(0, save->Size(), cont+left, runlength+left, current-left +1);
    save->length= to - from;
    if (leftlen)
	save->runlength[0]-= leftlen;
    if (to < length)
	save->runlength[save->count-1]-= EndOfRun() - to;
}

void RunArray::FreeAll()
{
    Cut(0, Size(), TRUE);
}

RunArrayPtr RunArray::Save(int from, int to)
{
    RunArray *save;

    if (OutOfRange(from, to, length)) {
	Error("Save", cOutOfRangeMsg, from, to, length);
	return 0;
    }
    save= new RunArray;
    Copy(save, from, to);
    return save;
}

ObjPtr RunArray::RunAt(int i, int *start, int *end, int *runsize, int *lenat)
{
    if (OutOfRange(i, length-1)) {
	OutOfRangeError("RunAt", i);
	return 0;
    }
    MoveTo(i);
    *start= offset;
    *end= EndOfRun();
    *runsize= runlength[current];
    *lenat= *end - i;
    return cont[current];
}

int RunArray::LengthAt(int i)
{
    if (OutOfRange(i, length-1)) {
	OutOfRangeError("LengthAt", i);
	return 0;
    }
    MoveTo(i);
    return EndOfRun() - i;
}

int RunArray::RunSizeAt(int i)
{
    if (OutOfRange(i, length-1)) {
	OutOfRangeError("RunSizeAt", i);
	return 0;
    }
    MoveTo(i);
    return runlength[current];
}

bool RunArray::IsEqual(ObjPtr op)
{
    if (!op->IsKindOf(RunArray))
	return FALSE;

    RunArray *t= (RunArray *) op; 

    if (t->count != count || t->length !=length)
	return FALSE;
    register int i;
    for (i= 0; i < count; i++) {
	if (cont[i] == 0 || t->cont[i] == 0) {
	    Error("IsEqual", "empty objects found");
	    break;
	}
	if (runlength[i] != t->runlength[i] || !cont[i]->IsEqual( t->cont[i] ))
	    break;
    }
    return i == count;
}

ostream& RunArray::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    s << count SP;
    for (int i = 0; i < count; i++)
	s << cont[i] SP << runlength[i] SP;
    return s;
}

istream& RunArray::ReadFrom(istream& s)
{
    current = offset = length = 0;
    Object::ReadFrom(s);
    s >> count;
    if (size < count) {
	delete cont;
	delete runlength;
	cont= new ObjPtr[size= count];
	runlength= new int[count];
    }
    length = current = offset = 0;
    for (int i= 0; i < count; i++) {
	cont[i]= ReadItem(s);
	s >> runlength[i];
	length += runlength[i];
    }
    return s;
}

Object *RunArray::ReadItem(istream& s)
{
    Object *op;
    s >> op;
    return op;
}

ostream& RunArray::DisplayOn(ostream&s)
{
    s << "[ ";
    for (int i = 0; i < count; i++) {
	if (i != 0)
	    s << ", ";
	if (cont[i]) {
	    cont[i]->DisplayOn(s);
	    s << ":" << runlength[i];
	} else
	    s << "NIL";
    }
    return s << " ]";
}

void RunArray::Dump()
{
    cerr << "size   : " << size NL;
    cerr << "count  : " << count NL;
    cerr << "length : " << length NL;
    cerr << "current: " << current NL;
    cerr << "offset : " << offset NL;

    for (int i = 0 ; i < count ; i++){
	cerr << runlength[i] << " : " ;
	cont[i]->DisplayOn(cerr);
	cerr << "\n";;
    }
}

//---- private methods ---------------------------------------------------------

inline void RunArray::CopyRuns(ObjPtr *desto, int *desti, int from, int to, int len)
{
    BCOPY((byte*) &cont[from], (byte*) &desto[to], len * sizeof(ObjPtr));
    BCOPY((byte*) &runlength[from] , (byte*) &desti[to], len * sizeof(int)); 
}

void RunArray::MoveTo(int to)
{
    if (IsInRun(to))
	return;
    // find best start   
    if (to < offset) {
	if (to < offset/2) 
	    current= offset = 0;
    } else {
	if (to > (length+offset)/2) {
	    current= count -1;
	    offset= length - runlength[current];
	}
    }
    if (to < offset)
	// go left
	while (!IsInRun(to))
	    PrevRun(); 
    else 
	while (current < count  && !IsInRun(to))
	    NextRun(); 
}  

void RunArray::Shift(int at, int shift)
{
    ObjPtr *tmpop;
    int *tmpint;

    if (shift == 0)
	return;
    if (-shift > at) {
	OutOfRangeError("Shift", at);
	return;
    }
    if (HighWaterMark(count + shift, size)) {
	// expand
	if (size)
	    size= max(size * cRunArrExpandFactor,count + shift);
	else
	    size= cCollectionInitCap;
	tmpop= new ObjPtr[size];
	tmpint= new int[size];
	CopyRuns(tmpop, tmpint, 0, 0, at);
	CopyRuns(tmpop, tmpint, at, at + shift, count - at);
	delete cont;
	delete runlength;
	cont= tmpop; 
	runlength= tmpint;
    } else {
	if (LowWaterMark(count + shift, size)) {
	    //--- shrink
	    size= size / cRunArrExpandFactor;
	    tmpop= new ObjPtr[size];
	    tmpint= new int[size];
	    CopyRuns(tmpop, tmpint, 0, 0, at + shift);
	    CopyRuns(tmpop, tmpint, at, at + shift, count - at);
	    delete cont;
	    delete runlength;
	    cont= tmpop;
	    runlength= tmpint;
	} else 
	    CopyRuns(cont, runlength, at, at + shift, count -at); 
    }        
    count+= shift;
}

void RunArray::InsertRuns(int from, int to, ObjPtr *value, 
					    int *run, int inssize, bool free)
{
    // Insert 'inssize' runs pointed to by 'value' between 'from' and 'to'

    int insert= inssize;
    int rightindex, rightlen, leftindex, leftlen;
    int i, j, firstlen= -1, lastlen= -1;
    ObjPtr blankobj= 0;
    int    blankint= 0;

    if (!inssize) {      
	// nothing to insert
	MoveTo(to);
	if (IsInRun(from) && to - from < runlength[current]) {
	    runlength[current] -= to - from;
	    return;
	}
	value= &blankobj;      
	run= &blankint;
	if ( to < EndOfRun()) {
	    // set 'value' and 'run' to run right of 'to' and move right to the end of this run
	    value[0] = cont[current];
	    run[0] = EndOfRun() - to;
	    to = EndOfRun();
	    inssize = insert = 1;       
	}
    }

    MoveTo(max(from-1,0));
    leftindex = current;            // index of run left of 'from'
    leftlen = from - offset;        // new length of run left of 'from'

    // delete runs between from and to
    while (!IsInRun(to)) { 
	if (offset >= from) {
	    if (free) {
		for (i = 0; i < inssize && cont[current] != value[i]; i++)
		    ;
		if (i == inssize)
		    delete cont[i];
	    }
	    insert--;
	}
	NextRun() ;
    }

    rightindex = current;           // index of run right of 'to'
    rightlen = EndOfRun() - to;     // new length of run right of 'to'

    current = leftindex;
    offset = from - leftlen;
    if (inssize){
	firstlen = run[0];          // save length of first and last run 
	lastlen = run[inssize-1];   // to insert
    }
    if ( leftlen && rightlen && leftindex == rightindex )
	// insert new runs into one run ==> split this run
	insert++;
    if ( inssize && leftlen && cont[leftindex]->IsEqual(value[0]) ){
	// left run is equal to the first run to insert
	// add his length to it
	run[0] += leftlen;
	leftlen = 0;
	insert--;
    }
    if ( inssize && rightlen && cont[rightindex]->IsEqual(value[inssize-1]) ){
	// right run is equal to the last run to insert
	// add his length to it
	run[inssize-1] += rightlen;
	rightlen = 0;
	insert--;  
    }

    Shift(rightindex, insert);
    i = leftindex;

    // adjust length of left and right run  and insert new runs
    if ( leftlen ){
	if ( leftindex == rightindex )
	    // we had to split this run
	    cont[i] = cont[leftindex + insert]->DeepClone();
	runlength[i] = leftlen;
	i++;
    }
    for ( j = 0; j < inssize; i++, j++ ){
	cont[i] = value[j]->DeepClone();
	runlength[i] = run[j];
    }
    if (rightlen)
	runlength[i] = rightlen;

    if (inssize){
	// restore length of first and last run to insert
	run[0] = firstlen;                  
	run[inssize-1] = lastlen;
    }
    nullrun = 0;
} 

void RunArray::InspectorId (char *buf, int)
{
    sprintf(buf, "Size %d", length);   
}

//---- RunArrayIter ------------------------------------------------

void RunArrayIter::Reset(Collection *s)
{
    cs= (RunArray*)s;
    ce= ci= cp= 0;
}

ObjPtr RunArrayIter::operator()()
{
    if (ci >= cs->length)
	return (0);
    if (ci >= cp + cs->runlength[ce]){
	cp += cs->runlength[ce];
	ce++;
    }
    ci ++;
    return cs->cont[ce];
}

ObjPtr RunArrayIter::Run(int *start, int *end, int *size)
{
    if (ci >= cs->length)
	return (0);

    *start = cp;
    *size = cs->runlength[ce];
    *end = *start + *size;
    cp += *size;
    ci = cp;
    return cs->cont[ce++];
}

ObjPtr *RunArrayIter::RunPtr(int *start, int *end, int *size)
{
    if (ci >= cs->length)
	return (0);

    *start = cp;
    *size = cs->runlength[ce];
    *end = *start + *size;
    cp += *size;
    ci = cp;
    return &cs->cont[ce++];
}

bool RunArrayIter::Filter(ObjPtr)
{
    return TRUE;
}
