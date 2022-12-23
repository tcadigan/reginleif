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
 *
 * Galactic Bloodshed Bug Reporting System
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   bug() - Called from GB_server.c
 *   ancillary functions
 *
 * These functions write to and display the bugrep.txt file. This file replaces
 * the original system, this is designed to be more helpful for both user and
 * admin.
 * *****************************************************************************
 *
 * #ident  "@(#)bug.c  1.0 7/5/01 "
 */
#include "bug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../server/buffers.h"
#include "../server/config.h"
#include "../server/dispatch.h"
#include "../server/files.h"
#include "../server/GB_server.h"
#include "../server/game_info.h"
#include "../server/power.h"
#include "../server/races.h"
#include "../server/shlmisc.h"
#include "../server/ships.h"
#include "../server/vars.h"

void bug(int playernum, int governor, int apcount)
{
    FILE *fp;
    time_t t;
    struct tm *st;
    int i;

    t = time(NULL);
    st = localtime(&t);

    if (argn < 2) {
        notify(playernum, governor, "No bug to report?\n");

        return;
    }

    if ((argn == 2) && match(args[2], "report")) {
        read_bug_report(playernum, governor);

        return;
    }

    sprintf(buf, "%s", args[1]);

    for (i = 2; i < argn; ++i) {
        sprintf(temp, " %s", args[i]);
        strcat(buf, temp);
    }

    sprintf(long_buf,
            "{{bugid}}: ?? Status: Reported\n%.2d/%.2d/%2.d %.2d:%.2d:%.2d Reported by: %s, %s [%d,%d]\nDesc: %s\nNarr:\n--\n\n",
            st->tm_mon + 1,
            st->tm_mday,
            (st->tm_year > 99) ? st->tm_year - 100 : st->tm_year,
            st->tm_hour,
            st->tm_min,
            st->tm_sec,
            races[playernum - 1]->name,
            races[playernum - 1]->governor[governor].name,
            playernum,
            governor,
            buf);

    sprintf(telegram_buf, "+++ BUG REPORT FILED +++ %s", buf);

    fp = fopen(BUGREP, "a+");

    if (fp != 0) {
        fprintf(fp, "%s", long_buf);
        fclose(fp);

        send_race_dispatch(playernum, governor, 1, TO_RACE, 1, telegram_buf);

        if (send_bug_email(long_buf)) {
            notify(playernum, governor, "Bug reported, thank you.\n");
        } else {
            notify(playernum, governor, "Bug reported, but email NOT sent.\n");
        }
    } else {
        notify(playernum, governor, "But NOT reported, can't open bug db.\n");
    }
}

int send_bug_email(char *report)
{
    char repfile[80];
    FILE *fp;

    strcpy(repfile, "/tmp/gbbugrep");
    fp = fopen(repfile, "w");

    if (fp != 0) {
        fprintf(fp, "%s", report);
        fclose(fp);

        sprintf(temp,
                "cat %s | %s -s \"GB Bug Report\" %s; rm %s",
                repfile,
                MAILPROG,
                GODADDR,
                repfile);

        if (!system(temp)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void read_bug_report(int playernum, int governor)
{
    FILE *f;

    f = fopen(BUGREP, "r");

    if (f != 0) {
        while(fgets(buf, sizeof(buf), f)) {
            /* strcat(buf, "\n"); */
            notify(playernum, governor, buf);
        }

        fclose(f);
        notify(playernum, governor, "----\nEnd.\n");
    } else {
        notify(playernum, governor, "Unable to open bug report file.\n");
    }
}
