/*
 * signal.c
 * Larn is copyrighted 1986 by Noah Morgan.
 */

#include <signal.h>

#include "header.h"

#define BIT(a) (1 << ((a) - 1))

extern char savefilename[];
extern char wizard;
extern char predostuff;
extern char nosignal;

/* Text to be displayed if ^C during intro screen */
static void s2choose()
{
    cursor(1, 24);
    lprcat("Prees ");
    setbold();
    lprcat("return");
    resetbold();
    lprcat(" to continue: ");
    lflush();
}

/* What to do for a ^C */
static void cntlc()
{
    /* Don't do anything if inhibited */
    if(nosignal) {
	return;
    }

    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    quit();

    if(predostuff == 1) {
	s2choose();
    }
    else {
	showplayer();
    }

    lflush();
    signal(SIGQUIT, cntlc);
    signal(SIGINT, cntlc);
}

/* Subroutine to save the game if a hangup signal */
static void sgam()
{
    savegame(savefilename);
    wizard = 1;

    /* Hangup signal */
    died(-257);
}

#ifdef SIGTSTP
/* ^Y */
static void tstop()
{
    /* Nothing if inhibited */
    if(nosignal) {
	return;
    }

    lcreat((char *)0);
    clearvt100();
    lflush();
    signal(SIGTSTP, SIG_DFL);

#ifdef SIGVTALRM
    /* Looks like BSD4.2 or higher - must clr mask for signal to take effect */
    sigsetmask(sigblock(0) & ~BIT(SIGTSTP));
#endif

    kill(getpid(), SIGTSTP);
    setupvt100();
    signal(SIGTSTP, tstop);

    if(predostuff == 1) {
	s2choose();
    }
    else {
	drawscreen();
    }

    showplayer();
    lflush();
}
#endif

/* Subroutine to issue the needed signal traps call from main() */
static void sigill()
{
    sigpanic(SIGILL);
}

static void sigtrap()
{
    sigpanic(SIGTRAP);
}

static void sigiot()
{
    sigpanic(SIGIOT);
}

static void sigemt()
{
    sigpanic(SIGEMT);
}

static void sigfpe()
{
    sigpanic(SIGFPE);
}

static void sigbus()
{
    sigpanic(SIGBUS);
}

static void sigsegv()
{
    sigpanic(SIGSEGV);
}

static void sigsys()
{
    sigpanic(SIGSYS);
}

static void sigpipe()
{
    sigpanic(SIGPIPE);
}

static void sigterm()
{
    sigpanic(SIGTERM);
}

void sigsetup()
{
    signal(SIGQUIT, cntlc);
    signal(SIGINT, cntlc);
    signal(SIGKILL, SIG_IGN);
    signal(SIGHUP, sgam);
    signal(SIGILL, sigill);
    signal(SIGTRAP, sigtrap);
    signal(SIGIOT, sigiot);
    signal(SIGEMT, sigemt);
    signal(SIGFPE, sigfpe);
    signal(SIGBUS, sigbus);
    signal(SIGSEGV, sigsegv);
    signal(SIGSYS, sigsys);
    signal(SIGPIPE, sigpipe);
    signal(SIGTERM, sigterm);

#ifdef SIGTSTP
    signal(SIGTSTP, tstop);
    signal(SIGSTOP, tstop);
#endif
}

/* For BSD UNIX? */
#ifdef BSD

static char *signame[NSIG] = {
    "",
    "SIGHUP",    /*  1    Hangup */
    "SIGINT",    /*  2    Interrupt */
    "SIGQUIT",   /*  3    Quit */
    "SIGILL",    /*  4    Illegal instruction (not reset when caught) */
    "SIGTRAP",   /*  5    Trace trap (not reset when caught) */
    "SIGIOT",    /*  6    IOT instruction */
    "SIGEMT",    /*  7    EMT instruction */
    "SIGFPE",    /*  8    Floating point exception */
    "SIGKILL",   /*  9    Kill (cannot be caught or ignored) */
    "SIGBUS",    /* 10    Bus error */
    "SIGSEGV",   /* 11    Segmentation violation */
    "SIGSYS",    /* 12    Bad argument to system call */
    "SIGPIPE",   /* 13    Write on a pipe with no one to read it */
    "SIGALRM",   /* 14    Alarm clock */
    "SIGTERM",   /* 15    Software termination signal from kill */
    "SIGURG",    /* 16    Urgent condition on IO channel */
    "SIGSTOP",   /* 17    Sendable stop signal not from tty */
    "SIGTSTP",   /* 18    Stop signal from tty */
    "SIGCONT",   /* 19    Continue a stopped process */
    "SIGCHLD",   /* 20    To parent on child stop or exit */
    "SIGTTIN",   /* 21    To reader's pgrp upon backgroun tty read */
    "SIGTTOU",   /* 22    Like TTIN for output if (tp->t_local & LTOSTOP) */
    "SIGIO",     /* 23    Input/output possible signal */
    "SIGXPCU",   /* 24    Exceeded CPU time limit */
    "SIGXFSZ",   /* 25    Exceeded file size limit */
    "SIGVTALRM", /* 26    Virtual time alarm */
    "SIGPROF",   /* 27    Profiling time alarm */
    "",
    "",
    "",
    ""
};

/* For system V? */
#else

static char *signame[NSIG] = {
    "",
    "SIGHUP",  /*  1    Hangup */
    "SIGINT",  /*  2    Interrupt */
    "SIGQUIT", /*  3    Quit */
    "SIGILL",  /*  4    Illegal instruction (not reset when caught) */
    "SIGTRAP", /*  5    Trace trap (not reset when caught) */
    "SIGIOT",  /*  6    IOT instruction */
    "SIGEMT",  /*  7    EMT instruction */
    "SIGFPE",  /*  8    Floating point exception */
    "SIGKILL", /*  9    Kill (cannot be caught or ignored) */
    "SIGBUS",  /* 10    Bus error */
    "SIGSEGV", /* 11    Segmentation violation */
    "SIGSYS",  /* 12    Bad argument to system call */
    "SIGPIPE", /* 13    Write on a pipe with no one to read it */
    "SIGALRM", /* 14    Alarm clock */
    "SIGTERM", /* 15    Software termination signal from kill */
    "SIGUSR1", /* 16    User defines signal 1 */
    "SIGUSR2", /* 17    User defines signal 2 */
    "SIGCLD",  /* 18    Child death */
    "SIGPWR",  /* 19    Power fail */
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
};

#endif

/* Routine to process a fatal error signal */
static void sigpanic(int sig)
{
    char buf[128];

    signal(sig, SIG_DFL);
    sprintf(buf, "\nLarn - Panic! Signal %d received [%s]", sig, signame[sig]);
    write(2, buf, strlen(buf));
    sleep(2);
    sncbr();
    savegame(savefilename);

    /* This will terminate us */
    kill(getpid(), sig);
}
