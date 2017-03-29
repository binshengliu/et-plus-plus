#ifndef ShellTView_First
#ifdef __GNUG__
#pragma once
#endif
#define ShellTView_First

#include "FixLineTView.h"
#include "PttyConnection.h"
#include "CmdNo.h"

extern Rectangle gShellTextViewRect;
extern char *gDefaultArgv[];

//---- ShellTextView --------------------------------------------------------
// a TextView connected to a ptty running a shell

class ShellTextView: public FixedLineTextView {
    class PttyInputHandler *pttyinp;
    class ShellZombieHandler *zombieHandler;
    class PttyConnection *pttycon;
    class Mark *outputCursor, *inputCursor;
    bool doReveal;
    class CheapText *noEchoBuffer;  // used when echo is turned off
    int lastSubmit;
    int state;          // for interpreting ESC-Sequences
    bool pendingCR;     
    PttyChars pttyc;    // tty-chararacters that submit a command
    char *pttycstr;
    char **argv;

    void Init(bool doReveal, char *name, char **argv, bool cons);
    void Term();
    bool ModifiesReadOnly();
    void Wrap();
    void BackSpace(Text *t);
    void CarriageReturn(Text *t);
    void ProcessOutput(Text *t, char *buf, int size);
    void MarksToEOT();
    void DrawOutputCursor(Point p);
protected:
    virtual void InsertReceivedText(Text *t, int from, int to);
public:
    MetaDef(ShellTextView);

    ShellTextView(EvtHandler *eh, Rectangle r, Text *t, char *name= 0,
	    char **args= 0, bool becomeconsole= FALSE, eTextJust m= eLeft, 
		eSpacing= eOne, TextViewFlags= (TextViewFlags)eVObjDefault, 
		    Point border = gBorder, int id = -1);    
	    // for efficency reasons do not set contentRect.extent.x to cFit
    ~ShellTextView();
    /* virtual */ char *GetTermCap();
    /* virtual */ char *GetTerm();
    void BecomeConsole();
    void Received();
    void Submit(char *buf, int size); 
    void SubmitInterrupt(char c); 
    void SlaveDied();
    void CleanupDeath();
    Command *CntrlChar(byte b);   
    void DoReveal(bool);    
    void Draw(Rectangle);   // draw output cursor
    Text *SetText(Text *);     // returns old text       
    void SetTtySize(int rows, int cols);
    void SetTtySize(Rectangle r, Font *fd);
    char **GetArgv()
	{ return argv; }
    void Clear();
    void Reconnect();
    void Doit();
    Command *TtyInput(char *buf, int len= -1); // appends buf to ttys input queue

    //---- overridden to maintain a fence of read only text
    bool DeleteRequest(int from, int to);
    void Paste(Text *insert);
    //---- controller methods 
    Command *DoKeyCommand(int, Point, Token); 
    void DoSetupMenu(class Menu *);
    Command *DoMenuCommand(int);
    void SelectAll();
    //---- input/output
    ostream& PrintOn (ostream&);
    istream& ReadFrom(istream&);
};

#endif ShellTView_First  
