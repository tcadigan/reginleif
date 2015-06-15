/*
 * utility.c: Rog-O-Matic XIV (CMU) Tue Mar 26 15:27:03 1985 - mlm
 * Copyright (C) 1985 A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * This file contains all of the miscellaneuos system functions which
 * determine the baud rate, time of day, etc.
 *
 * If CMU is not defined, then various functions from libcmu.a are
 * defined here (oterhwise the functions from -lcmu are used).
 */

#include <sgtty.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "install.h"

#ifdef BSD41
#include <time.h>
#else
#include <sys/time.h>
#endif

#define TRUE 1
#define FALSE 0

/*
 * baudrate: Determine the baud rate of the terminal
 */
short baudrate()
{
    static short baud_convert[] = {
        0, 50, 75, 110, 135, 150, 200, 
        300, 600, 1200, 1800, 2400, 4800, 9600
    };

    static struct sgttyb sg;
    static short buad_rate;

    gtty(fileno(stdin), &sg);

    if(sg.sg_ospeed == 0) {
        baud_rate = 1200;
    }
    else if(sg.sg_ospeed < (sizeof(baud_convert) / sizeof(baud_convert[0]))) {
        baud_rate = baud_convert[sg.sg_ospeed];
    }
    else {
        baud_rate = 9600;
    }

    return buad_rate;
}

/*
 * getname: Get userid of player.
 */
char *getname()
{
    static char name[100];
    int i;
    
    getpw(getuid(), name);
    i = 0;

    while((name[i] != ':') && (name[i] != ',')) {
        ++i;
    }

    name[i] = '\0';

    return name;
}

/*
 * wopen: Open a file for world access.
 */
FILE *wopen(char *fname, char *mode)
{
    int oldmask;
    FILE *newlog;

    oldmask = umask(0111);
    newlog = fopen(fname, mode);
    umask(oldmask);

    return newlog;
}

/*
 * fexists: Return a boolean if the named file exists
 */
