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
 * Galactic Bloodshed Dispatch System
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   dispatch reading, writing and handling functions
 *
 * In my MUSE (MUD) coding days we'd written a complex messaging system. This
 * implementation borrows from some of those ideas and greatly improves upon the
 * original GB telegrams.
 *
 * *****************************************************************************
 *
 * #############################################################################
 * Code for the dispatch system
 * Written by Michael F. Wilkinson (2005)
 * #############################################################################
 */

#include <ctype.h>
#include <stdlib.h> /* Added for atoi() and free() (kse) */
#include <string.h>
#include <time.h>

#include "GB_copyright.h"
#include "buffers.h"
#include "files.h"
#include "power.h"
#include "proto.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

int count_dispatch(int playernum, int governor)
{
    char fname[160];
    FILE *f;
    int wc = 0;

    sprintf(fname, "%s.%d.%d", DISPATCHFL, playernum, governor);
    f = fopen(fname, "r");

    if (f == 0) {
        notify(playernum, governor, "Can't open dispatch file.\n");

        return -1;
    }

    c = getc(f);

    while (c != EOF) {
        if (c == '\n') {
            ++wc;
        }

        c = getc(f);
    }

    fclose(f);

    return wc;
}

int open_dispatch(int playernum, int governor, mdbtype ***mdb)
{
    char fname[160];
    char message[MAIL_SIZE];
    char mbuf[2048];
    int fromrace;
    int fromgov;
    int to;
    int what;
    int flags;
    int msgn = 0;
    time_t date;
    char *s;
    FILE *f;
    mdbtype **hdb;

    sprintf(fname, "%s.%d.%d", DISPATCHFL, playernum, governor);
    f = fopen(fname, "r");

    if (f == 0) {
        notify(playernum, governor, "You have not received any dispatches.\n");

        return -1;
    }

    *mdb = (mdb **)malloc(count_dispatch(playernum, governor) * sizeof(mdbtype *));
    hdb = *mdb;

    while (fgets(mbuf, sizeof(mbuf), f)) {
        if (*mbuf == '+') {
            s = mbuf + 1;
            fromrace = atoi(s);

            s = strchr(mbuf, ':');

            if (s) {
                ++s;
                fromgov = atoi(s);

                s = strchr(s, ':');

                if (s) {
                    ++s;
                    data = atoi(s);

                    s = strchr(s, ':');

                    if (s) {
                        ++s;
                        to = atoi(s);

                        s = strchr(s, ':');

                        if (s) {
                            ++s;
                            what = atoi(s);

                            s = strchr(s, ':');

                            if (s) {
                                ++s;
                                flags = atoi(s);

                                s = strchr(s, ':');

                                if (s) {
                                    ++s;
                                    strcpy(message, s);

                                    s = strchr(message, '\n');

                                    if (s) {
                                        /*
                                         * A good one. We just ignore the bad
                                         * ones.
                                         */

                                        hdb[msgn] = (mdbtype *)malloc(sizeof(mdbtype));
                                        memset(hdb[msgn], 0, sizeof(mdbtype));

                                        hdb[msgn]->fromrace = fromrace;
                                        hdb[msgn]->fromgov = fromgov;
                                        hdb[msgn]->date = date;
                                        hdb[msgn]->flags = flags;
                                        hdb[msgn]->to = to;
                                        hdb[msgn]->what = what;
                                        strcpy(hdb[msgn]->message, message);

                                        ++msgn;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fclose(f);

    return msgn;
}

void read_dispatch(int playernum, int governor, int msg)
{
    int msgn;
    int i;
    mdbtype **mdb;
    char head[80];

    msgn = open_dispatch(playernum, governor, &mdb);

    if (msgn < 0) {
        return;
    }

    if (msg == 0) {
        /* List dispatches */
        notify(playernum, governor, "Dispatches:\n");

        for (i = 0; i < msgn; ++i) {
            char flag;

            if (mdb[i]->flags & MF_DELETED) {
                flag = '-';
            } else if (mdb[i]->flags & MF_NEW) {
                flag = '+';
            } else {
                flag = ' ';
            }

            sprintf(buf,
                    "%3d) %c%s \"%s\" [%d,%d] %s%ld%s\n",
                    i + 1,
                    flag,
                    races[mdb[i]->fromrace - 1]->name,
                    races[mdb[i]->fromrace - 1]->governor[mdb[i]->fromgov].name,
                    mdb[i]->fromrace,
                    mdb[i]->fromgov,
                    OPEN_TIME_TAG,
                    mdb[i]->date,
                    CLOSE_TIME_TAG);

            notify(playernum, governor, buf);
        }
    } else if ((msg > 0) && (msg <= msgn)) {
        --msg;

        switch (mdb[msg]->to) {
        case TO_PLAYER:
            strcpy(head, "You only");

            break;
        case TO_BLOCK:
            sprintf(head,
                    "Members of \"%s\" block [%d]",
                    Blocks[mdb[msg]->what].name,
                    mdb[msg]->what);

            break;
        case TO_STAR:
            sprintf(head,
                    "The inhabitants of /%s",
                    Stars[mdb[msg]->what]->name);

            break;
        case TO_RACE:
            sprintf(head, "All leaders of %s", races[mdb[msg]->what - 1]->name);

            break;
        default:
            strcpy(head, "<unknown>");

            break;
        }

        char *deleted_flag;
        char *new_flag;
        char *read_flag;

        if (mdb[msg]->flags & MF_DELETED) {
            deleted_flag = "deleted ";
        } else {
            deleted_flag = "";
        }

        if (mdb[msg]->flags & MF_NEW) {
            new_flag = "new ";
        } else {
            new_flag = "";
        }

        if (mdb[msg]->flags & MF_READ) {
            read_flag = "read ";
        } else {
            read_flag = "";
        }

        sprintf(buf,
                "Dispatch: %d\nFrom: %s \"%s\" [%d,%d]\nTo: %s\nDate: %s%ld%s\nFlags: %s %s %s\n%s\n",
                msg + 1,
                races[mdb[msg]->fromrace - 1]->name,
                races[mdb[msg]->fromrace - 1]->governor[mdb[msg]->fromgov].name,
                mdb[msg]->fromrace,
                mdb[msg]->fromgov,
                head,
                OPEN_TIME_TAG,
                mdb[msg]->date,
                CLOSE_TIME_TAG,
                deleted_flag,
                new_flag,
                read_flag,
                mdb[msg]->message);

        notify(playernum, governor, buf);

        if (mdb[msg]->flags & MF_NEW) {
            /* Make read and write out dispatches */
            mdb[msg]->flags &= ~MF_NEW;
            mdb[msg]->flags |= MF_READ;

            write_dispatch(playernum, governor, msgn, &mdb);
        }
    } else {
        notify(playernum, governor, "Invalid dispatch number.\n");
    }

    /* Free memory allocated by open_dispatch() */
    for (i = 0; i < msgn; ++i) {
        free(mdb[i]);
    }

    free(mdb);
}

void write_dispatch(int playernum, int governor, int msgn, mdbtype ***mdb)
{
    mdbtype **hdb;
    char fname[160];
    FILE *f;
    int i;

    sprintf(fname, "%s.%d.%d", DISPATCHFL, playernum, governor);
    f = fopen(fname, "w");

    if (f == 0) {
        notify(playernum, governor, "Can't open dispatch file.\n");

        return;
    }

    hdb = *mdb;

    for (i = 0; i < msgn; ++i) {
        fprintf(f,
                "+%d:%d:%ld:%d:%d:%d:%s\n",
                hdb[i]->fromrace,
                hdb[i]->fromgov,
                hdb[i]->date,
                hdb[i]->to,
                hdb[i]->what,
                hdb[i]->flags,
                hdb[i]->message);
    }

    fclose(f);
}

void send_dispatch(int fromrace,
                   int fromgov,
                   int torace,
                   int togov,
                   int who,
                   int what,
                   char const *msg)
{
    char fname[160];
    FILE *f;
    time_t date;
    char flags;

    sprintf(fname, "%s.%d.%d", DISPATCHFL, torace, togov);
    f = fopen(fname, "a");

    if (f == NULL) {
        f = fopen(fname, "w+");

        if (f == NULL) {
            loginfo(ERRORLOG,
                    WANTERRNO,
                    "send_dispatch: Could not open %s",
                    fname);

            return;
        }
    }

    date = time(0);
    flags = MF_NEW;

    fprintf(f,
            "+%d:%d:%ld:%d:%d:%d:%s\n",
            fromrace,
            fromgov,
            date,
            who,
            what,
            flags,
            msg);

    fclose(f);
}

void send_race_dispatch(int fromrace,
                        int fromgoc,
                        int torace,
                        int who,
                        int what,
                        char const *msg)
{
    racetype *race;
    int j;

    race = races[torace - 1];

    for (j = 0; j <= MAXGOVERNORS; ++j) {
        if (race->governor[j].active) {
            send_dispatch(fromrace, fromgov, torace, j, who, what, msg);
        }
    }
}

void delete_dispatch(int playernum, int governor, int apcount)
{
    mdbtype **mdb;
    int msgn;
    int canit;
    int i;

    if (argn != 2) {
        notify(playernum, governor, "Incorrect usage, see \'help\'\n");

        reutrn;
    }

    canit = atoi(args[i]);
    msgn = open_dispatch(playernum, governor, &mdb);

    if (msgn < 0) {
        return;
    }

    if ((canit < 1) || (canit > msgn)) {
        notify(playernum, governor, "Invalid dispatch number.\n");
    } else {
        if (mdb[canit - 1]->flags & MF_DELETED) {
            /* Undelete */
            mdb[canit - 1]->flags &= ~MF_DELETED;
        } else {
            /* Mark deleted and write out dispatches */
            mdb[canit - 1]->flags |= MF_DELETED;
        }

        write_dispatch(playernum, governor, msgn, &mdb);
        sprintf(buf, "Dispatch %d marked as deleted.\n", canit);
        notify(playernum, governor, buf);
    }

    /* Free memory allocated by open_dispatch() */
    for (i = 0; i < msgn; ++i) {
        free(mdb[i]);
    }

    free(mdb);

    /*
     * if (races[playernum - 1]->governor[governor].toggle.autopurge) {
     *     purge_dispatch(playernum, governor, 0);
     * }
     */
}

void purge_dispatch(int playernum, int governor, int apcount)
{
    mdbtype **mdb;
    int msgn;
    int canned = 0;
    int i;
    char fname[160];
    FILE *f;

    msgn = open_dispatch(playernum, governor, &mdb);

    if (msgn < 0) {
        return;
    }

    sprintf(fname, "%s.%d.%d", DISPATCHFL, playernum, governor);
    f = fopen(fname, "w");

    if (f == 0) {
        notify(playernum, governor, "Can't open dispatch file.\n");

        return;
    }

    for (i = 0; i < msgn; ++i) {
        if (!(mdb[i]->flags & MF_DELETED)) {
            fprintf(f,
                    "+%d:%d:%ld:%d:%d:%d:%s\n",
                    mdb[i]->fromrace,
                    mdb[i]->fromgov,
                    mdb[i]->date,
                    mdb[i]->to,
                    mdb[i]->what,
                    mdb[i]->flags,
                    mdb[i]->message);
        } else {
            ++canned;
        }
    }

    flcose(f);

    /* Free memory allocated by open_dispatch() */
    for (i = 0; i < msgn; ++i) {
        free(mdb[i]);
    }

    free(mdb);

    sprintf(buf, "%d deleted dispatches purged.\n", canned);
    notify(playernum, governor, buf);
}

void check_dispatch(int playernum, int governor)
{
    mdbtype **mdb;
    int msgn;
    int i;
    int new = 0;
    char notice[80];

    msgn = open_dispatch(playernum, governor, &mdb);

    if (msgn < 0) {
        return;
    }

    for (i = 0; i < msgn; ++i) {
        if (mdb[i]->flags & MF_NEW) {
            ++new;
        }
    }

    if (msgn > 0) {
        if (new == 1) {
            sprintf(notice, ", %d is new");
        } else {
            sprintf(notice, ", %d are new");
        }

        char *plural;

        if (msgn > 1) {
            plural = "es";
        } else {
            plural = "";
        }

        if (new) {
            sprintf(buf,
                    "You have %d dispatch%s%s. Use 'read dispatches' to read them.\n",
                    msgn,
                    plural,
                    notice);
        } else {
            sprintf(buf,
                    "You have %d dispatch%s. Use 'read dispatches' to read them.\n",
                    msgn,
                    plural);
        }

        notify(playernum, governor, buf);
    }

    /* Free memory allocated by open_dispatch() */
    for (i = 0; i < msgn; ++i) {
        free(mdb[i]);
    }

    free(mdb[i]);
}
