#include <sys/types.h>

#ifdef sun
#include <sgtty.h>
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#include <sys/stat.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <sys/param.h>
#include <pwd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/file.h>

typedef int bool;
#define TRUE 1
#define FALSE 0

extern int errno;
extern char *sys_errlist[];
extern char *sys_siglist[];

extern char *malloc();
extern void free();

typedef void (*sigHandlerType)();

int sunwindow_select(nfds, readready, writeready, timeout)
unsigned int nfds, *readready, *writeready, timeout;
{
    extern int select();
    
    int retcode;
    struct timeval tv;
    
    tv.tv_sec= timeout / 1000;
    tv.tv_usec= (timeout % 1000) * 1000;
    retcode= select(nfds, readready, writeready, 0, &tv);
    if (retcode == -1) {
	if (errno == EINTR)
	    return -2;
	return -1;
    }
    return retcode;
}

#define MAXSIGNALS 11

struct signal_map {
    int code;
    int (*handler)();
} signal_map[MAXSIGNALS]= {
    SIGBUS, 0, 
    SIGSEGV, 0, 
    SIGSYS, 0, 
    SIGPIPE, 0, 
    SIGILL, 0, 
    SIGQUIT, 0, 
    SIGINT, 0, 
    SIGWINCH, 0, 
    SIGALRM, 0, 
    SIGCHLD, 0, 
    SIGURG, 0
};

static void sighandler(sig)
int sig;
{
    int i;
    
    for (i= 0; i < MAXSIGNALS; i++)
	if (signal_map[i].code == sig) {
	    (*signal_map[i].handler)(i);
	    return;
	}
}

void sunsystem_signal(sig, handler)
int sig;
int (*handler)();
{
    struct signal_map *sm= &signal_map[sig];
    sm->handler= handler;
    signal(sm->code, sighandler);
}

void sunsystem_resetsignal(sig)
int sig;
{
    struct signal_map *sm= &signal_map[sig];
    signal(sm->code, SIG_DFL);
}

char *sunsystem_signame(sig)
int sig;
{
    return sys_siglist[signal_map[sig].code];    
}

char *workingdirectory()
{
    static char path[MAXPATHLEN];

    if (getwd(path) == 0) {
	strcpy(path, "/"); 
	Warning("workingdirectory", "getwd failed");
    }
    return path;
}

char *homedirectory()
{
    static char path[MAXPATHLEN];
    struct passwd   *pw;

    if ((pw= getpwuid(getuid())) == 0) 
	return 0;
    strncpy(path,pw->pw_dir,MAXPATHLEN);
    return path;
}

char *getdirentry(dirp)
DIR *dirp;
{
#ifdef sun
    struct dirent *dp;
#else
    struct direct *dp;
#endif sun
     
    if (dirp && (dp= readdir(dirp)))
	return dp->d_name;
    return 0;
}

void *getstatbuf(path)
char *path;
{
    struct stat *statbuf;
    
    if (path != 0) {
	statbuf= (struct stat*) malloc(sizeof (struct stat));
	if (statbuf == NULL)
	    return NULL;
	if (stat(path, statbuf) < 0)
	    return NULL;
	return (void*) statbuf;
    }
    return NULL;
}

void freestatbuf(statbuf)
struct stat *statbuf;
{
    if (statbuf)
	free(statbuf);
}

int uniquefileid(statbuf)
struct stat *statbuf;
{    
    return statbuf->st_dev + statbuf->st_ino;
}

