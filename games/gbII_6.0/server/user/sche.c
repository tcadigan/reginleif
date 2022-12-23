/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (C) 1989-90 by Robert P. Chansky, et al.
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
 * sched.c
 *
 * Created: Fri Dec 11 03:50:49 EST 1992
 * Author: ???
 * Version: 1.8 17:35:15
 *
 * Contains:
 *   GB_schedule()
 *
 * #ident  "@(#)sche.c  1.13 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/sche.c,v 1.3 2007/07/06 18:09:34 gbp Exp $
 * *****************************************************************************
 */
#include "sche.h"

#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include "../server/buffers.h"
#include "../server/GB_server.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/ranks.h"
#include "../server/ships.h"
#include "../server/vars.h"

/* Prototypes */
void timedifftoascii(long, long, char *);

/*
 * GB_schedule:
 *
 * Arguments:
 *   Playernum Playernum who called it
 *   Governor  Governor who called it
 *
 * Called by:
 *   process_command
 */
void GB_schedule(int playernum, int governor)
{
    char *ctp;
    char *nlp;
    time_t clk;
    char tbuf[BUFSIZ];
    char fbuf[BUFSIZ];
    struct stat stbuf;
    racetype *r;

    r = races[playernum - 1];

    if (suspended || (stat(NOGOFL, &stbuf) >= 0)) {
        sprintf(buf, "Updates and segments currently suspended.\n");
        notify(playernum, governor, buf);

        return;
    }

    clk = time(0);
    sprintf(buf, "%d minute update intervals.\n", update_time);
    notify(playernum, governor, buf);
    sprintf(buf, "%ld movement segments per update.\n", segments);
    notify(playernum, governor, buf);
    sprintf(buf, "Combat update is: %d\n", CombatUpdate);
    notify(playernum, governor,   buf);

    if (r->governor[governor].CSP_client_info.csp_user == 1) {
        sprintf(tbuf, "%ld", clk);

        sprintf(buf,
                "Current time    : %s%-10s%s\n",
                OPEN_TIME_TAG,
                tbuf,
                CLOSE_TIME_TAG);

        notify(playernum, governor, buf);

        sprintf(fbuf, "%ld", next_backup_time);
        timedifftoascii(next_backup_time, clk, tbuf);

        sprintf(buf,
                "Next Backup %3s : %s%-10s%s (%s)\n",
                " ",
                OPEN_TIME_TAG,
                fbuf,
                CLOSE_TIME_TAG,
                tbuf);

        notify(playernum, governor, buf);
        sprintf(fbuf, "%ld", next_segment_time);
        timedifftoascii(next_segment_time, clk, tbuf);

        sprintf(buf,
                "Next Segment %2d : %s%-10s%s (%s)\n",
                nsegments_done == segments ? 1 : nsegments_done + 1,
                OPEN_TIME_TAG,
                fbuf,
                CLOSE_TIME_TAG,
                tbuf);

        notify(playernum, governor, buf);
        sprintf(fbuf, "%ld", next_update_time);
        timedifftoascii(next_update_time, clk, tbuf);

        sprintf(buf,
                "Next Update %3d : %s%-10s%s (%s)\n",
                nupdates_done + 1,
                OPEN_TIME_TAG,
                fbuf,
                CLOSE_TIME_TAG,
                tbuf);

        notify(playernum, governor, buf);

        if (next_shutdown_time) {
            sprintf(fbuf, "%ld", next_shutdown_time);
            timedifftoascii(next_shutdown_time, clk, tbuf);

            sprintf(buf,
                    "SHUTDOWN AT    : %s%-10s%s (%s)\n",
                    OPEN_TIME_TAG,
                    fbuf,
                    CLOSE_TIME_TAG,
                    tbuf);

            notify(playernum, governor, buf);
        }
    } else {
        sprintf(buf, "Current time    : %s", ctime(&clk));
        notify(playernum, governor, buf);
        timedifftoascii(next_backup_time, clk, tbuf);
        ctp = ctime(&next_backup_time);
        nlp = index(ctp, '\n');

        while (nlp) {
            *nlp = '\0';
            nlp = index(ctp, '\n');
        }

        sprintf(buf, "Next Backup %2s : %s (%s)\n", " ", ctp, tbuf);
        notify(playernum, governor, buf);
        timedifftoascii(next_segment_time, clk, tbuf);
        ctp = ctime(&next_segment_time);
        nlp = index(ctp, '\n');

        while (nlp) {
            *nlp = '\0';
            nlp = index(ctp, '\n');
        }

        sprintf(buf,
                "Next Segments %2d : %s (%s)\n",
                nsegments_done == segments ? 1: nsegments_done + 1,
                ctp,
                tbuf);

        notify(playernum, governor, buf);
        timedifftoascii(next_update_time, clk, tbuf);
        ctp = ctime(&next_update_time);
        nlp = index(ctp, '\n');

        while (nlp) {
            *nlp = '\0';
            nlp = index(ctp, '\n');
        }

        sprintf(buf,
                "Next Update %3d : %s (%s)\n",
                nupdates_done + 1,
                ctp,
                tbuf);

        notify(playernum, governor, buf);
    }
}

void timedifftoascii(long t1, long t2, char *tbuf)
{
    long x;
    long tdiff;
    char tbuf2[BUFSIZ];

    tbuf[0] = '\0';
    tdiff = t1 - t2;

    /* Days */
    x = tdiff / (24 * 60 * 60);

    if (x) {
        sprintf(tbuf2, "%ld day%s", x, x > 1 ? "s" : "");
        strcat(tbuf, tbuf2);
    }

    tdiff -= (x * 24 * 60 * 60);

    /* Hours */
    x = tdiff / (60 * 60);

    if (x) {
        sprintf(tbuf2, "%ld hour%s", x, x > 1 ? "s" : "");

        if (*tbuf) {
            strcat(tbuf, ", ");
        }

        strcat(tbuf, tbuf2);
    }

    tdiff -= (x * 60 * 60);

    /* Minutes */
    x = tdiff / 60;

    if (x) {
        sprintf(tbuf2, "%ld min%s", x, x > 1 ? "s" : "");

        if (*tbuf) {
            strcat(tbuf, ", ");
        }

        strcat(tbuf, tbuf2);
    }

    tdiff -= (x * 60);

    /* Seconds */
    x = tdiff;

    if (x) {
        sprintf(tbuf2, "%ld sec%s", x, x > 1 ? "s" : "");

        if (*tbuf) {
            strcat(tbuf, ", ");
        }

        strcat(tbuf, tbuf2);
    }
}
