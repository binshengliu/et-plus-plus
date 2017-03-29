#ifndef sunsystem_First
#define sunsystem_First

#include <stdio.h>
#include <osfcn.h>
#include <sys/types.h>

#define MAXSIGNALS 11

#ifndef FD_SET
#   define FD_SET(n, p)    (*(p) |= (1 << (n)))
#   define FD_CLR(n, p)    (*(p) &= ~(1 << (n)))
#   define FD_ISSET(n, p)  (*(p) & (1 << (n)))
#   define FD_ZERO(p)      (*(p) = 0)
    typedef u_int fd_set;
#endif

typedef void (*sigHandlerType)(Signals);

extern "C" {
    int uniquefileid(struct stat*);
    struct stat *getstatbuf(char *path);
    void freestatbuf(struct stat *);
    int isexecutable(struct stat*);
    int isdirectory(struct stat*);
    int isspecial(struct stat*);
    long filesize(struct stat*);
    void *opendir(char *name);
    void closedir(void*);
    char *getdirentry(void*);
    char *workingdirectory();
    char *homedirectory();
    char *expandpathname(char*, int);
    int  waitchild();
    int myread(FILE*, int, void*, int);
    int sunsystem_Call(CallFunc cf, void *p1, void *p2, void *p3, void *p4);
    void sunsystem_LongJmp();
    int sunsystem_GetTtyChars(char *backspace, char *rubout);
    int sunwindow_select(u_int nfds, fd_set *readready, fd_set *writeready, u_int timeout);
    void sunsystem_signal(Signals, sigHandlerType);
    void sunsystem_resetsignal(Signals);
    char *sunsystem_signame(Signals);
}

#endif sunsystem_First

