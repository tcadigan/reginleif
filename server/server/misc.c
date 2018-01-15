/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, at al.
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
 * Scales used in production efficiency etc.
 *   Input both: int 0-100
 *   Output both: float 0.0-1.0
 *   logscale output both: float 0.5-0.95
 *
 * #ident  "@(#)misc.c  1.11 12/1/93 "
 *
 * $Header:$
 *
 * static char *ver = "@(#)       $RCSfile: misc.c,v $ $Revision: 1.3 $";
 */

#include <errno.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "debug.h"
#include "files.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

extern double logscale(int);
extern void getsdata(struct stardata *);
extern void getstar(struct star **, int);
extern void getplanet(struct planet **, int, int);
extern void getrace(struct race **, int);
extern void putrace(struct race *);
extern void setdebug(int, int);

double logscale(int x)
{
    /* return ((x + 5.0) / (x + 10.0)); */
    return (log10((double)x + 1.0) / 2.0);
}

void adjust_morale(racetype *winner, racetype *loser, int amount)
{
    winner->morale += amount;
    loser->morale -= amount;
    winner->points[loser->Playernum] += amount;
}

void load_star_data(void)
{
    int s;
    int t;
    int i;
    int j;
    int pcount = 0;

    /* Get star database */
    Planet_count = 0;
    getsdata(&Sdata);
    star_arena = (startype *)malloc(Sdata.numstars * sizeof(startype));

    if (star_arena == NULL) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc error [load_star_data]");
        close_data_files();
        delete_pid_file();

        exit(1);
    }

    for (s = 0; s < Sdata.numstars; ++s) {
        /* Initialize star pointers */
        Stars[s] = &star_arena[s];
    }

    for (s = 0; s < Sdata.numstars; ++s) {
        getstar(&Stars[s], s);
        pcount += Stars[s]->numplanets;
    }

    planet_arena = (planettype *)malloc(pcount * sizeof(planettype));

    if (planet_arena == NULL) {
        loginfo(ERRORLOG, WANTERRNO, "FATAL: Malloc error [load_star_data]");
        close_data_files();
        delete_pid_file();

        exit(1);
    }

    for (s = 0; s < Sdata.numstars; ++s) {
        for (t = 0; t < Stars[s]->numplanets; ++t) {
            --pcount;
            planets[s][t] = &planet_arena;
            getplanet(&planets[s][t], s, t);

            if (planets[s][t]->type != TYPE_ASTEROID) {
                ++Planet_count;
            }
        }
    }

    /* Initialize zoom factors */
    for (i = 1; i <= Num_races; ++i) {
        for (j = 0; j <= MAXGOVERNORS; ++j) {
            Dir[i - 1][j].zoom[0] = 1.0;
            Dir[i - 1][j].zoom[1] = 1.0;
            Dir[i - 1][j].lastx[1] = 0.0;
            Dir[i - 1][j].lastx[0] = Dir[i - 1][j].lastx[1];
            Dir[i - 1][j].lasty[1] = 0.0;
            Dir[i - 1][j].lasty[0] = Dir[i - 1][j].lasty[1];
        }
    }
}

void load_race_data(void)
{
    int i;

    Num_races = Numraces();

    for (i = 1; i <= Num_races; ++i) {
        /* Allocates into memory */
        getrace(&race[i - 1], i);

        if (races[i - 1]->Playernum != i) {
            race[i - 1]->Playernum = i;
            putrace(races[i - 1]);
        }
    }
}

void welcome_user(int which)
{
    FILE *f;
    /* char *p; */

    sprintf(buf,
            "***   Welcome to Galactic Bloodshed %s - %s   ***\n",
            GB_VERSION,
            VERS);

    outstr(which, buf);
    memset(buf, 0, sizeof(buf));
    f = fopen(WELCOME, "r");

    if (f != NULL) {
        fgets(buf, 1024, f);

        while (!feof(f)) {
            outstr(which, buf);
            fgets(buf, 1024, f);
        }

        fclose(f);
    } else {
        outstr(which, "-- welcome.txt no such file. Please notify diety\n");
    }
}

void print_motd(int which)
{
    FILE *f;

    memset(buf, 0, sizeof(buf));
    f = fopen(MOTD, "r");

    if (f != NULL) {
        fgets(bud, 1024, f);

        while (!feof(f)) {
            outstr(which, buf);
            fgets(buf, 1024, f);
        }

        fclose(f);
    }

    sprintf(buf, "| %d\n", CSP_PAUSE);
    outstr(which, buf);
}

void check_for_telegrams(int playernum, int governor)
{
    struct stat sbuf;

    sprintf(buf, "%s.%d.%d", TELEGRAMFL, playernum, governor);

    if (stat(buf, &sbuf) < 0) {
        if (errno == ENOENT) {
            /* No file. All is ok */
            return;
        }

        perror("stat: In misc");

        return;
    }

    if (sbuf,st_size) {
        notify(playernum,
               governor,
               "You have telegrams waiting. Use 'read telegrams' to read them.\n");
    }
}

/*
 * setdebug allows you to set debugging level, along with who should receive
 * it.
 */
