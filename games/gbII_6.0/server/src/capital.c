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
 * capital.c -- Designate a capital
 *
 * #ident  "@(#)capital.c       1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/capital.c,v 1.4 2007/07/06 18:09:34 gbp Exp $
 */
#include "capital.h"

#include <stdlib.h> /* Added for atoi() and free() (kse) */

#include "buffers.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "shlmisc.h"
#include "shipdata.h"
#include "ships.h"
#include "vars.h"

#include "fire.h"

void capital(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    int shipno;
    int stat;
    int snum;
    shiptype *s;
    racetype *race;

    race = races[playernum - 1];

    if (argn != 2) {
        shipno = race->Gov_ship;
    } else {
        if (args[1][0] == '#') {
            shipno = atoi(args[1] + 1);
        } else {
            shipno = atoi(args[1]);
        }
    }

    if (shipno <= 0) {
        notify(playernum, governor, "Change the capital to be what ship?\n");

        return;
    }

    stat = getship(&s, shipno);

    if (argn == 2) {
        snum = s->storbits;

        if (!stat || testship(playernum, governor, s)) {
            notify(playernum, governor, "You can't do that!\n");
            free(s);

            return;
        }

        if (!landed(s)) {
            notify(playernum, governor, "Try landing this ship first!\n");
            free(s);

            return;
        }

        if (!enufAP(playernum, governor, Stars[snum]->AP[playernum - 1], apcount)) {
            free(s);

            return;
        }

        if (s->type != OTYPE_GOV) {
            sprintf(buf, "That ship is not a %s.\n", Shipnames[OTYPE_GOV]);
            notify(playernum, governor, buf);
            free(s);

            return;
        }

        deductAPs(playernum, governor, apcount, snum, 0);
        race->Gov_ship = shipno;
        putrace(race);
    }

    if (s->type == OTYPE_FACTORY) {
        sprintf(buf,
                "Efficiency of governmental center: %.0f%%.\n",
                ((double)s->popn / (Shipdata[s->type][ABIL_MAXCREW] - s->troops))
                * (100 - (double)s->damage));
    } else {
        sprintf(buf,
                "Efficiency of governmental center: %.0f%%.\n",
                ((double)s->popn / (s->max_crew - s->troops))
                * (100 - (double)s->damage));
    }

    notify(playernum, governor, buf);
    free(s);
}
