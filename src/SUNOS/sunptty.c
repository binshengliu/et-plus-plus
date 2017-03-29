/*
 * This file is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.
 * Users may copy, modify or distribute this file at will.
 * 
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * This file is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY THIS FILE
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even
 * if Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 *
 * Modifications to the original Sun Microsystems, Inc. source code
 * made by the Grasshopper Group are in the Public Domain.
 *
 * Extensions to this file by Eric Messick of the Grasshopper Group.
 *
 * Grasshopper Group
 * 212 Clayton St
 * San Francisco, CA 94117
 *
 * Adapted for ET++ at ifi unizh
 *
 */
/*
 * Copyright (c) 1985 by Sun Microsystems, Inc. 
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <utmp.h>
#include <pwd.h>
#include <sgtty.h>
#include <sys/file.h>
#include <fcntl.h>
#include <signal.h>

extern void exit();
extern unsigned sleep();
extern char *getenv();
static void get_tty_util();
static int GetTTYDefaults();
static int SetTTYState();
static int rmut();

extern char *strsave();

#define utmpf "/etc/utmp"
#define wtmpf "/usr/adm/wtmp"
#define PTCNAME "/dev/ptyXX"
#define PTSNAME "/dev/ttyXX"

/* XXX - there should be a library routine to get a pty */
static char ptcname[sizeof(PTCNAME) + 1];
static char ptsname[sizeof(PTSNAME) + 1];
extern int errno;

/* Tty mode statics */
static struct tchars tc;
static struct ltchars ltc;
static struct sgttyb sg;
static int lm;                 /* localmodes */
static int ld;                 /* ldisc */

/* Default settings for tty mode stuff */
static  struct tchars tc_df =
    { CINTR, CQUIT, CSTART, CSTOP, CEOF, CBRK };
static  struct ltchars ltc_df =
    { CSUSP, CDSUSP, CRPRNT, CFLUSH, CWERASE, CLNEXT };
static  struct sgttyb sg_df =
    { B9600, B9600, CERASE, CKILL, ANYP|ECHO|CRMOD };
static  int lm_df = LCRTBS|LCRTERA|LCRTKIL|LCTLECH;
static  int ld_df = NTTYDISC;

static int CharsPerLine= 80;
static int LinesPerScreen= 24;

struct PttyChars {
    char erase;
    char kill;
    char rprnt;
    char susp;
    char intr;
    char quit;
    char eof;
};

/*---- interface functions ----------------------------------------------*/

#define MAXDELAY 120

FILE *sunPttySpawnSlave(name, args, slavepid, pttyname, pc, tslot)
char *name, **args, **pttyname;
int *slavepid, *tslot;
struct PttyChars *pc;
{
    int Mfd= -1, Sfd= -1, i, pid, uid, gid, pidchild;
    FILE *Master;
    unsigned delay= 2;
    char *gp, *tp;
    struct sigvec vec, ovec;
	
    strcpy(ptcname, PTCNAME);
    strcpy(ptsname, PTSNAME);
    
    GetTTYDefaults();
    for (gp= "pqrst"; *gp; gp++)
	for (tp= "0123456789abcdef"; *tp; tp++) {
	    ptcname[sizeof(PTCNAME)-3]= *gp;
	    ptcname[sizeof(PTCNAME)-2]= *tp;
	    if ((Mfd= open(ptcname, 2)) < 0)
		continue;
	    ptsname[sizeof(PTCNAME)-3]= *gp;
	    ptsname[sizeof(PTCNAME)-2]= *tp;
	    if ((Sfd= open(ptsname, 2)) >= 0)
		goto done;
	    close(Mfd);
    }
done:
    if (Mfd < 0 || Sfd < 0)
	return 0;
    uid= getuid();
    gid= getgid();
    chown(ptsname, uid, gid);            /* set tty ownership */
    chmod(ptsname, 0622);                /* set tty protection */
    /*
     * Establish tty state.
     */
    SetTTYState(Sfd);

