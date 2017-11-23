/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, 5th floor, Boston, MA 02110-1301, USA.
 *
 * The GNU General Public License is contained in the file LICENSE.
 * -----------------------------------------------------------------------------
 *
 * GB_guardian.c - Makes sure your GB servers keep running, sends notice via
 *                 email of any crashes, bails after 5 restarts.
 *
 * By Michael F. Wilkinson 10/07/03
 */

#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "files.h"

/*
 * Minimum UID (user) that we'll run as
 */
#define UIDMIN 100

/*
 * Minimum GID (group) that we'll run as
 */
#define GIDMIN 100

/*
 * Number of times we restart before giving up
 */
#define RESTARTS 4

char *guard;
char *prog;

int main(int argc, char *argv[])
{
    sigset_t newsigset;
    int pid;
    int status;
    char timestr[80];
    char corestr[160];
    char temp[512];
    char *bfn;
    struct tm *mytime;
    time_t tim;
    int foundcore = 0;
    int died = 0;
    char repfile[80];
    FILE *pidfile;
    FILE *fp;
    uid_t uid;
    gid_t gid;

    /*
     * Remember guardian name
     */
    guard = basename(argv[0]);
    prog = PATH(bin/GB_server);

    uid = getuid();

    if (uid < UIDMIN) {
	fprintf(stderr,
		"%s tried to run as user-id %d, requires at least %d.\n",
		guard,
		uid,
		UIDMIN);

	exit(1);
    } else {
	uid = geteuid();

	if (uid < UIDMIN) {
	    fprintf(stderr,
		    "%s tried to run as user-id %d, reuires at least %d.\n",
		    guard,
		    uid,
		    UIDMIN);

	    exit(1);
	}
    }

    gid = getgid();

    if (gid < GIDMIN) {
	fprintf(stderr,
		"%s tried to run as group-id %d, requires at least %d.\n",
		guard,
		gid,
		GIDMIN);

	exit(1);
    } else {
	gid = getegid();

	if (gid < GIDMIN) {
	    fprintf(stderr,
		    "%s tried to run as group-id %d, requires at least %d.\n",
		    guard,
		    gid,
		    GIDMIN);

	    exit(1);
	}
    }

    if (getenv("HOME") == NULL) {
	fprintf(stderr, "Problem with shell, check HOME variable.\n");

	exit(1);
    }

    if (argc != 1) {
	fprintf(stderr, "No arguments required...\n");
	fprintf(stderr, "%s will automatically start %s\n", guard, prog);

	exit(1);
    }

    if (chdir(PATH(bin)) != 0) {
	fprintf(stderr, "%s: Could not change directory to: %s\n",
		guard,
		PATH(BIN));

	exit(1);
    }

    bfn = basename(prog);

    /*
     * newsigset is empty
     */
    sigemptyset(&newsigset);

    /*
     * Add SIGHUP to newsigset
     */
    sigaddset(&newsigset, SIGHUP);

    if (sigprocmask(SIG_BLOCK, &newsigset, NULL) < 0) {
	perror("Could not block the signal");

	exit(1);
    }

    if (dup2(1, 2) < 0) {
	perror("Cound not duplicate stderr/stdout");

	exit(1);
    }

    /*
     * Create pid file
     */
    sprintf(temp, "%s/%s.pid", LOG(), guard);
    pidfile = fopen(temp, "w+");

    if (pidfile != 0) {
	fprintf(pidfile, "%d", (int)getpid());
	fclose(pidfile);
    } else {
	fprintf(stderr, "Unable to create %s\n", temp);

	exit(1);
    }

    fprintf(stderr, "%s: on duty.\n", guard);

    while (1) {
	pid = fork();

	switch(pid) {
	case -1:
	    /*
	     * Error!
	     */
	    perror("Can't fork a child");

	    /*
	     * 10 minutes
	     */
	    sleep(600);

	    break;
	case 0:
	    /*
	     * Child
	     */
	    execl(prog, bfn, (char *)NULL);
	    perror("Can't exec child");

	    exit(1);

	    break;
	default:
	    /*
	     * Parent
	     */
	    fprintf(stderr,
		    "%s: starting \'%s\' with pid [%d]\n",
		    guard,
		    bfn,
		    pid);

	    /*
	     * Wait for the little brat
	     */
	    while (wait(&status) != pid) {
	    }

	    break;
	}

	/*
	 * Did the child exit abnormally?
	 */
	if (WIFEXITED(status) == 0) {
	    /*
	     * We're out of fork, the process has died
	     */
	    ++died;

	    fprintf(stderr, "%s: \'%s\' died!\n", guard, bfn);

	    /*
	     * Get the current time
	     */
	    tim = time(NULL);
	    mytime = localtime(&tim);
	    strftime(timestr, sizeof(timestr), "%m.%d.%Y-%T", mytime);
	    sprintf(corestr, "core-%s", timestr);

#ifdef DEBUG
	    if (rename("core", corestr) == 0) {
		foundcore = 1;
		fprintf(stderr,
			"%s: Found core file, moved to %s\n",
			guard,
			corestr);
	    }

#ifdef WCOREDUMP
	    if (WCOREDUMP(status)) {
		foundcore = 1;
	    }
#endif
#endif
	    sprintf(repfile, "/tmp/%s-%d", guard, pid);
	    fp = fopen(repfile, "w");

	    if (fp != 0) {
		fprintf(fp,
			"Notice from %s\n\'%s\' died on %s\n",
			guard,
			bfn,
			timestr);

		if (foundcore) {
		    fprintf(fp, "A core file was produced.\n");
		}

		if (died > RESTARTS) {
		    fprintf(fp, "\nToo many restarts...giving up!\n\n");
		} else {
		    fprintf(fp,
			    "\nWill attempt a restart...have a nice day!\n\n");
		}

		fclose(fp);

		sprintf(temp,
			"cat %s | %s -s \"GB Crash Guard Report\" %s; rm %s",
			repfile,
			MAILPROG,
			GODADDR,
			repfile);

		if (!system(temp)) {
		    fprintf(stderr, "%s: Sent email crash report\n", guard);
		} else {
		    fprintf(stderr,
			    "%s: Could not send email crash report\n",
			    guard);
		}
	    } else {
		fprintf(stderr, "%s: Could not open %s\n", guard, repfile);
	    }

	    fprintf(stderr, "%s: Time is \'%s\'\n", guard, timestr);

	    if (died > RESTARTS) {
		fprintf(stderr,
			"Too many restarts (%d), shutting down.\n",
			died);

		exit(1);
	    }

	    /*
	     * Wait a minute then try to start it up again
	     */
	    sleep(60);

	    foundcore = 0;
	} else {
	    /*
	     * We exited normally, so the guardian will shut down too
	     */
	    exit(0);
	}
    }
}