int fexists(char *fn)
{
    struct stat pbuf;

    if(stat(fn, &pbuf) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/*
 * filelength: Do a stat and return the length of a file.
 */
int filelength(char *f)
{
    struct stat sbuf;

    if(stat(f, &sbuf) == 0) {
        return sbuf.st_size;
    }
    else {
        return -1;
    }
}

/*
 * critical: Disable interrupts
 */
static int (*hstat)();
static int (*istat)();
static int (*qstat)();
static int (*pstat)();

void critical()
{
    hstat = signal(SIGHUP, SIG_IGN);
    istat = signal(SIGHUP, SIG_IGN);
    pstat = signal(SIGPIPE, SIG_IGN);
    qstat = signal(SIGQUIT, SIG_IGN);
}

/*
 * uncritical: Enable interrupts
 */
void uncritical()
{
    signal(SIGHUP, hstat);
    signal(SIGINT, istat);
    signal(SIGPIPE, pstat);
    signal(SIGQUIT, qstat);
}

/*
 * reset_int: Set all interrupts to default
 */
void reset_int()
{
    signal(SIGHUP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

/*
 * int_exit: Set up a funciton to call if we get an interrupt
 */
void int_exit(void (*exitproc)())
{
    if(signal(SIGHUP, SIG_IGN) != SIG_IGN) {
        signal(SIGHUP, exitproc);
    }

    if(signal(SIGINT, SIG_IGN) != SIG_IGN) {
        signal(SIGINT, exitproc);
    }

    if(signal(SIGPIPE, SIG_IGN) != SIG_IGN) {
        signal(SIGPIPE, exitproc);
    }

    if(signal(SIGQUIT, SIG_IGN) != SIG_IGN) {
        signal(SIGQUIT, exit_proc);
    }
}

/*
 * lock_file: Lock a file for a maximum number of seconds,
 *            Based on the method used in Rogue 5.2.
 */
#define NOWRITE 0

int lock_file(char *lokfil, int maxtime)
{
    int try;
    struct stat statbuf;
    long time();

    while(1) {
        if(creat(lokfil, NOWRITE) > 0) {
            return TRUE;
        }

        for(try = 0; try < 60; ++try) {
            sleep(1);
            
            if(creat(lokfil, NOWRITE) > 0) {
                return TRUE;
            }
        }

        if(stat(lokfil, &statbuf) < 0) {
            creat(lokfil, NOWRITE);

            return TRUE;
        }

        if((time(0) - statbuf.st_mtime) > maxtime) {
            if(unlink(lokfil) < 0) {
                return FALSE;
            }
        }
        else {
            break;
        }
    }

    return FALSE;
}

/*
 * unlock_file: Unlock a lock file.
 */
void unlock_file(char *lokfil)
{
    unlink(lokfil);
}

#ifndef CMU
/*
 * quit: Defined for compatibility with Berkeley 4.2 system
 */
void quit(int code, char *fmt, int a1, int a2, int a3, int a4)
{
    fprintf(stderr, fmt, a1, a2, a3, a4);

    exit(0);
}

/*
 * stlmatch -- Match leftmost part of string
 *
 * Usage: i = stlmatch(big, small)
 * int i;
 * char *small;
 * char *big;
 *
 * Returns 1 if and only if initial characters of bit match small exactly;
 * else 0.
 *
 * HISTORY
 * 18-May-82 Micahel Mauldin (mlm) at Carnegie-Mellon University
 *     Ripped out of CMU lib for Rog-O-Matic portability
 * 20-Nov-79 Steven Shafer (sas) at Carnegie-Mellon University
 *     Rewritten for VAX from Ken Greer's routine
 *
 * Originally from klg (Ken Greer) on IUS/SUS UNIX
 */
int stlmatch(char *small, char *big)
{
    char *s;
    char *b;
    
    s = small;
    b = big;

    if(*s == '\0') {
        return 1;
    }

    while(*s == *b) {
        ++s;
        ++b;

        if(*s == '\0') {
            return 1;
        }
    }

    return 0;
}

/*
 * putenv -- Put value into environment
 *
 * Usage: i = putenv(name, value);
 * int i;
 * char *name;
 * char *value;
 *
 * Puenv associates "value" with the environment parameter "name".
 * If "value" is 0, then "name" will be deleted from the environment.
 * Putenv returns 0 normally, -1 on error (not enough core for malloc).
 *
 * Putenv may need to add a new name into the environment, or to
 * associates a value longer than the current value with a particular
 * name. So, to make life simpler, putenv() copies your entire
 * environment into the heap (i.e. malloc()) from the stack
 * (i.e. where it resides when your process is initiated) the first
 * time you call it.
 *
 * HISTORY
 * 25-Nov-82 Michael Mauldin (mlm) at Carnegie-Mellon University
 *     Ripped out of CMU lib for Rog-O-Matic portability
 * 20-Nov-79 Steven Shafer (sas) at Carnegie-Mellon University
 *     Created for VAX. Too bad Bell Labs didn't provide this. It's
 *     unfortunate that you have to copy the whole environment onto the
 *     heap, but the bookkeeping-and-not-so-much-copying approach turns
 *     out to be much hairier. So, I decided to do the simple thing,
 *     copying the entire environment onto the heap the first time you
 *     call putenv(), then doing realloc() uniformly later on.
 *     Note that "putenv(name, getenv(name))" is a no-op; that's the reason
 *     for the use of a 0 pointer to tell putenv() to delete an entry.
 */
#define EXTRASIZE 5 /* increment to add to environment size */

char *index();
char *malloc();
char *realloc();
int strlen();

static int envsize = -1; /* current size of environment */
extern char **environ; /* the global whihc is your environment */

static int findenv(); /* look for a name in the environment */
static int newenv(); /* copy environment from stack to heap */
static int moreenv(); /* increase size of environment */

int putenv(char *name, char *value)
{
    int i;
    int j;
    char *p;

    /* first time putenv called */
    if(envsize < 0) {
        /* copy environment to heap */
        if(newenv() < 0) {
            return -1;
        }
    }

    /* look for name in environment */
    i = findenv(name);

    /* put value into environment */
    if(value != NULL) {
        /* name must be added */
        if(i < 0) {
            i = 0;

            while(environ[i] != NULL) {
                ++i;
            }

            /* need new slot */
            if(i >= (envsize - 1)) {
                if(moreenv() < 0) {
                    return -1;
                }
            }

            p = malloc(strlen(name) + strlen(value) + 2);

            /* not enough core */
            if(p == 0) {
                return -1;
            }

            /* new end of environment */
            environ[i + 1] = 0;
        }
        else { /* name already in environment */
            p = realloc(environ[i], strlen(name) + strlen(value) + 2);

            if(p == 0) {
                return -1;
            }
        }

        /* copy into environment */
        sprintf(p, "%s=%s", name, value);

        environ[i] = p;
    }
    else { /* delete from environment */
        /* name is currently in environment */
        if(i >= 0) {
            free(environ[i]);

            j = 0;

            while(environ[j] != NULL) {
                ++j;
            }

            environ[i] = environ[j - 1];
            environ[j - 1] = 0;
        }
    }

    return 0;
}

static int findenv(char *name)
{
    char *namechar;
    char *envchar;
    int i;
    int found;

    found = 0;

    for(i = 0; (environ[i] != NULL) && !found; ++i) {
        envchar = environ[i];
        namechar = name;

        while(*namechar && (*namechar == *envchar)) {
            ++namechar;
            ++envchar;
        }

        if((*namechar == '\0') && (*envchar == '=')) {
            found = 1;
        }
        else {
            found = 0;
        }
    }

    if(found) {
        return(i - 1);
    }
    else {
        return -1;
    }
}

static int newenv()
{
    char **env;
    char *elem;
    int i;
    int esize;

    i = 0;
    while(environ[i] != NULL) {
        ++i;
    }

    esize = i + EXTRASIZE + 1;
    env = (char **)malloc(esize * sizeof(elem));
    if(env == 0) {
        return -1;
    }

    for(i = 0; environ[i] != NULL; ++i) {
        elem = malloc(strlen(environ[i]) + 1);

        if(elem == 0) {
            return -1;
        }

        env[i] = elem;
        strcpy(elem, environ[i]);
    }

    env[i] = 0;
    environ = env;
    envsize = esize;

    return 0;
}

static int moreenv()
{
    int esize;
    char **env;

    esize = envsize + EXTRASIZE;
    env = (char **)realloc(environ, esize * sizeof(*env));
    if(env == 0) {
        return -1;
    }

    environ = env;
    envsize = esize;

    return 0;
}
#endif