    Master= fdopen(Mfd, "r+");
    while ((pid= vfork()) < 0 && errno == EAGAIN) {
	sleep(delay);
	if ((delay <<= 1) > MAXDELAY) {
	    close (Mfd);
	    close (Sfd);
	    return 0;
	}
    }
    if (pid == 0) {
	/*
	 * Setup controlling tty.
	 */
	vec.sv_handler= SIG_DFL;
	vec.sv_mask= vec.sv_onstack= 0;
	sigvec(SIGWINCH, &vec, 0);

	pidchild= getpid();

	vec.sv_handler= SIG_IGN;
	vec.sv_mask= vec.sv_onstack= 0;
	sigvec(SIGTTOU, &vec, &ovec);

	if (ioctl(Sfd, TIOCSPGRP, &pidchild) == -1)
	    perror("ioctl(TIOCSPGRP)");
	setpgrp(pidchild, pidchild);
	
	/* under SUNOS 4.1 we have to create a new session */
#ifdef TIOCSETPGRP 
	if (setsid() == -1)
	    perror("setsid");
#endif TIOCSETPGRP

	sigvec(SIGTTOU, &ovec, 0);

	for (i= 0; i < 3; i++)
	    dup2(Sfd, i);
	for (i= getdtablesize(); i > 2; i--)
	    close(i);
	    
	setuid(uid);
	setgid(gid);

	execvp(name, args);
	SysError("sunPttySpawnSlave", "execvp(%s)", name);
	exit(errno);
    }
    *slavepid= pid;
    fcntl(Mfd, F_SETFL, fcntl(Mfd, F_GETFL, 0) | FNDELAY);
    close(Sfd);
    
    if (pttyname)
	*pttyname= strsave(ptsname, -1);
    
    if (pc) {
	pc->erase= sg.sg_erase;
	pc->kill= sg.sg_kill;
	pc->rprnt= ltc.t_rprntc;
	pc->susp= ltc.t_suspc;
	pc->intr= tc.t_intrc;
	pc->quit= tc.t_quitc;
	pc->eof= tc.t_eofc;
    }
    vec.sv_handler= SIG_IGN;
    vec.sv_mask= vec.sv_onstack= 0;
    sigvec(SIGTSTP, &vec, 0);
    sigvec(SIGTTOU, &vec, 0);
    
    return Master;
}

sunPttyCleanupPtty(ptsname, tslot)
char *ptsname;
int tslot;
{
    rmut(ptsname, tslot);
    chown(ptsname, 0, 0);                /* revoke ownership */
    chmod(ptsname, 0666);                /* revoke protection */
}

sunPttyKillChild(pgrp)
int pgrp;
{
    if (pgrp != 0)
	killpg(pgrp, SIGTERM);
}

sunPttyBecomeConsole(ptsname)
char *ptsname;
{
#ifdef TIOCCONS
    int fd;
    if ((fd= open(ptsname, O_RDWR, 0)) > 0) {
	if (ioctl(fd, TIOCCONS, 0) < 0)
	    SysError("sunPttyBecomeConsole", "sunOsPtty(TIOCCONS)");
    } else 
	printf("Cannot become console %s\n", ptsname);
    close(fd);
#endif
}

void sunPttySetSize(slavename, rows, cols)
char *slavename;
int rows, cols;
{
#ifdef TIOCSWINSZ
    int fd;
    if ((fd= open(slavename, O_RDWR, 0)) > 0) {
	struct winsize ws;
	ws.ws_row = rows; ws.ws_col = cols;
	ioctl(fd, TIOCSWINSZ, &ws);    
    } else 
	printf("Cannot set window size %s\n", ptsname);
    close(fd);
#endif
}

static GetTTYDefaults()
{
    int fd;

    /* Get settings of controlling terminal */
    fd= open("/dev/tty", O_RDWR);
    get_tty_util(fd);
    if (fd > 0)
	close(fd);
    /*
     *   Try to get the tty parameters from stderr (2).
     *   Using stdin (0) fails when being started in the background
     *     because csh redirects stdin from the tty to /dev/null.
     */
    fd= 2;
    if (!isatty(fd))
	fd= open("/dev/console", 2);
    get_tty_util(fd);
    if (fd != 2)
	close(fd);
}

