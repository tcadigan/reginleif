/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th Floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * *****************************************************************************
 * log.c
 * Created: Fri Dec 11 03:50:49 EST 1992
 * Author: J. Deragon (deragon@jethro.nyu.edu)
 *
 * Version: 1.5 20:59:33
 *
 * Contains:
 *   clearlog()
 *   log()
 *   check_logsize()
 * *****************************************************************************
 *
 * #ident  "@(#)log.c     1.13 11/15/93 "
 *
 * $Header: /var/cvs/gbp/GB+/server/log.c,v 1.14 2007/07/06 17:30:26 gbp Exp $
 *
 * static char *ver = "@(#)        $RCSfile: log.c,v $ $Revision: 1.4 $";
 */
#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "buffers.h"
#include "config.h"
#include "files.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

int check_logsize(int);

int where;

/*
 * clearlog:
 *
 * Arguments:
 *   override - override is used upon startup, when true it will remove
 *              (truncate) _all_ log files. It is normally set to false.
 *
 * Called by:
 *   main
 *   check_logsize
 *
 * Description:
 *   Called once from main, upon startup to clear all the log files to prevent
 *   disgustingly large log files from eating up the disk, and called from
 *   check_logsize. If override is set to false, it will determine the log to
 *   truncate from the global variable "where".
 *
 * JPD Fri Dec 11 03:01:46 EST 1992
 *
 * Wed Mar 16 18:27:32 MST 2005
 *  Thirteen years later, disk space no longer a premium. When override is true
 *  we just append. clearlog() still clears according to MAX_LOGSIZE when called
 *  by check_logsize(). -mfw
 */
#ifdef SAVE_LOGS
int clearlog(int override)
{
    char cmd[1024];

    if ((where == ERRORLOG) || override) {
        sprintf(cmd,
                "uncompress %s; cat %s >> %s; compress %s",
                ERRLOGDIG,
                ERRLOG,
                ERRLOGDIG,
                ERRLOGDIG);

        system(cmd);
        flcose(fopen(ERRLOG, "w+"));
        loginfo(ERRORLOG, NOERRNO, "Clearing ERRORLOG");
    }

    if ((where == USERLOG) || override) {
        sprintf(cmd,
                "uncompress %s; cat %s >> %s; compress %s",
                USLOGDIG,
                USLOG,
                USLOGDIG,
                USLOGDIG);

        system(cmd);
        fclose(fopen(USLOG, "w+"));
        loginfo(ERRORLOG, NOERRNO, "Clearing USERLOG");
    }

    if ((where == UPDATELOG) || override) {
        sprintf(cmd,
                "uncompress %s; cat %s >> %s; compress %s",
                UPLOGDIG,
                UPLOG,
                UPLOGDIG,
                UPLOGDIG);

        system(cmd);
        fclose(fopen(UPLOG, "w+"));
        loginfo (ERRORLOG, NOERRNO, "Clearing UPDATE");
    }

    /* Simple enough eh? */
    return 1;
}

#else

int clearlog(int override)
{
    if (override) {
        fclose(fopen(ERRLOG, "a+"));
        fclose(fopen(USLOG, "a+"));
        fclose(fopen(UPLOG, "a+"));
    } else {
        switch (where) {
        case ERRORLOG:
            fclose(fopen(ERRLOG, "w+"));
            loginfo(ERRORLOG, NOERRNO, "Clearing ERRORLOG");

            break;
        case USERLOG:
            fclose(fopen(USLOG, "w+"));
            loginfo(ERRORLOG, NOERRNO, "Clearing USERLOG");

            break;
        case UPDATELOG:
            fclose(fopen(UPLOG, "w+"));
            loginfo(ERRORLOG, NOERRNO, "Clearing UPDATE");

            break;
        }
    }

    /* Simple enough eh? */
    return 1;
}
#endif
/* End of clearlog */

/*
 * loginfo:
 *
 * Arguments:
 *   multiple - First is _always_ the log file that the error should be written
 *              to. Either ERRORLOG, USERLOG, or UPDATELOG. Second arg is the
 *              noerrno flag. Set to false when we should ignore the
 *              errno. (i.e. when we don't care about it) third arg is the data
 *
 * Called by:
 *   Everyone!
 *
 * Description:
 *   Called whenever something needs to be logged. the most frequent logs are
 *   the USERLOG (when someone logs on) and UPDATELOG (details of
 *   updates/segments)
 *
 * JPD Fri Dec 11 03:01:46 EST 1992
 */

/* VARARGS */
int loginfo(int Where, ...)
{
    vA_list list;
    time_t now;
    char buf[1024];
    char buf1[1024];
    char *fmt; /* The actual data to be printed */
    int noerrno; /* Should we care about the errno? */
    int logf;
    char const *error;
    char *p;
    char const *logfile = NULL; /* The actual log file name */
    struct tm *no_tm;

    va_start(list, where);
    where = Where; /* Put it where? */
    noerrno = va_arg(list, int); /* Disregard errno? false yes, true no */
    fmt = va_arg(list, char *); /* Data */
    vsprintf(buf, fmt, list);

    /*
     * We do this little bit to allow us an easy way to pass filenames back and
     * forth between clearlogs and log. I.e. pass an int, instead of a (char *),
     * so we can use switch.
     */

    switch (where) {
    case ERRLOG:
        logfile = ERRLOG;

        break;
    case USERLOG:
        logfile = USLOG;

        break;
    case UPDATELOG:
        logfile = UPLOG;

        break;
    default:

        break;
    }

    p = index(buf, '\n');

    /* Get rid of the newline */
    if (p != 0) {
        *p = 0;
    }

    time(&now); /* For the timestamp */
    now_tm = localtime(&now);
    error = "log";

    if ((errno != 0) && noerrno) {
        error = strerror(errno);
    }

    sprintf(buf1,
            "%2d/%02d %2d:%02d:%02d - %s\n",
            now_tm->tm_mon + 1,
            now_tm->tm_mday,
            now_tm->hour,
            now_tm->tm_min,
            now_tm->tm_sec,
            buf);

    logf = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if (logf < 0) {
        return 0;
    }

    check_logsize(logf); /* Make sure we don't hog space */
    write(logf, buf1, strlen(buf1));
    close(logf);
    errno = 0;
    va_end(list);

    return 1;
}
/* End of loginfo() */

/*
 * check_logsize:
 *
 * Arguments:
 *   log_fd - The log file descriptor (int).
 *
 * Called by:
 *   log (local to this file)
 *
 * Description:
 *   Called whenever log attempts to write to a file. Before the write it will
 *   check to see if the log is larger than the max log size (default 100K). If
 *   it is, it calls clearlog() to truncate the log.
 *
 * JPD Fri Dec 11 03:01:46 EST 1992
 */
int check_logsize(int log_fd)
{
    struct stat logstats;

    fstat(log_fd, &logstats);

    if (logstats.st_size > MAX_LOGSIZE) {
        clearlog(0);
    }

    return 1;
}
/* End of check_logsize */