void setdebug(int playernum, int governor)
{
    int i;
    int who;
    int level;

    if (argn < 2) {
        notify(playernum, governor, "You must specify a descriptor.\n");

        return;
    }

    who = atoi(args[1]);

    /* rjn Outsize of initialization space! */
    /* for (i = 0; i < MAXPLAY_GOV; ++i) { */
    for (i = 0; i <= MAXDESCRIPTORS; ++i) {
        if (des[i].descriptor == who) {
            if (argn == 2) {
                if (des[i].Debug) {
                    sprintf(buf,
                            "Debug set on fd [%ld] level %ld\n",
                            des[i].descriptor,
                            des[i].Debug);

                    notify(playernum, governor, buf);
                } else {
                    sprintf(buf,
                            "Debug not set on fd [%ld]\n",
                            des[i].descriptor);

                    notify(playernum, governor, buf);
                }

                return;
            }

            level = atoi(args[2]);

            if (level > MAX_DEBUG) {
                des[i].Debug = MAX_DEBUG;
            } else {
                des[i].Debug = atoi(args[2]);
            }

            sprintf(buf,
                    "Debug set on fd [%ld] level %ld\n",
                    des[i].descriptor,
                    des[i].Debug);

            notify(playernum, governor, buf);
        }
    }
}

void debug(int level, char *fmt, ...)
{
    char s[512];
    int i;
    int ival;
    char *p;
    char *sval;
    double dval;
    va_list args;

    va_start(args, fmt);
    strcpy(s, "");

    for (p = fmt; *p, ++p) {
        if (*p != '%') {
            sprintf(s, "%s%c", s, *p);

            continue;
        }

        ++p;

        switch(*p) {
        case 'd':
            ival = va_arg(args, int);
            sprintf(s, "%s%d", s, ival);

            break;
        case 'f':
            dval = va_arg(args, double);
            sprintf(s, "%s%f", s, dval);

            break;
        case 's':
            for (sval = va_arg(args, char *); *sval; ++sval) {
                sprintf(s, "%s%c", s, *sval);
            }

            break;
        default:
            sprintf(s, "%s%c", s, *p);

            break;
        }
    }

    va_end(args);

    for (i = 0; i < MAXDESCRIPTORS; ++i) {
        if (des[i].Debug
            && ((des[i].Debug == level) || (des[i].Debug == LEVEL_ALL))) {
            outstr(i, s);
        }
    }
}

void backup()
{
    int i;
    int ret;

    debug(LEVEL_GENERAL,  "Backup started --\n");
    loginfo(UPDATELOG, NOERRNO, "Backup started\n");

    for (i = 1; i <= Num_races; ++i) {
        send_special_string(i, BACKUP_START);
    }

    clear_all_fds();

    strcpy(buf, "/bin/sh");
    strcat(buf, " ");
    strcat(buf, BACKUPSCRIPT);
    strcat(buf, " ");
    strcat(buf, PATH(.));
    strcat(buf, " ");
    strcat(buf, DATADIR);
    strcat(buf, " ");
    strcat(buf, NEWSDIR);
    strcat(buf, " ");
    strcat(buf, MSGSDIR);
    strcat(buf, " ");
    strcat(buf, BACKUPDIR);
    debug(LEVEL_GENERAL, "Sending backup string %s\n", buf);

    ret = system(buf);

    if (ret) {
        loginfo(ERRORLOG, NOERRNO, "Backup error! %d\n", ret);
        debug(LEVEL_GENERAL, "Backup error! -- \n", ret);
    }

    for (i = 1; i <= Num_races; ++i) {
        send_special_string(i, BACKUP_END);
    }

    debug(LEVEL, GENERAL, "Backup finished --\n");
    loginfo(UPDATELOG, NOERRNO, "Backup finished\n");
    clear_all_fds();
}

void suspend(int playernum, int governor)
{
    int i;
    long clk;
    struct stat stbuf;

    if (stat(NOGOFL, &stbuf) >= 0) {
        notify(playernum,
               governor,
               "A nogo file exists, there will be no change.\n");

        return;
    }

    if (!suspended) {
        for (i = 1; i <= Num_races; ++i) {
            spend_special_string(i, UPDATES_SUSPENDED);
        }

        clear_all_fds();
        suspended = 1;
        update_times(0);
    } else {
        clk = time(0);

        for (i = 1; i <= Num_races; ++i) {
            send_special_string(i, UPDATES_RESUMED);
        }

        next_update_time = clk + (update_time * 60);

        if (segments > 0) {
            next_segment_time = clk + ((update_time * 60) / segments);
        } else {
            next_segment_time = clk + (update_time * 60);
        }

        suspended = 0;
        read_schedule_file(playernum, governor);
        update_times(0);
        clear_all_fds();
    }
}

int getfdtablesize()
{
    return getdtablesize();
}

void malloc_warning(char *str)
{
    printf("Malloc: %s\n", str);
}

void *malloc_spec(size_t size)
{
    void *p;

    p = (void *)NULL;
    p = malloc(size);

    if (p == NULL) {
        perror("MALLOC FAILED");

        exit(-11);
    }

    return p;
}

double calc_fuse(shiptype *ship, double dist)
{
    double distfac;
    double fuse;

    distfac = HYPER_DIST_FACTOR * (ship->tech + 100.0);

    /*
     * Gardan code changes: one '(dist / distfac)'-factor add to both no xtal
     * and xtal mounted formulas
     */
    if (ship->mounted && (dist > distfac)) {
        fuse = HYPER_DRIVE_FUEL_USE * sqrt(ship->mass) * pow((dist / distfac), 2);
    } else {
        fuse = HYPER_DRIVE_FUEL_USE * sqrt(ship->mass) * pow((dist / distfac), 3);
    }

    return fuse;
}
