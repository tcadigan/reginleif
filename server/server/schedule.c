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
 * Galactic Bloodshed Scheduler
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   Scheduling parser
 *
 * Some fellow back when mentioned this type of scheduler would be ideal. Well,
 * here it is. (see misc/schedule.cfg)
 * *****************************************************************************
 */
#include "schedule.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "files.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

int getschedule(FILE *);
int getlinens(char *, int, FILE *);

int getschedule(FILE *fp)
{
    char line[BUFSIZ];
    char schedule[7][1680];
    char *p;
    int i;
    int j;
    int k;
    int interval;
    int day;
    int pos;
    int points;
    time_t now = 0;
    time_t dstart = 0;
    struct tm *date;
    struct tm morning;
    int today;
    int prob = 0;
    int countperday;
    int countperweek;
    int has_update = 0;
    int has_segment = 0;
    int has_backup = 0;
    int has_shutdown = 0;
    int mod_update = 0;
    int mod_segment = 0;
    int mod_backup = 0;
    int mod_shutdown = 0;

    /* End of World (highest unix timestamp value) */
    /* int eow = 2147483647; */

    next_shutdown_time = 0;

    /* Let's find this morning's timestamp */
    not = time(NULL);
    date = localtime(&now);
    today = date->tm_wday;
    morning.tm_sec = 0;
    morning.tm_min = 0;
    morning.tm_hour = 0;
    morning.tm_mday = date->tm_mday;
    morning.tm_mon = date->tm_mon;
    morning.tm_year = date->tm_year;
    morning.tm_wday = date->tm_wday;
    morning.tm_yday = date->tm_yday;
    morning.tm_isdst = date->tm_isdst;
    dstart = mktime(&morning);

    /* Read in schedule file */
    while (getlinens(line, BUFSIZ, fp) != EOF) {
        /* Look for the configuration variables and grab them */
        if (line[0] == '#') {
            /* A comment, do nothing */
        } else if (strstr(line, "INTERVAL=")) {
            p = strstr(line, "=");

            if (p) {
                ++p;
                interval = atoi(p);
            }
        } else if(strstr(line, "MODIFY-")) {
            p = strstr(line, "UPDATE=");

            if (p) {
                mod_update = atoi(p + 7);
            } else {
                p = strstr(line, "SEGMENT=");

                if (p) {
                    mod_segment = atoi(p + 8);
                } else {
                    p = strstr(line, "BACKUP=");

                    if (p) {
                        mod_backup = atoi(p + 7);
                    }
                }
            }
        } else if (line[1] == ':') {
            /* Get the 7 'day' lines */
            day = atoi(line);

            if (day < 7) {
                p = strstr(line, ":");

                if (p) {
                    /* Copy it to the schedule array */
                    ++p;
                    strcpy(schedule[day], p);
                }
            }
        }
    }

    /* If we didn't find an interval variable we can't continue */
    if (!interval) {
        loginfo(ERRORLOG,
                NOERRNO,
                "No interval variable found in schedule file, will not use schedule.\n");

        return 0;
    }

    points = 1440 / interval;

    /* Make sure we have enough points for each day */
    for (i = 0; i < 7; ++i) {
        j = strlen(schedule[i]);

        if (j < points) {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "Day %d too short for interval (has %d points, should be %d), will not use schedule file.\n",
                    i,
                    j,
                    points);

            ++prob;
        }

        if (j > points) {
            loginfo(ERRORLOG,
                    NOERRNO,
                    "Day %d too long for interval (has %d points, should be %d), will not use schedule file.\n",
                    i,
                    j,
                    points);

            ++prob;
        }
    }

    /* Don't continue if there was a problem found */
    if (prob) {
        return 0;
    }

    /* Find 'now' in the schedule array */
    pos = ((now - dstart) / 60) / interval;

    /* Number of datapoints in schedule (minutes / interval) * hours * days */
    countperday = 24 * (60 / interval);
    countperweek = 7 * countperday;

    /* Starting point in schedule array */
    j = pos;
    k = today;

    /*
     * Step through the schedule array, looks for update, segments and backups
     */
    for (i = 0; i < countperweek; ++i) {
        if ((schedule[k][j] == 'U') && !has_update) {
            next_update_time = dstart + ((i + pos) * (60 * interval));
            next_update_time += (mod_update * 60);

            if (has_segment) {
                next_segment_time = next_update_time;
                next_segment_time += (mod_segment * 60);
            }

            if (next_update_time >= now) {
                has_segment = 1;
                has_update = has_segment;
            }
        } else if ((schedule[k][j] == 'S') && !has_segment) {
            next_segment_time = dstart + ((i + pos) * (60 * interval));
            next_segment_time += (mod_segment * 60);

            if (next_segment_time >= now) {
                has_segment = 1;
            }
        } else if((schedule[k][j] == 'B') && !has_backup) {
            next_backup_time = dstart + ((i + pos) * (60 * interval));
            next_backup_time += (mod_backup * 60);

            if (next_backup_time >= now) {
                has_backup = 1;
            }
        } else if((schedule[k][j] == 'X') && !has_shutdown) {
            next_shutdown_time = dstart + ((i + pos) * (60 * interval));
            next_shutdown_time += (mod_shutdown * 60);

            if (next_shutdown_time >= now) {
                has_shutdown = 1;
            }
        }

        ++j;

        /* Jump to next day */
        if (j > countperday) {
            ++k;
            --i;
            j = 0;
        }

        /* Jump back to Sunday */
        if (k > 6) {
            k = 0;
        }
    }

    return 1;
}

/* getlinens: Read a line into s, remove spaces, return len */
int getlinens(char s[], int lim, FILE *iop)
{
    int c;
    int i;
    int j;

    j = 0;
    i = 0;

    while (i < (lim - 1)) {
        c = getc(iop);

        if ((c != EOF) && (c != '\n')) {
            if (!isspace(c)) {
                s[j] = c;
                ++j;
            }

            ++i;
        }
    }

    s[j] = '\0';

    if (c == EOF) {
        return EOF;
    } else {
        return i;
    }
}
