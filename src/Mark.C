//$Mark,MarkList$
#include "Mark.h"

MetaImpl(Mark, (T(pos), T(len), T(state), 0));

Mark::Mark(int p, int l, eMarkState s, eMarkFlags flags)
{
    pos = p;
    len = l;
    state= s;
    if ((flags & eMarkInclStart) == eMarkInclStart)
	SetFlag(eMarkInclStart);
    if ((flags & eMarkFixedSize) == eMarkFixedSize)
	SetFlag(eMarkFixedSize);
    if ((flags & eMarkLocked) == eMarkLocked)
	SetFlag(eMarkLocked);
}

Mark::Mark(Mark *m)
{
    pos= m->pos;
    len= m->len;
    state= m->state;
}

int Mark::Compare(Object *op)   
{
    return (pos - ((Mark *)op)->pos);
}

bool Mark::HasChanged (int start,int l)
{
    return (state != eStateNone || pos != start || len != l);
}

ostream &Mark::PrintOn(ostream& s)
{
    Object::PrintOn(s);
    return s << pos SP << len SP << state SP;
}

istream &Mark::ReadFrom(istream& s)
{
    Object::ReadFrom(s);
    return s >> pos >> len >> Enum(state);
}

ostream &Mark::DisplayOn(ostream& s)
{
    return s << "pos =  " << pos << "\tlen = " << len << "\t" << state NL;
}

Object *Mark::DeepClone()
{
    return new Mark(pos, len, (eMarkState)state);
}

//---- class MarkList --------------------------------------------------------

MetaImpl(MarkList, (TB(doRemove)));

MarkList::MarkList(bool remove)
{   
    doRemove= remove;
}

void MarkList::Paste(int at,int n)
{
    Iter next(this);
    Mark *m;

    n= max(0, n);
    at= max(0, at);

    while (m = (Mark*)next()) {
	if (m->TestFlag(eMarkLocked))
	    continue;
	if ((at > m->pos || (m->TestFlag(eMarkInclStart) && at == m->pos))
						    && at < m->pos + m->len) {
	    m->state = eStateChanged;
	    m->len += n;
	    //m->ChangedWhat((void*) eMarkLength);
	} else if (at < m->pos || (!m->TestFlag(eMarkInclStart) && at == m->pos)) {
	    m->pos += n;
	    //m->ChangedWhat((void*) eMarkPos);
	}
    }
}

void MarkList::Cut(int at,int n)
{
    Iter next(this);
    Mark *m;

    if (at < 0)
	n += at;
    at = max(0,at);

    // the different cases are shown as: '|' = mark positions '^' deleted range
    while (m = (Mark*)next()) {

	if (m->TestFlag(eMarkLocked))
	    continue;

	if (m->TestFlag(eMarkFixedSize)) {
	    // ^ |    |   ^ includes ^ |   ^|
	    if (at <= m->pos && at + n >= m->pos + m->len) {
		m->state = eStateDeleted;
		m->len = 0;
		m->pos = at;
		//m->ChangedWhat((void*) eMarkDeleted);
		if (doRemove) {
		    Remove(m);
		    m->FreeAll();
		    delete m;
		}
	    }
	    // ^ ^ |    | includes ^  ^|   |
	    else if (at + n <= m->pos) {
		m->pos -= n;
		//m->ChangedWhat((void*) eMarkPos);
	    }

	    //    |    |  ^  ^ includes |    |^  ^
	    else if (at >= m->pos + m->len)
		;
	    // ^ | ^  |,  |  ^ | ^,| ^ ^ | includes |^   ^|
	    else {  
		Error("Cut", "should not occur");
	    }           

	} else {           
	    // | ^ ^ | includes |^   ^|
	    if (at >= m->pos && at + n <= m->pos + m->len) { // contained
		m->state = eStateChanged;
		m->len -= n;
		//m->ChangedWhat((void*) eMarkLength);
	    }

	    // ^ |    |   ^ includes ^ |   ^|
	    else if (at < m->pos && at + n >= m->pos + m->len) {
		m->state = eStateDeleted;
		m->len = 0;
		m->pos = at;
		//m->ChangedWhat((void*) eMarkDeleted);
		if (doRemove) {
		    Object *tmp= Remove(m);
		    delete tmp;
		}
	    }

	    // ^ | ^  |
	    else if (at < m->pos && at + n > m->pos) {
		m->len = m->pos+m->len - (at +n); 
		m->pos = at;
		m->state = eStateChanged;
		//m->ChangedWhat((void*) eMarkPos);
	    }

	    // |  ^ | ^
	    else if (at >= m->pos && at  < m->pos + m->len) {
		m->len = at - m->pos;
		m->state = eStateChanged;
		//m->ChangedWhat((void*) eMarkLength);
	    }

	    // ^ ^ |    | includes ^  ^|   |
	    else if (at + n <= m->pos) {
		m->pos -= n;
		//m->ChangedWhat((void*) eMarkPos);
	    }

	    //    |    |  ^  ^ includes |    |^  ^
	    else if (at >= m->pos + m->len)
		;
	    else    
		Error("Cut", "should not occur");  
	}          
    }
}

void MarkList::RangeChanged(int at,int n)
{
    Iter next(this);
    Mark *m;

    n = max(0,n);
    at = max(0,at);

    while (m = (Mark*)next()) {
	if (m->TestFlag(eMarkLocked))
	    continue;
	if (at < m->pos && at+n >= m->pos+m->len)
	    m->state = eStateChanged;
	else if (at >= m->pos && at < m->pos+m->len)
	    m->state = eStateChanged;
	else if (at+n >= m->pos && at+n < m->pos+m->len)
	    m->state = eStateChanged;
    }
}

ostream& MarkList::PrintOn (ostream&s)
{
    OrdCollection::PrintOn(s);
    return s << doRemove SP;
}

istream& MarkList::ReadFrom(istream &s)
{
    OrdCollection::ReadFrom(s);
    return s >> Bool(doRemove);
}