static void get_tty_util(fd)
int fd;
{
    if (fd > 0) {
	if(ioctl(fd, TIOCGETP, &sg) == -1)
	    sg= sg_df;
	(void) ioctl(fd, TIOCGETC, &tc);
	if (tc.t_quitc == 0)
	    tc= tc_df;
	(void) ioctl(fd, TIOCGETD, &ld);
	if (ld == 0)
	    ld= ld_df;
	(void) ioctl(fd, TIOCLGET, &lm);
	if (lm == 0)
	    lm= lm_df;
	(void) ioctl(fd, TIOCGLTC, &ltc);
	if (ltc.t_suspc == 0)
	    ltc= ltc_df;
	(void) ioctl(fd, TIOCNOTTY, 0);
    } else {
	/* Last resort...use some default values */
	tc= tc_df;
	ld= ld_df;
	lm= lm_df;
	ltc= ltc_df;
	sg= sg_df;
    }
}

static SetTTYState(fd)
int fd;
{
    ioctl(fd, TIOCHPCL, 0);
    ioctl(fd, TIOCSETD, &ld);
    sg.sg_flags|= XTABS;
    if ((sg.sg_flags & ECHO) == 0) 
	sg.sg_flags = sg_df.sg_flags;
    ioctl(fd, TIOCSETP, &sg);
    ioctl(fd, TIOCSETC, &tc);
    ioctl(fd, TIOCSLTC, &ltc);
    ioctl(fd, TIOCLSET, &lm);
#ifdef TIOCSWINSZ
    {
	struct winsize ws;
	ws.ws_row= LinesPerScreen;
	ws.ws_col= CharsPerLine;
	ioctl(fd, TIOCSWINSZ, &ws);
    }
#endif
#ifdef TIOCSSIZE
    {
	struct ttysize ts;
	ts.ts_lines= LinesPerScreen;
	ts.ts_cols= CharsPerLine;
	ioctl(fd, TIOCSSIZE, &ts);
    }
#endif
}


static SetupControllingTTY(line)
char line[];
{
/*    int fd, pid;

    if ((fd= open("/dev/tty", O_RDWR)) < 0) {
	setpgrp(0, 0);
	close(open(line, O_RDWR));
    } else
	close(fd);
    setpgrp(0, pid= getpid());
    ioctl(0, TIOCSPGRP, &pid);*/
}

static int addut(uid, line)
int uid;
char *line;
{
    struct utmp utmp;
    struct passwd *pw;
    int i, tslot= -1;

    /*
     * Record entry in /etc/utmp if possible.
     */
    tslot= ttyslot();
    pw= getpwuid(uid);
    if (tslot > 0 && pw && (i= open(utmpf, O_RDWR)) >= 0) {
    
	bzero((char*) &utmp, sizeof(utmp));
	strncpy(utmp.ut_line, &line[5], sizeof(utmp.ut_line));
	strncpy(utmp.ut_name, pw->pw_name, sizeof(utmp.ut_name));
	time(&utmp.ut_time);
	lseek(i, (long)(tslot * sizeof (utmp)), L_SET);
	write(i, (char*) &utmp, sizeof (utmp));
	close(i);
	
	if ((i= open(wtmpf, O_WRONLY|O_APPEND)) >= 0) {
	    write(i, (char*) &utmp, sizeof(utmp));
	    close(i);
	}
    }
    return tslot;
}

static rmut(line, tslot)
char *line;
int tslot;
{
    struct utmp utmp;
    int fd;
    
    if (tslot >= 0 && (fd= open(utmpf, O_WRONLY)) >= 0) {
	
	bzero((char*) &utmp, sizeof (utmp));
	lseek(fd, (long)(tslot * sizeof (utmp)), L_SET);
	write(fd, (char*) &utmp, sizeof (utmp));
	close(fd);
	
	fd= open(wtmpf, O_WRONLY | O_APPEND);
	if (fd >= 0) {
	    strncpy(utmp.ut_line, &line[5], sizeof(utmp.ut_line));
	    time(&utmp.ut_time);
	    write(fd, (char*) &utmp, sizeof(utmp));
	    close(fd);
	}
    }
}

