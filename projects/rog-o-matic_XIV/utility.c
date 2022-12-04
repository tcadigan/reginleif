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
#include "utility.h"

#include <fcntl.h>
#include <pwd.h>
#include <sgtty.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

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
    struct termios term;

    tcgetattr(STDOUT_FILENO, &term);

    return cfgetospeed(&term);
}

/*
 * getname: Get userid of player.
 */
char *getname()
{
    static char name[100];

    struct passwd *pass = getpwuid(getuid());

    strncpy(name, pass->pw_name, strlen(pass->pw_name));
    name[strlen(pass->pw_name)] = '\0';

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
static void (*hstat)();
static void (*istat)();
static void (*qstat)();
static void (*pstat)();

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
        signal(SIGQUIT, exitproc);
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

/*
 * quit: Defined for compatibility with Berkeley 4.2 system
 */
void quit(int code, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

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
