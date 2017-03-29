#ifndef TextFormatter_First
#ifdef __GNUG__
#pragma once
#endif
#define TextFormatter_First

#include "Object.h"

//---- abstract class Formatter ------------------------------------------

class TextFormatter: public Object {
protected:
    int startedAt;
    class StaticTextView *tv;
    bool isPreemptive;
public:
    MetaDef(TextFormatter);
    TextFormatter(StaticTextView *tv);
    virtual DoIt(int fromLine, int upto= cMaxInt, int minUpto= 0);
    int StartedAt()
	{ return startedAt; }
    void Preemptive(bool p)
	{ isPreemptive= p; }
    virtual void ResumeFormat(bool preemptive= FALSE);
};

//---- SimpleFormatter, break lines at '\n' -----------------------------

class SimpleFormatter: public TextFormatter {
public:
    MetaDef(SimpleFormatter);
    SimpleFormatter(StaticTextView *tv);
    int DoIt(int fromLine, int upto= cMaxInt, int minUpto= 0);
    void ResumeFormat(bool preemptive= FALSE);
};

//---- FoldingFormatter, fold lines (preemptive) -------------------------

class FoldingFormatter: public TextFormatter {
    class Text *text;
    int start, end, width, nWords, startLine, line, lastpeek, wx, cx;
    int preemptedAt;
    bool wasPreempted;
protected:
    int Format(int fromLine, int fromCh, int upto, int minUpto);
    void BreakWord(class LineDesc *maxld);
    bool Suspend(int);
public:
    MetaDef(FoldingFormatter);
    FoldingFormatter(StaticTextView *tv);
    int DoIt(int fromLine, int upto= cMaxInt, int minUpto= 0);
    void ResumeFormat(bool preemptive= FALSE);
};

#endif TextFormatter_First
