/*
 * Galactic Bloodshed, A multi-player 4X game of space conquest
 * Copyright (c) 1989-90 by Robert P. Chansky, et al.
 * See GB_copyright.h for additional authors and details.
 *
 * this program is free software; you can redistribute it and/or modify it under
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
 * *****************************************************************************
 * Galactic Bloodshed Fleet System
 *
 * Author: Michael F. Wilkinson (mfw)
 *
 * Contains:
 *   fleet() called from GB_server.c
 *   ancillary functions
 *
 * Take from an idea from HAP and adapted for GB+, see 'fleets' in concept.txt
 *
 * #ident  "@(#)fleet.c  1.0 6/28/01 "
 * *****************************************************************************
 */
#include "user_fleet.h"

#include <stdlib.h>
#include <string.h>

#include "files_shl.h"
#include "fleet.h"
#include "GB_server.h"
#include "power.h"
#include "races.h"
#include "ships.h"
#include "vars.h"

extern long Shipdata[NUMSTYPES][NUMABILS];
extern char const *Shipnames[];

int bad_flagship(int, int, int);
int fctofi(char);
char fitofc(int);

void user_fleet(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    shiptype *st;
    int i;
    int sh;
    int adm;
    int some = 0;
    char obuf[1024];
    char hbuf[80];
    char name[80];

    notify(playernum,
           governor,
           "\nOverview of Operational Fleets with Attached Vessels\n");

    notify(playernum,
           governor,
           "----------------------------------------------------\n");

    for (i = 1; i <= MAXFLEETS; ++i) {
        obuf[0] = '\0';
        sh = races[playernum - 1]->fleet[i].flagship;

        if (sh) {
            some = 1;
            strcpy(name, races[playernum - 1]->fleet[i].name);
            adm = races[playernum - 1]->fleet[i].admiral;

            if (name[0]) {
                sprintf(obuf,
                        "(%c) %18s - Admiral: \"%s\" [%d,%d]\n",
                        fitofc(i),
                        name,
                        races[playernum - 1]->governor[adm].name,
                        playernum,
                        adm);
            } else {
                sprintf(obuf,
                        "(%c) %12s Fleet - Admiral: \"%s\" [%d,%d]\n",
                        fitofc(i),
                        Fleetnames[i],
                        races[playernum - 1]->governor[adm].name,
                        playernum,
                        adm);
            }

            strcat(obuf, "                         Vessels: ");

            if (bad_flagship(playernum, governor, i)) {
                sh = 0;
                sprintf(hbuf, "Error with flagship (cleared).");
                strcat(obuf, hbuf);
            }

            while (sh) {
                if (getship(&st, sh)) {
                    sh = st->nextinfleet;

                    if (st->alive) {
                        sprintf(hbuf, "%c%d ", Shipltrs[st->type], st->number);
                    }

                    strcat(obuf, hbuf);
                    free(st);
                } else {
                    sh = 0;
                }
            }

            strcat(obuf, "\n");
            notify(playernum, governor, obuf);
        }
    }

    if (!some) {
        notify(playernum, governor, "No active fleets.\n");
    }

    notify(playernum, governor, "\n");
}

int bad_flagship(int player, int gov, int fl)
{
    racetype *r;
    shiptype *s;
    int flagship;

    r = races[player - 1];
    flagship = r->fleet[fl].flagship;

    if (!flagship) {
        /* No flagship, there is no error, return ok */
        return 0;
    }

    if (!getship(&s, flagship)) {
        /* Couldn't get the ship, we have a problem, clear it */
        r->fleet[fl].flagship = 0;
        r->fleet[fl].admiral = 0;
        putrace(r);

        return 1;
    }

    if (s->fleetmember != fl) {
        /* The fleet the ship thinks it's in is different, error, clear it */
        r->fleet[fl].flagship = 0;
        r->fleet[fl].admiral = 0;
        putrace(r);
        free(s);

        return 1;
    }

    free(s);

    return 0;
}

/* Fleet character to fleet integer */
int fctofi(char c)
{
    int i;

    if (c >= ('a' - 1)) {
        i = c - ('a' - 1);
    } else {
        i = c - ('A' - 1);
    }

    if ((i < 1) || (i > MAXFLEETS)) {
        return 0;
    } else {
        return i;
    }
}

/* Fleet integer to fleet character */
char fitofc(int i)
{
    if ((i < 1) || (i > MAXFLEETS)) {
        return (char)0;
    } else {
        return (i + ('A' - 1));
    }
}
