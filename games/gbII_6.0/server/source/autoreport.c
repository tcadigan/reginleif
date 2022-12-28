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
 * autoreport.c -- Tell server to generate a report for each planet
 *
 * #ident  "@(#)autoreport.c    1.8 12/3/93 "
 *
 * $Header: /var/cvs/gbp/GB+/user/autoreport.c,v 1.3 2007/07/06 18:09:34 gbp Exp
 * $
 */
#include "autoreport.h"

#include <stdlib.h> /* Added for free() (kse) */

#include "buffers.h"
#include "files_shl.h"
#include "GB_server.h"
#include "getplace.h"
#include "power.h"
#include "races.h"
#include "ranks.h"
#include "ships.h"
#include "vars.h"

void autoreport(int playernum, int governor, int apcount, int unused4, orbitinfo *unused5)
{
    planettype *p;
    placetype place;
    int snum;
    int pnum;

    snum = Dir[playernum - 1][governor].snum;
    pnum = Dir[playernum - 1][governor].pnum;

    if (argn == 1) {
        /* No args */
        if (Dir[playernum - 1][governor].level == LEVEL_PLAN) {
            getplanet(&p, snum, pnum);

            if (p->info[playernum - 1].autorep) {
                p->info[playernum - 1].autorep = 0;
            } else {
                p->info[playernum - 1].autorep = TELEG_MAX_AUTO;
            }

            putplanet(p, snum, pnum);

            if (p->info[playernum - 1].autorep) {
                sprintf(buf,
                        "Autoreport on %s has been set.\n",
                        Stars[snum]->pnames[pnum]);
            } else {
                sprintf(buf,
                        "Autorepor on %s has been unset.\n",
                        Stars[snum]->pnames[pnum]);
            }

            notify(playernum, governor, buf);
            free(p);
        } else {
            sprintf(buf, "Scope must be a planet.\n");
            notify(playernum, governor, buf);
        }
    } else if (argn > 1) {
        /* argn == 2, place specified */
        place = Getplace(playernum, governor, args[1], 0);

        if (place.level == LEVEL_PLAN) {
            getplanet(&p, snum, pnum);

            if (p->info[playernum - 1].autorep) {
                sprintf(buf,
                        "Autoreport on %s has been set",
                        Stars[snum]->pnames[pnum]);
            } else {
                sprintf(buf,
                        "Autoreport on %s has been unset",
                        Stars[snum]->pnames[pnum]);
            }

            notify(playernum, governor, buf);
            p->info[playernum - 1].autorep = !p->info[playernum - 1].autorep;
            putplanet(p, snum, pnum);
            free(p);
        } else {
            sprintf(buf, "Scope must be a planet.\n");
            notify(playernum, governor, buf);
        }
    }
}