int isexecutable(statbuf)
struct stat *statbuf;
{
    if (statbuf->st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
	return 1;
    return 0;
}

int isdirectory(statbuf)
struct stat *statbuf;
{
    if ((statbuf->st_mode & S_IFMT) == S_IFDIR ) 
	return 1;
    return 0;
}

int isspecial(statbuf)
struct stat *statbuf;
{
    if ((statbuf->st_mode & S_IFMT) != S_IFREG && !isdirectory(statbuf))
	return 1;
    return 0;
}

long filesize(statbuf)
struct stat *statbuf;
{
    return (long) statbuf->st_size;   
}

int sunsystem_GetTtyChars(backspace, rubout)
char *backspace, *rubout;
{
    struct sgttyb ttyBuf;
    struct tchars tcharBuf;
    struct ltchars ltcharBuf;
    int fd;

    if ((fd= open("/dev/tty", O_RDONLY)) < 0) {
	fprintf(stderr, "GetTtyChars: can't open control terminal");
	return 1;
    }

    if (ioctl(fd, (int) TIOCGETP, (char*) &ttyBuf) == -1 ||
	    ioctl(fd, (int) TIOCGETC, (char*) &tcharBuf) == -1 ||
		ioctl(fd, (int) TIOCGLTC, (char*) &ltcharBuf) == -1) {
	close(fd);
	return 1;
    }    
    close(fd);

    *backspace= ttyBuf.sg_erase;
    *rubout= tcharBuf.t_intrc;
    return 0;
}

static jmp_buf buf;

void sunsystem_LongJmp()
{
    longjmp(buf, 1);
}

typedef void (*CallFunc)();
static int retcode;

int sunsystem_Call(cf, p1, p2, p3, p4)
CallFunc cf;
char *p1, *p2, *p3, *p4;
{
    retcode= 0;
    
    if (setjmp(buf))
	retcode= 1;
    else
	(*cf)(p1, p2, p3, p4);
    /*if (retcode)
	fprintf(stderr, "sunsystem_Call: %d\n", retcode);*/
    return retcode;
}

/* expand the metacharacters as in the shell */

static char *shellMeta   = "~*[]{}?$",
	    *shellStuff  = "(){}<>\"'",
	    shellEscape = '\\';

char *expandpathname(patbuf, buflen)
char *patbuf;
int buflen;
{
    char cmd[170], StuffedPat[200], name[70];
    register char *p, *q;
    FILE *pf;
    struct passwd *pw;
    int ch, i;

    /* skip leading blanks */
    while (*patbuf == ' ')
	patbuf++;

    /* any shell meta characters ? */
    for (p= patbuf; *p; p++)
	if (index(shellMeta, *p))
	    goto needshell;

    return 0;

needshell:    
    /* escape shell quote characters */
    StuffChar(patbuf, StuffedPat, sizeof StuffedPat, shellStuff, shellEscape);

    strcpy(cmd, "echo -n ") ;
    strcat(cmd, " ");

    /* emulate csh -> popen executes sh */
    if (StuffedPat[0] == '~') {
	if (StuffedPat[1] != '\0' && StuffedPat[1] != '/') {
	    /* extract user name */
	    for (p= &StuffedPat[1], q= name; *p && *p !='/';)
		*q++= *p++;
	    *q = '\0';
	    if ((pw= getpwnam(name)) == NULL) 
		strcat(cmd, StuffedPat); 
	    else {
		strcat(cmd, pw->pw_dir);            
		strcat(cmd, p);
	    }           
	} else {
	    if ((pw= getpwuid(getuid())) == NULL)
		return sys_errlist[errno];
	    strcat(cmd, pw->pw_dir);            
	    strcat(cmd, &StuffedPat[1]);
	}
    } else
	strcat(cmd, StuffedPat);

    if ((pf= popen(&cmd[0], "r")) == NULL)
	return sys_errlist[errno];

    /* read first argument */
    for (i= 0, ch= getc(pf); ch != EOF && ch != ' '; ch= getc(pf)) {
	if (i == buflen-1)
	    break;
	patbuf[i++]= ch;
    }
    patbuf[i]= '\0';

    /* overread rest of pipe */
    while (ch != EOF) {
	ch= getc(pf);
	if (ch == ' ' || ch == '\t')
	    return "expression ambigous";
    }

    pclose(pf);

    return 0;
}

int waitchild()
{
    union wait status;
    
    return wait3(&status, WNOHANG, 0);
}

int myread(ifp, millisec, b, size)
FILE *ifp;
int millisec, size;
void *b;
{
    int rval, fd;
    fd_set readready;
    struct timeval t, *tv= &t;
    
    fd= fileno(ifp);
		   
    if (millisec < 0) /* block */
	tv= 0;
    else if (millisec == 0) /* poll */
	t.tv_sec= t.tv_usec= 0L;
    else {
	t.tv_sec= (long) (millisec / 1000);
	t.tv_usec= (long) ((millisec % 1000) * 1000);
    }
    for (;;) {
	FD_ZERO(&readready);
	FD_SET(fd, &readready);
	if ((rval= select(fd+1, &readready, 0, 0, tv)) < 0) {
	    if (errno != EINTR)
		perror("select");
	    continue;
	}
	if (rval == 0) /* timeout */
	    return 1;
	if (FD_ISSET(fd, &readready)) {
	    rval= fread(b, size, 1, ifp);
	    if (rval == 0) {
		if (millisec < 0)
		    Error("myread", "server died");
		return 1;
	    }
	    return 0;
	}
    }
}

#ifdef sony

char *tempnam(dir, pfx)
char *dir, *pfx;
{
    return "/tmp/et";
}

#endif sony

